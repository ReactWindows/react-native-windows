// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <DynamicReader.h>
#include <JSValueWriter.h>
#include <JsiWriter.h>
#include <Views/SIPEventHandler.h>
#include <Views/ShadowNodeBase.h>
#include "Impl/ScrollViewUWPImplementation.h"
#include "ScrollViewManager.h"

using namespace winrt::Microsoft::ReactNative;

namespace Microsoft::ReactNative {

enum class CoalesceType {
  CoalesceByTag,
  Durable,
};

namespace ScrollViewCommands {
constexpr const char *ScrollTo = "scrollTo";
constexpr const char *ScrollToEnd = "scrollToEnd";
}; // namespace ScrollViewCommands

class ScrollViewShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  ScrollViewShadowNode();
  ~ScrollViewShadowNode();
  void dispatchCommand(const std::string &commandId, winrt::Microsoft::ReactNative::JSValueArray &&commandArgs)
      override;
  void createView(const winrt::Microsoft::ReactNative::JSValueObject &) override;
  void updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) override;

 private:
  void AddHandlers(const winrt::ScrollViewer &scrollViewer);
  void EmitScrollEvent(
      const winrt::ScrollViewer &scrollViewer,
      int64_t tag,
      winrt::hstring &&eventName,
      double x,
      double y,
      double zoom,
      CoalesceType coalesceType);
  template <typename T>
  std::tuple<bool, T> getPropertyAndValidity(
      const winrt::Microsoft::ReactNative::JSValue &propertyValue,
      T defaultValue);
  void SetScrollMode(const winrt::ScrollViewer &scrollViewer);
  void UpdateZoomMode(const winrt::ScrollViewer &scrollViewer);

  float m_zoomFactor{1.0f};
  bool m_isScrollingFromInertia = false;
  bool m_isScrolling = false;
  bool m_isHorizontal = false;
  bool m_isScrollingEnabled = true;
  bool m_changeViewAfterLoaded = false;
  bool m_dismissKeyboardOnDrag = false;

  std::shared_ptr<SIPEventHandler> m_SIPEventHandler;

  xaml::FrameworkElement::SizeChanged_revoker m_scrollViewerSizeChangedRevoker{};
  xaml::FrameworkElement::SizeChanged_revoker m_contentSizeChangedRevoker{};
  winrt::ScrollViewer::ViewChanged_revoker m_scrollViewerViewChangedRevoker{};
  winrt::ScrollViewer::ViewChanging_revoker m_scrollViewerViewChangingRevoker{};
  winrt::ScrollViewer::DirectManipulationCompleted_revoker m_scrollViewerDirectManipulationCompletedRevoker{};
  winrt::ScrollViewer::DirectManipulationStarted_revoker m_scrollViewerDirectManipulationStartedRevoker{};
  xaml::Controls::Control::Loaded_revoker m_controlLoadedRevoker{};
};

ScrollViewShadowNode::ScrollViewShadowNode() {}

ScrollViewShadowNode::~ScrollViewShadowNode() {
  m_SIPEventHandler.reset();
}

void ScrollViewShadowNode::dispatchCommand(
    const std::string &commandId,
    winrt::Microsoft::ReactNative::JSValueArray &&commandArgs) {
  const auto scrollViewer = GetView().as<winrt::ScrollViewer>();
  if (scrollViewer == nullptr)
    return;

  if (commandId == ScrollViewCommands::ScrollTo) {
    double x = commandArgs[0].AsDouble();
    double y = commandArgs[1].AsDouble();
    bool animated = commandArgs[2].AsBoolean();
    scrollViewer.ChangeView(x, y, nullptr, !animated /*disableAnimation*/);
  } else if (commandId == ScrollViewCommands::ScrollToEnd) {
    bool animated = commandArgs[0].AsBoolean();
    bool horiz = scrollViewer.HorizontalScrollMode() == winrt::ScrollMode::Auto;
    if (horiz)
      scrollViewer.ChangeView(scrollViewer.ScrollableWidth(), nullptr, nullptr, !animated /*disableAnimation*/);
    else
      scrollViewer.ChangeView(nullptr, scrollViewer.ScrollableHeight(), nullptr, !animated /*disableAnimation*/);
  }
}

