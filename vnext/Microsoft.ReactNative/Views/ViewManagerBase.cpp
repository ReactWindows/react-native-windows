// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ViewManagerBase.h>

#include "Unicode.h"
#include "ViewPanel.h"
#include "cdebug.h"

#include <Modules/NativeUIManager.h>

#include <IReactInstance.h>
#include <IXamlRootView.h>
#include <Modules/PaperUIManagerModule.h>
#include <ReactPropertyBag.h>
#include <TestHook.h>
#include <Views/ExpressionAnimationStore.h>
#include <Views/ShadowNodeBase.h>

namespace winrt {
using namespace xaml;
}

namespace Microsoft::ReactNative {

float GetConstrainedResult(float constrainTo, float measuredSize, YGMeasureMode measureMode) {
  // Round up to workaround truncation inside yoga
  measuredSize = ceil(measuredSize);

  if (measureMode == YGMeasureMode::YGMeasureModeExactly)
    return constrainTo;
  if (measureMode == YGMeasureMode::YGMeasureModeAtMost)
    return std::min(constrainTo, measuredSize);
  return measuredSize;
}

YGSize DefaultYogaSelfMeasureFunc(
    YGNodeRef node,
    float width,
    YGMeasureMode widthMode,
    float height,
    YGMeasureMode heightMode) {
  YogaContext *context = reinterpret_cast<YogaContext *>(YGNodeGetContext(node));

  // TODO: VEC context != nullptr, DefaultYogaSelfMeasureFunc expects a context.

  XamlView view = context->view;
  auto element = view.as<xaml::UIElement>();

  float constrainToWidth =
      widthMode == YGMeasureMode::YGMeasureModeUndefined ? std::numeric_limits<float>::max() : width;
  float constrainToHeight =
      heightMode == YGMeasureMode::YGMeasureModeUndefined ? std::numeric_limits<float>::max() : height;

  try {
    winrt::Windows::Foundation::Size availableSpace(constrainToWidth, constrainToHeight);

    // Clear out current size so it doesn't constrain the measurement
    auto widthProp = xaml::FrameworkElement::WidthProperty();
    auto heightProp = xaml::FrameworkElement::HeightProperty();
    auto origWidth = element.GetValue(widthProp);
    auto origHeight = element.GetValue(heightProp);
    element.ClearValue(widthProp);
    element.ClearValue(heightProp);

    element.Measure(availableSpace);

    element.SetValue(widthProp, origWidth);
    element.SetValue(heightProp, origHeight);
  } catch (winrt::hresult_error const &) {
    // TODO add more error handling here
    assert(false);
  }

  YGSize desiredSize = {
      GetConstrainedResult(constrainToWidth, element.DesiredSize().Width, widthMode),
      GetConstrainedResult(constrainToHeight, element.DesiredSize().Height, heightMode)};
  return desiredSize;
}

ViewManagerBase::ViewManagerBase(const Mso::React::IReactContext &context)
    : m_context(&context),
      m_batchingEventEmitter{std::make_shared<React::BatchingEventEmitter>(Mso::CntPtr(&context))} {}

void ViewManagerBase::GetExportedViewConstants(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {}

void ViewManagerBase::GetCommands(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {}

void ViewManagerBase::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  React::WriteProperty(writer, L"onLayout", L"function");
  React::WriteProperty(writer, L"keyDownEvents", L"array");
  React::WriteProperty(writer, L"keyUpEvents", L"array");
}

void ViewManagerBase::GetConstants(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  writer.WritePropertyName(L"Constants");
  writer.WriteObjectBegin();
  GetExportedViewConstants(writer);
  writer.WriteObjectEnd();

  writer.WritePropertyName(L"Commands");
  writer.WriteObjectBegin();
  GetCommands(writer);
  writer.WriteObjectEnd();

  writer.WritePropertyName(L"NativeProps");
  writer.WriteObjectBegin();
  GetNativeProps(writer);
  writer.WriteObjectEnd();

  writer.WritePropertyName(L"bubblingEventTypes");
  writer.WriteObjectBegin();
  GetExportedCustomBubblingEventTypeConstants(writer);
  writer.WriteObjectEnd();

  writer.WritePropertyName(L"directEventTypes");
  writer.WriteObjectBegin();
  GetExportedCustomDirectEventTypeConstants(writer);
  writer.WriteObjectEnd();
}

ShadowNode *ViewManagerBase::createShadow() const {
  // This class is the default ShadowNode that most view managers can use. If
  // they need special functionality
  //  they should override this function and create their own ShadowNodeBase
  //  sub-class.
  return new ShadowNodeBase();
}

void ViewManagerBase::destroyShadow(ShadowNode *node) const {
  delete node;
}

void ViewManagerBase::GetExportedCustomBubblingEventTypeConstants(
    const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  const PCWSTR standardEventBaseNames[] = {
      // Generic events
      L"Press",
      L"Change",
      L"Focus",
      L"Blur",
      L"SubmitEditing",
      L"EndEditing",
      L"KeyPress",

      // Touch events
      L"TouchStart",
      L"TouchMove",
      L"TouchCancel",
      L"TouchEnd",

      // Keyboard events
      L"KeyUp",
      L"KeyDown",
  };

  folly::dynamic bubblingEvents = folly::dynamic::object();
  for (auto &standardEventBaseName : standardEventBaseNames) {
    using namespace std::string_literals;
    std::wstring eventName = L"top"s + standardEventBaseName;
    std::wstring bubbleName = L"on"s + standardEventBaseName;

    writer.WritePropertyName(eventName);
    writer.WriteObjectBegin();
    writer.WritePropertyName(L"phasedRegistrationNames");
    writer.WriteObjectBegin();
    React::WriteProperty(writer, L"captured", bubbleName + L"Capture");
    React::WriteProperty(writer, L"bubbled", std::move(bubbleName));
    writer.WriteObjectEnd();
    writer.WriteObjectEnd();
  }
}

void ViewManagerBase::GetExportedCustomDirectEventTypeConstants(
    const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  constexpr PCWSTR eventNames[] = {// Generic events
                                   L"Layout",
                                   L"MouseEnter",
                                   L"MouseLeave",
                                   L"AccessibilityAction"};

  for (auto &eventBaseName : eventNames) {
    using namespace std::string_literals;
    std::wstring eventName = L"top"s + eventBaseName;
    std::wstring bubbleName = L"on"s + eventBaseName;

    writer.WritePropertyName(L"top"s + eventBaseName);
    writer.WriteObjectBegin();
    React::WriteProperty(writer, L"registrationName", L"on"s + eventBaseName);
    writer.WriteObjectEnd();
  }
}

XamlView ViewManagerBase::CreateView(int64_t tag, const winrt::Microsoft::ReactNative::JSValueObject &props) {
  XamlView view = CreateViewCore(tag, props);

  OnViewCreated(view);
  // Set the tag if the element type supports it
  SetTag(view, tag);

  // In Debug, set the element name to the tag for convienent
  // searching within VisualStudio's Live Visual Tree pane
#ifdef DEBUG
  auto element = view.try_as<xaml::FrameworkElement>();
  if (element) {
    element.Name(L"<reacttag>: " + std::to_wstring(tag));
  }
#endif

  return view;
}

void ViewManagerBase::AddView(const XamlView & /*parent*/, const XamlView & /*child*/, int64_t /*index*/) {
  // ASSERT: Child must either implement or not allow children.
  assert(false);
}

void ViewManagerBase::RemoveChildAt(const XamlView & /*parent*/, int64_t /*index*/) {
  // ASSERT: Child must either implement or not allow children.
  assert(false);
}

void ViewManagerBase::RemoveAllChildren(const XamlView &parent) {}

void ViewManagerBase::ReplaceChild(const XamlView &parent, const XamlView &oldChild, const XamlView &newChild) {
  // ASSERT: Child must either implement or not allow children.
  assert(false);
}

void ViewManagerBase::UpdateProperties(
    ShadowNodeBase *nodeToUpdate,
    winrt::Microsoft::ReactNative::JSValueObject &props) {
  // Directly dirty this node since non-layout changes like the text property do
  // not trigger relayout
  //  There isn't actually a yoga node for RawText views, but it will invalidate
  //  the ancestors which
  //  will include the containing Text element. And that's what matters.
  int64_t tag = GetTag(nodeToUpdate->GetView());
  if (auto uiManager = GetNativeUIManager(GetReactContext()).lock())
    uiManager->DirtyYogaNode(tag);

  for (const auto &pair : props) {
    const std::string &propertyName = pair.first;
    const auto &propertyValue = pair.second;
    if (!UpdateProperty(nodeToUpdate, propertyName, propertyValue)) {
      NotifyUnimplementedProperty(nodeToUpdate, propertyName, propertyValue);
    }
  }

  OnPropertiesUpdated(nodeToUpdate);
}

bool ViewManagerBase::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  if (propertyName == "onLayout") {
    nodeToUpdate->m_onLayoutRegistered = !propertyValue.IsNull() && propertyValue.AsBoolean();
  } else if (propertyName == "keyDownEvents") {
    nodeToUpdate->UpdateHandledKeyboardEvents(propertyName, propertyValue);
  } else if (propertyName == "keyUpEvents") {
    nodeToUpdate->UpdateHandledKeyboardEvents(propertyName, propertyValue);
  } else {
    return false;
  }
  return true;
}

void ViewManagerBase::TransferProperties(const XamlView & /*oldView*/, const XamlView & /*newView*/) {}

void ViewManagerBase::DispatchCommand(
    const XamlView & /*viewToUpdate*/,
    const std::string & /*commandId*/,
    winrt::Microsoft::ReactNative::JSValueArray &&commandArgs) {
  assert(false); // View did not handle its command
}

static const winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<std::shared_ptr<ExpressionAnimationStore>>>
    &ExpressionAnimationStorePropertyId() noexcept {
  static const winrt::Microsoft::ReactNative::ReactPropertyId<
      winrt::Microsoft::ReactNative::ReactNonAbiValue<std::shared_ptr<ExpressionAnimationStore>>>
      prop{L"ReactNative.ViewManagerBase", L"ExpressionAnimationStore"};
  return prop;
}

std::shared_ptr<ExpressionAnimationStore> ViewManagerBase::GetExpressionAnimationStore() noexcept {
  return winrt::Microsoft::ReactNative::ReactPropertyBag(GetReactContext().Properties())
      .GetOrCreate(ExpressionAnimationStorePropertyId(), []() { return std::make_shared<ExpressionAnimationStore>(); })
      .Value();
}

void ViewManagerBase::NotifyUnimplementedProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &value) {
#ifdef DEBUG
  auto viewManagerName = nodeToUpdate->GetViewManager()->GetName();
  auto element(nodeToUpdate->GetView().as<winrt::IInspectable>());

  if (element != nullptr) {
    auto className = Microsoft::Common::Unicode::Utf16ToUtf8(winrt::get_class_name(element));
    TestHook::NotifyUnimplementedProperty(
        Microsoft::Common::Unicode::Utf16ToUtf8(viewManagerName), className, propertyName, value);
  } else {
    cdebug << "[NonIInspectable] viewManagerName = " << viewManagerName << "\n";
  }
#endif // DEBUG
}