void ScrollViewShadowNode::createView(const winrt::Microsoft::ReactNative::JSValueObject &props) {
  Super::createView(props);

  const auto scrollViewer = GetView().as<winrt::ScrollViewer>();
  const auto scrollViewUWPImplementation = ScrollViewUWPImplementation(scrollViewer);
  scrollViewUWPImplementation.ScrollViewerSnapPointManager();

  AddHandlers(scrollViewer);

  m_scrollViewerSizeChangedRevoker =
      scrollViewer.SizeChanged(winrt::auto_revoke, [scrollViewUWPImplementation](const auto &, const auto &) {
        scrollViewUWPImplementation.UpdateScrollableSize();
      });

  m_scrollViewerViewChangedRevoker = scrollViewer.ViewChanged(
      winrt::auto_revoke, [this, scrollViewUWPImplementation](const auto &sender, const auto & /*args*/) {
        const auto scrollViewerNotNull{sender.as<winrt::ScrollViewer>()};
        const auto zoomFactor{scrollViewerNotNull.ZoomFactor()};
        if (m_zoomFactor != zoomFactor) {
          m_zoomFactor = zoomFactor;
          scrollViewUWPImplementation.UpdateScrollableSize();
        }
      });

  m_contentSizeChangedRevoker = scrollViewUWPImplementation.ScrollViewerSnapPointManager()->SizeChanged(
      winrt::auto_revoke, [this, scrollViewUWPImplementation](const auto &, const auto &) {
        scrollViewUWPImplementation.UpdateScrollableSize();
      });
}

void ScrollViewShadowNode::updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) {
  m_updating = true;

  const auto scrollViewer = GetView().as<winrt::ScrollViewer>();
  if (scrollViewer == nullptr)
    return;

  for (const auto &pair : props) {
    const std::string &propertyName = pair.first;
    const auto &propertyValue = pair.second;

    if (propertyName == "horizontal") {
      const auto [valid, horizontal] = getPropertyAndValidity(propertyValue, false);
      if (valid) {
        m_isHorizontal = horizontal;
        ScrollViewUWPImplementation(scrollViewer).SetHorizontal(horizontal);
        SetScrollMode(scrollViewer);
      }
    }
    if (propertyName == "scrollEnabled") {
      const auto [valid, scrollEnabled] = getPropertyAndValidity(propertyValue, true);
      if (valid) {
        m_isScrollingEnabled = scrollEnabled;
        SetScrollMode(scrollViewer);
      }
    } else if (propertyName == "showsHorizontalScrollIndicator") {
      const auto [valid, showsHorizontalScrollIndicator] = getPropertyAndValidity(propertyValue, true);
      if (valid) {
        scrollViewer.HorizontalScrollBarVisibility(
            showsHorizontalScrollIndicator ? winrt::ScrollBarVisibility::Visible : winrt::ScrollBarVisibility::Hidden);
      }
    } else if (propertyName == "showsVerticalScrollIndicator") {
      const auto [valid, showsVerticalScrollIndicator] = getPropertyAndValidity(propertyValue, true);
      if (valid) {
        scrollViewer.VerticalScrollBarVisibility(
            showsVerticalScrollIndicator ? winrt::ScrollBarVisibility::Visible : winrt::ScrollBarVisibility::Hidden);
      }
    } else if (propertyName == "minimumZoomScale") {
      const auto [valid, minimumZoomScale] = getPropertyAndValidity(propertyValue, 1.0);
      if (valid) {
        scrollViewer.MinZoomFactor(static_cast<float>(minimumZoomScale));
        UpdateZoomMode(scrollViewer);
      }
    } else if (propertyName == "maximumZoomScale") {
      const auto [valid, maximumZoomScale] = getPropertyAndValidity(propertyValue, 1.0);
      if (valid) {
        scrollViewer.MaxZoomFactor(static_cast<float>(maximumZoomScale));
        UpdateZoomMode(scrollViewer);
      }
    } else if (propertyName == "zoomScale") {
      const auto [valid, zoomScale] = getPropertyAndValidity(propertyValue, 1.0);
      if (valid) {
        m_zoomFactor = static_cast<float>(zoomScale);
        m_changeViewAfterLoaded = !scrollViewer.ChangeView(nullptr, nullptr, m_zoomFactor);
      }
    } else if (propertyName == "snapToInterval") {
      const auto [valid, snapToInterval] = getPropertyAndValidity(propertyValue, 0.0);
      if (valid) {
        ScrollViewUWPImplementation(scrollViewer).SnapToInterval(static_cast<float>(snapToInterval));
      }
    } else if (propertyName == "snapToOffsets") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Array) {
        const auto snapToOffsets = winrt::single_threaded_vector<float>();
        for (const auto &val : propertyValue.AsArray()) {
          if (val.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
              val.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
            snapToOffsets.Append(val.AsSingle());
        }
        ScrollViewUWPImplementation(scrollViewer).SnapToOffsets(snapToOffsets.GetView());
      }
    } else if (propertyName == "snapToStart") {
      const auto [valid, snaptoStart] = getPropertyAndValidity(propertyValue, true);
      if (valid) {
        ScrollViewUWPImplementation(scrollViewer).SnapToStart(snaptoStart);
      }
    } else if (propertyName == "snapToEnd") {
      const auto [valid, snapToEnd] = getPropertyAndValidity(propertyValue, true);
      if (valid) {
        ScrollViewUWPImplementation(scrollViewer).SnapToEnd(snapToEnd);
      }
    } else if (propertyName == "keyboardDismissMode") {
      m_dismissKeyboardOnDrag = false;
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
        m_dismissKeyboardOnDrag = (propertyValue.AsString() == "on-drag");
        if (m_dismissKeyboardOnDrag) {
          m_SIPEventHandler = std::make_unique<SIPEventHandler>(GetViewManager()->GetReactContext());
          m_SIPEventHandler->AttachView(GetView(), false /*fireKeyboardEvents*/);
        }
      }
    } else if (propertyName == "snapToAlignment") {
      const auto [valid, snapToAlignment] = getPropertyAndValidity(propertyValue, winrt::SnapPointsAlignment::Near);
      if (valid) {
        ScrollViewUWPImplementation(scrollViewer).SnapPointAlignment(snapToAlignment);
      }
    } else if (propertyName == "pagingEnabled") {
      const auto [valid, pagingEnabled] = getPropertyAndValidity(propertyValue, false);
      if (valid) {
        ScrollViewUWPImplementation(scrollViewer).PagingEnabled(pagingEnabled);
      }
    }
  }

  Super::updateProperties(props);
  m_updating = false;
}