void ViewManagerBase::SetLayoutProps(
    ShadowNodeBase &nodeToUpdate,
    const XamlView &viewToUpdate,
    float left,
    float top,
    float width,
    float height) {
  if (auto uiManager = GetNativeUIManager(GetReactContext()).lock()) {
    auto parent = nodeToUpdate.GetParent();
    if (parent != -1) {
      const auto &parentShadowNode = uiManager->getHost()->GetShadowNodeForTag(parent);
      const auto &parentVM = parentShadowNode.m_viewManager;
      if (parentVM->RequiresNativeLayout()) {
        return;
      }
    }
  }

  auto element = viewToUpdate.as<xaml::UIElement>();
  if (element == nullptr) {
    // TODO: Assert
    return;
  }
  auto fe = element.as<xaml::FrameworkElement>();

  const bool layoutHasChanged = left != ViewPanel::GetLeft(element) || top != ViewPanel::GetTop(element) ||
      width != fe.Width() || height != fe.Height();

  // Set Position & Size Properties
  ViewPanel::SetLeft(element, left);
  ViewPanel::SetTop(element, top);

  fe.Width(width);
  fe.Height(height);

  // Fire Events
  if (layoutHasChanged && nodeToUpdate.m_onLayoutRegistered) {
    int64_t tag = GetTag(viewToUpdate);
    React::JSValueObject layout{{"x", left}, {"y", top}, {"height", height}, {"width", width}};

    React::JSValueObject eventData{{"target", tag}, {"layout", std::move(layout)}};

    m_batchingEventEmitter->DispatchCoalescingEvent(tag, L"topLayout", MakeJSValueWriter(std::move(eventData)));
  }
}

YGMeasureFunc ViewManagerBase::GetYogaCustomMeasureFunc() const {
  return nullptr;
}

bool ViewManagerBase::RequiresYogaNode() const {
  return true;
}

bool ViewManagerBase::IsNativeControlWithSelfLayout() const {
  return GetYogaCustomMeasureFunc() != nullptr;
}

void ViewManagerBase::DispatchEvent(
    int64_t viewTag,
    winrt::hstring &&eventName,
    const React::JSValueArgWriter &eventDataWriter) const noexcept {
  m_batchingEventEmitter->DispatchEvent(viewTag, std::move(eventName), eventDataWriter);
}

} // namespace Microsoft::ReactNative