void ScrollViewShadowNode::AddHandlers(const winrt::ScrollViewer &scrollViewer) {
  m_scrollViewerViewChangingRevoker =
      scrollViewer.ViewChanging(winrt::auto_revoke, [this](const auto &sender, const auto &args) {
        const auto scrollViewerNotNull = sender.as<winrt::ScrollViewer>();

        // If we are transitioning to inertial scrolling.
        if (m_isScrolling && !m_isScrollingFromInertia && args.IsInertial()) {
          m_isScrollingFromInertia = true;

          EmitScrollEvent(
              scrollViewerNotNull,
              m_tag,
              L"topScrollEndDrag",
              args.NextView().HorizontalOffset(),
              args.NextView().VerticalOffset(),
              args.NextView().ZoomFactor(),
              CoalesceType::Durable);

          EmitScrollEvent(
              scrollViewerNotNull,
              m_tag,
              L"topScrollBeginMomentum",
              args.NextView().HorizontalOffset(),
              args.NextView().VerticalOffset(),
              args.NextView().ZoomFactor(),
              CoalesceType::Durable);
        }

        EmitScrollEvent(
            scrollViewerNotNull,
            m_tag,
            L"topScroll",
            args.NextView().HorizontalOffset(),
            args.NextView().VerticalOffset(),
            args.NextView().ZoomFactor(),
            CoalesceType::CoalesceByTag);
      });

  m_scrollViewerDirectManipulationStartedRevoker =
      scrollViewer.DirectManipulationStarted(winrt::auto_revoke, [this](const auto &sender, const auto &) {
        m_isScrolling = true;

        if (m_dismissKeyboardOnDrag && m_SIPEventHandler) {
          m_SIPEventHandler->TryHide();
        }

        const auto scrollViewer = sender.as<winrt::ScrollViewer>();
        EmitScrollEvent(
            scrollViewer,
            m_tag,
            L"topScrollBeginDrag",
            scrollViewer.HorizontalOffset(),
            scrollViewer.VerticalOffset(),
            scrollViewer.ZoomFactor(),
            CoalesceType::Durable);
      });

  m_scrollViewerDirectManipulationCompletedRevoker =
      scrollViewer.DirectManipulationCompleted(winrt::auto_revoke, [this](const auto &sender, const auto &) {
        const auto scrollViewer = sender.as<winrt::ScrollViewer>();
        if (m_isScrollingFromInertia) {
          EmitScrollEvent(
              scrollViewer,
              m_tag,
              L"topScrollEndMomentum",
              scrollViewer.HorizontalOffset(),
              scrollViewer.VerticalOffset(),
              scrollViewer.ZoomFactor(),
              CoalesceType::Durable);
        } else {
          EmitScrollEvent(
              scrollViewer,
              m_tag,
              L"topScrollEndDrag",
              scrollViewer.HorizontalOffset(),
              scrollViewer.VerticalOffset(),
              scrollViewer.ZoomFactor(),
              CoalesceType::Durable);
        }

        m_isScrolling = false;
        m_isScrollingFromInertia = false;
      });
  m_controlLoadedRevoker = scrollViewer.Loaded(winrt::auto_revoke, [this](const auto &sender, const auto &) {
    if (m_changeViewAfterLoaded) {
      const auto scrollViewer = sender.as<winrt::ScrollViewer>();
      scrollViewer.ChangeView(nullptr, nullptr, static_cast<float>(m_zoomFactor));
      m_changeViewAfterLoaded = false;
    }
  });
}

void ScrollViewShadowNode::EmitScrollEvent(
    const winrt::ScrollViewer &scrollViewer,
    int64_t tag,
    winrt::hstring &&eventName,
    double x,
    double y,
    double zoom,
    CoalesceType coalesceType) {
  const auto scrollViewerNotNull = scrollViewer;

  JSValueObject contentOffset{{"x", x}, {"y", y}};
  JSValueObject contentInset{{"left", 0}, {"top", 0}, {"right", 0}, {"bottom", 0}};

  JSValueObject contentSize{
      {"width", scrollViewerNotNull.ExtentWidth()}, {"height", scrollViewerNotNull.ExtentHeight()}};

  JSValueObject layoutMeasurement{
      {"width", scrollViewerNotNull.ActualWidth()}, {"height", scrollViewerNotNull.ActualHeight()}};

  JSValueObject eventJson{
      {"target", tag},
      {"responderIgnoreScroll", true},
      {"contentOffset", std::move(contentOffset)},
      {"contentInset", std::move(contentInset)},
      {"contentSize", std::move(contentSize)},
      {"layoutMeasurement", std::move(layoutMeasurement)},
      {"zoomScale", zoom}};

  auto *viewManager = static_cast<ScrollViewManager *>(GetViewManager());

  if (coalesceType == CoalesceType::CoalesceByTag) {
    viewManager->BatchingEmitter().DispatchCoalescingEvent(
        tag, std::move(eventName), MakeJSValueWriter(std::move(eventJson)));
  } else {
    viewManager->BatchingEmitter().DispatchEvent(tag, std::move(eventName), MakeJSValueWriter(std::move(eventJson)));
  }
} // namespace Microsoft::ReactNative

template <typename T>
std::tuple<bool, T> ScrollViewShadowNode::getPropertyAndValidity(
    const winrt::Microsoft::ReactNative::JSValue &propertyValue,
    T defaultValue) {
  if (propertyValue.IsNull()) {
    return std::make_tuple(true, defaultValue);
  }

  if constexpr (std::is_same<T, bool>()) {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean) {
      return std::make_tuple(true, propertyValue.AsBoolean());
    }
  }

  if constexpr (std::is_same<T, double>()) {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
      return std::make_tuple(true, propertyValue.AsDouble());
    }
  }

  if constexpr (std::is_same<T, winrt::SnapPointsAlignment>()) {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
      return std::make_tuple(true, static_cast<T>([snapToAlignment = propertyValue.AsString()]() {
                               if (snapToAlignment == "end") {
                                 return winrt::SnapPointsAlignment::Far;
                               } else if (snapToAlignment == "center") {
                                 return winrt::SnapPointsAlignment::Center;
                               } else {
                                 return winrt::SnapPointsAlignment::Near;
                               }
                             }()));
    }
  }

  return std::make_tuple(false, defaultValue);
}

void ScrollViewShadowNode::SetScrollMode(const winrt::ScrollViewer &scrollViewer) {
  const auto horizontalScrollingEnabled = m_isScrollingEnabled && m_isHorizontal;
  const auto verticalScrollingEnabled = m_isScrollingEnabled && !m_isHorizontal;
  scrollViewer.HorizontalScrollMode(horizontalScrollingEnabled ? winrt::ScrollMode::Auto : winrt::ScrollMode::Disabled);
  scrollViewer.VerticalScrollMode(verticalScrollingEnabled ? winrt::ScrollMode::Auto : winrt::ScrollMode::Disabled);
}

void ScrollViewShadowNode::UpdateZoomMode(const winrt::ScrollViewer &scrollViewer) {
  scrollViewer.ZoomMode(
      scrollViewer.MinZoomFactor() != scrollViewer.MaxZoomFactor() ? winrt::ZoomMode::Enabled
                                                                   : winrt::ZoomMode::Disabled);
}

ScrollViewManager::ScrollViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *ScrollViewManager::GetName() const {
  return L"RCTScrollView";
}

void ScrollViewManager::GetCommands(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  // Upstream JS will dispatch the string directly instead of ever actually calling this, but providing a real
  // implementation is simple enough in case anything changes.
  writer.WritePropertyName(Microsoft::Common::Unicode::Utf8ToUtf16(ScrollViewCommands::ScrollTo));
  writer.WriteString(Microsoft::Common::Unicode::Utf8ToUtf16(ScrollViewCommands::ScrollTo));
  writer.WritePropertyName(Microsoft::Common::Unicode::Utf8ToUtf16(ScrollViewCommands::ScrollToEnd));
  writer.WriteString(Microsoft::Common::Unicode::Utf8ToUtf16(ScrollViewCommands::ScrollToEnd));
}

void ScrollViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetNativeProps(writer);
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"horizontal", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"scrollEnabled", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"showsHorizontalScrollIndicator", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"showsVerticalScrollIndicator", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"minimumZoomScale", L"float");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"maximumZoomScale", L"float");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"zoomScale", L"float");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"snapToInterval", L"float");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"snapToOffsets", L"array");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"snapToAlignment", L"number");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"snapToStart", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"snapToEnd", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"pagingEnabled", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"keyboardDismissMode", L"string");
}

ShadowNode *ScrollViewManager::createShadow() const {
  return new ScrollViewShadowNode();
}

void ScrollViewManager::GetExportedCustomDirectEventTypeConstants(
    const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetExportedCustomDirectEventTypeConstants(writer);

  writer.WritePropertyName(L"topScrollBeginMomentum");
  writer.WriteObjectBegin();
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"registrationName", L"onMomentumScrollBegin");
  writer.WriteObjectEnd();

  writer.WritePropertyName(L"topScrollEndMomentum");
  writer.WriteObjectBegin();
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"registrationName", L"onMomentumScrollEnd");
  writer.WriteObjectEnd();

  writer.WritePropertyName(L"topScrollBeginDrag");
  writer.WriteObjectBegin();
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"registrationName", L"onScrollBeginDrag");
  writer.WriteObjectEnd();

  writer.WritePropertyName(L"topScrollEndDrag");
  writer.WriteObjectBegin();
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"registrationName", L"onScrollEndDrag");
  writer.WriteObjectEnd();

  writer.WritePropertyName(L"topScroll");
  writer.WriteObjectBegin();
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"registrationName", L"onScroll");
  writer.WriteObjectEnd();
}

XamlView ScrollViewManager::CreateViewCore(int64_t /*tag*/, const winrt::Microsoft::ReactNative::JSValueObject &) {
  const auto scrollViewer = winrt::ScrollViewer{};

  scrollViewer.HorizontalScrollBarVisibility(winrt::ScrollBarVisibility::Auto);
  scrollViewer.VerticalScrollBarVisibility(winrt::ScrollBarVisibility::Auto);
  scrollViewer.VerticalSnapPointsAlignment(winrt::SnapPointsAlignment::Near);
  scrollViewer.VerticalSnapPointsType(winrt::SnapPointsType::Mandatory);
  scrollViewer.HorizontalSnapPointsType(winrt::SnapPointsType::Mandatory);
  scrollViewer.HorizontalScrollMode(winrt::ScrollMode::Disabled);

  const auto snapPointManager = SnapPointManagingContentControl::Create();
  scrollViewer.Content(*snapPointManager);

  return scrollViewer;
}

void ScrollViewManager::AddView(const XamlView &parent, const XamlView &child, [[maybe_unused]] int64_t index) {
  assert(index == 0);

  auto scrollViewer = parent.as<winrt::ScrollViewer>();
  auto snapPointManager = scrollViewer.Content().as<SnapPointManagingContentControl>();
  snapPointManager->Content(child);
}

void ScrollViewManager::RemoveAllChildren(const XamlView &parent) {
  auto scrollViewer = parent.as<winrt::ScrollViewer>();
  auto snapPointManager = scrollViewer.Content().as<SnapPointManagingContentControl>();
  snapPointManager->Content(nullptr);
}

void ScrollViewManager::RemoveChildAt(const XamlView &parent, [[maybe_unused]] int64_t index) {
  assert(index == 0);
  RemoveAllChildren(parent);
}

void ScrollViewManager::SnapToInterval(const XamlView &parent, float interval) {
  if (parent) {
    if (const auto scrollViewer = parent.as<winrt::ScrollViewer>()) {
      ScrollViewUWPImplementation(scrollViewer).SnapToInterval(interval);
    }
  }
}

void ScrollViewManager::SnapToOffsets(const XamlView &parent, const winrt::IVectorView<float> &offsets) {
  if (parent) {
    if (const auto scrollViewer = parent.as<winrt::ScrollViewer>()) {
      ScrollViewUWPImplementation(scrollViewer).SnapToOffsets(offsets);
    }
  }
}

BatchingEventEmitter &ScrollViewManager::BatchingEmitter() noexcept {
  return *m_batchingEventEmitter;
}

} // namespace Microsoft::ReactNative
