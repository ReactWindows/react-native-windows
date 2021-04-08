// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <pch.h>

#include <Views/ShadowNodeBase.h>
#include "TouchEventHandler.h"

#ifdef USE_FABRIC
#include <Fabric/FabricUIManagerModule.h>
#include <Fabric/ViewComponentView.h>
#include <react/renderer/components/view/TouchEventEmitter.h>
#endif

#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Documents.h>
#include <UI.Xaml.Input.h>
#include <UI.Xaml.Media.h>
#include <Utils/ValueUtils.h>

#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>

#ifdef USE_WINUI3
#include <winrt/Microsoft.UI.Input.Experimental.h>
#endif

namespace Microsoft::ReactNative {

std::vector<int64_t> GetTagsForBranch(INativeUIManagerHost *host, int64_t tag);

TouchEventHandler::TouchEventHandler(const Mso::React::IReactContext &context)
    : m_xamlView(nullptr), m_context(&context) {}

TouchEventHandler::~TouchEventHandler() {
  RemoveTouchHandlers();
}

void TouchEventHandler::AddTouchHandlers(XamlView xamlView) {
  auto uiElement(xamlView.as<xaml::UIElement>());
  if (uiElement == nullptr) {
    assert(false);
    return;
  }

  m_xamlView = xamlView;

  RemoveTouchHandlers();

  m_pressedRevoker = uiElement.PointerPressed(winrt::auto_revoke, {this, &TouchEventHandler::OnPointerPressed});
  m_releasedRevoker = uiElement.PointerReleased(winrt::auto_revoke, {this, &TouchEventHandler::OnPointerReleased});
  m_canceledRevoker = uiElement.PointerCanceled(winrt::auto_revoke, {this, &TouchEventHandler::OnPointerCanceled});
  m_captureLostRevoker =
      uiElement.PointerCaptureLost(winrt::auto_revoke, {this, &TouchEventHandler::OnPointerCaptureLost});
  m_exitedRevoker = uiElement.PointerExited(winrt::auto_revoke, {this, &TouchEventHandler::OnPointerExited});
  m_movedRevoker = uiElement.PointerMoved(winrt::auto_revoke, {this, &TouchEventHandler::OnPointerMoved});
}

void TouchEventHandler::RemoveTouchHandlers() {
  m_pressedRevoker.revoke();
  m_releasedRevoker.revoke();
  m_canceledRevoker.revoke();
  m_captureLostRevoker.revoke();
  m_exitedRevoker.revoke();
  m_movedRevoker.revoke();
}

void TouchEventHandler::OnPointerPressed(
    const winrt::IInspectable & /*sender*/,
    const winrt::PointerRoutedEventArgs &args) {
  // Short circuit all of this if we are in an error state
  if (m_context->State() == Mso::React::ReactInstanceState::HasError)
    return;

  if (IndexOfPointerWithId(args.Pointer().PointerId()) != std::nullopt) {
    // A pointer with this ID already exists
    assert(false);
    return;
  }

  // Only if the view has a Tag can we process this
  int64_t tag;
  xaml::UIElement sourceElement(nullptr);
  if (!TagFromOriginalSource(args, &tag, &sourceElement))
    return;

  // If this was caused by the user pressing the "back" hardware button, fire that event instead
  if (args.GetCurrentPoint(sourceElement).Properties().PointerUpdateKind() ==
      winrt::Windows::UI::Input::PointerUpdateKind::XButton1Pressed) {
    args.Handled(DispatchBackEvent());
    return;
  }

  if (m_xamlView.as<xaml::FrameworkElement>().CapturePointer(args.Pointer())) {
    // Pointer pressing updates the enter/leave state
    UpdatePointersInViews(args, tag, sourceElement);

    size_t pointerIndex = AddReactPointer(args, tag, sourceElement);

    // For now, when using the mouse we only want to send click events for the left button.
    // Finger and pen taps will also set isLeftButton.
    if (m_pointers[pointerIndex].isLeftButton) {
      DispatchTouchEvent(TouchEventType::Start, pointerIndex);
    }
  }
}

void TouchEventHandler::OnPointerReleased(
    const winrt::IInspectable & /*sender*/,
    const winrt::PointerRoutedEventArgs &args) {
  OnPointerConcluded(TouchEventType::End, args);
}

void TouchEventHandler::OnPointerCanceled(
    const winrt::IInspectable & /*sender*/,
    const winrt::PointerRoutedEventArgs &args) {
  OnPointerConcluded(TouchEventType::Cancel, args);
}

void TouchEventHandler::OnPointerCaptureLost(
    const winrt::IInspectable & /*sender*/,
    const winrt::PointerRoutedEventArgs &args) {
  OnPointerConcluded(TouchEventType::Cancel, args);
}

void TouchEventHandler::OnPointerExited(
    const winrt::IInspectable & /*sender*/,
    const winrt::PointerRoutedEventArgs &args) {
  // Short circuit all of this if we are in an error state
  if (m_context->State() == Mso::React::ReactInstanceState::HasError)
    return;

  UpdatePointersInViews(args, -1, nullptr);
}

void TouchEventHandler::OnPointerMoved(
    const winrt::IInspectable & /*sender*/,
    const winrt::PointerRoutedEventArgs &args) {
  // Short circuit all of this if we are in an error state
  if (m_context->State() == Mso::React::ReactInstanceState::HasError)
    return;

  // Only if the view has a Tag can we process this
  int64_t tag;
  xaml::UIElement sourceElement(nullptr);
  if (!TagFromOriginalSource(args, &tag, &sourceElement))
    return;

  auto optPointerIndex = IndexOfPointerWithId(args.Pointer().PointerId());
  if (optPointerIndex) {
    UpdateReactPointer(m_pointers[*optPointerIndex], args, sourceElement);
    DispatchTouchEvent(TouchEventType::Move, *optPointerIndex);
  } else {
    // Move with no buttons pressed
    UpdatePointersInViews(args, tag, sourceElement);
  }
}

void TouchEventHandler::OnPointerConcluded(TouchEventType eventType, const winrt::PointerRoutedEventArgs &args) {
  // Short circuit all of this if we are in an error state
  if (m_context->State() == Mso::React::ReactInstanceState::HasError)
    return;

  auto optPointerIndex = IndexOfPointerWithId(args.Pointer().PointerId());
  if (!optPointerIndex)
    return;

  // if the view has a Tag, update the pointer info.
  // Regardless of that, ensure we Dispatch & cleanup the pointer
  int64_t tag;
  xaml::UIElement sourceElement(nullptr);
  if (TagFromOriginalSource(args, &tag, &sourceElement))
    UpdateReactPointer(m_pointers[*optPointerIndex], args, sourceElement);

  if (m_pointers[*optPointerIndex].isLeftButton) {
    DispatchTouchEvent(eventType, *optPointerIndex);
  }

  m_pointers.erase(cbegin(m_pointers) + *optPointerIndex);
  if (m_pointers.size() == 0)
    m_touchId = 0;

  m_xamlView.as<xaml::FrameworkElement>().ReleasePointerCapture(args.Pointer());
}

size_t TouchEventHandler::AddReactPointer(
    const winrt::PointerRoutedEventArgs &args,
    int64_t tag,
    xaml::UIElement sourceElement) {
  ReactPointer pointer = CreateReactPointer(args, tag, sourceElement);
  m_pointers.emplace_back(std::move(pointer));
  return m_pointers.size() - 1;
}

TouchEventHandler::ReactPointer TouchEventHandler::CreateReactPointer(
    const winrt::PointerRoutedEventArgs &args,
    int64_t tag,
    xaml::UIElement sourceElement) {
  auto point = args.GetCurrentPoint(sourceElement);
  auto props = point.Properties();

  ReactPointer pointer;
  pointer.target = tag;
  pointer.identifier = m_touchId++;
  pointer.pointerId = point.PointerId();
#ifndef USE_WINUI3
  pointer.deviceType = point.PointerDevice().PointerDeviceType();
#else
  pointer.deviceType = point.PointerDeviceType();
#endif
  pointer.isLeftButton = props.IsLeftButtonPressed();
  pointer.isRightButton = props.IsRightButtonPressed();
  pointer.isMiddleButton = props.IsMiddleButtonPressed();
  pointer.isHorizontalScrollWheel = props.IsHorizontalMouseWheel();
  pointer.isEraser = props.IsEraser();

  UpdateReactPointer(pointer, args, sourceElement);

  return pointer;
}

void TouchEventHandler::UpdateReactPointer(
    ReactPointer &pointer,
    const winrt::PointerRoutedEventArgs &args,
    xaml::UIElement sourceElement) {
  auto rootPoint = args.GetCurrentPoint(m_xamlView.as<xaml::FrameworkElement>());
  auto point = args.GetCurrentPoint(sourceElement);
  auto props = point.Properties();
  auto keyModifiers = static_cast<uint32_t>(args.KeyModifiers());

  pointer.positionRoot = rootPoint.Position();
  pointer.positionView = point.Position();
  pointer.timestamp = point.Timestamp() / 1000; // us -> ms
  pointer.pressure = props.Pressure();
  pointer.isBarrelButton = props.IsBarrelButtonPressed();
  pointer.shiftKey = 0 != (keyModifiers & static_cast<uint32_t>(winrt::Windows::System::VirtualKeyModifiers::Shift));
  pointer.ctrlKey = 0 != (keyModifiers & static_cast<uint32_t>(winrt::Windows::System::VirtualKeyModifiers::Control));
  pointer.altKey = 0 != (keyModifiers & static_cast<uint32_t>(winrt::Windows::System::VirtualKeyModifiers::Menu));
}

std::optional<size_t> TouchEventHandler::IndexOfPointerWithId(uint32_t pointerId) {
  for (size_t i = 0; i < m_pointers.size(); ++i) {
    if (m_pointers[i].pointerId == pointerId)
      return i;
  }

  return std::nullopt;
}

void TouchEventHandler::UpdatePointersInViews(
    const winrt::PointerRoutedEventArgs &args,
    int64_t tag,
    xaml::UIElement sourceElement) {
  if (auto nativeUiManager = GetNativeUIManager(*m_context).lock()) {
    auto puiManagerHost = nativeUiManager->getHost();
    int32_t pointerId = args.Pointer().PointerId();

    // m_pointers is tracking the pointers that are 'down', for moves we usually
    // don't have any pointers down and should reset the touchId back to zero
    if (m_pointers.size() == 0)
      m_touchId = 0;

    // Get the branch of views under the pointer in leaf to root order
    std::vector<int64_t> newViews;
    if (tag != -1)
      newViews = GetTagsForBranch(puiManagerHost, tag);

    // Get the results of the last time we calculated the path
    auto it = m_pointersInViews.find(pointerId);
    TagSet *existingViews;
    if (it != m_pointersInViews.end()) {
      existingViews = &it->second;
    } else {
      existingViews = nullptr;
    }

    // Short-circuit if the hierarchy hasn't changed
    if ((existingViews == nullptr && newViews.size() == 0) ||
        (existingViews != nullptr && existingViews->orderedTags == newViews))
      return;

    // Prep to fire pointer events
    std::unordered_set<int64_t> newViewsSet(newViews.begin(), newViews.end());
    ReactPointer pointer;

    auto optPointerIndex = IndexOfPointerWithId(pointerId);
    if (optPointerIndex) {
      pointer = m_pointers[*optPointerIndex];
      UpdateReactPointer(pointer, args, sourceElement);
    } else {
      pointer = CreateReactPointer(args, tag, sourceElement);
    }

    // Walk through existingViews from innermost to outer, firing mouseLeave events if they are not in newViews
    if (existingViews) {
      for (int64_t existingTag : existingViews->orderedTags) {
        if (newViewsSet.count(existingTag)) {
          continue;
        }

        ShadowNodeBase *node = static_cast<ShadowNodeBase *>(puiManagerHost->FindShadowNodeForTag(existingTag));
        if (node != nullptr && node->m_onMouseLeaveRegistered)
          m_context->DispatchEvent(existingTag, "topMouseLeave", GetPointerJson(pointer, existingTag));
      }
    }

    // Walk through newViews from outermost to inner, firing mouseEnter events if they are not in existingViews
    for (auto iter = newViews.rbegin(); iter != newViews.rend(); ++iter) {
      int64_t newTag = *iter;
      if (existingViews && existingViews->tags.count(newTag)) {
        continue;
      }

      ShadowNodeBase *node = static_cast<ShadowNodeBase *>(puiManagerHost->FindShadowNodeForTag(newTag));
      if (node != nullptr && node->m_onMouseEnterRegistered)
        m_context->DispatchEvent(newTag, "topMouseEnter", GetPointerJson(pointer, newTag));
    }

    m_pointersInViews[pointerId] = {std::move(newViewsSet), std::move(newViews)};
  }
}

folly::dynamic TouchEventHandler::GetPointerJson(const ReactPointer &pointer, int64_t target) {
  folly::dynamic json =
      folly::dynamic::object()("target", target)("identifier", pointer.identifier)("pageX", pointer.positionRoot.X)(
          "pageY", pointer.positionRoot.Y)("locationX", pointer.positionView.X)("locationY", pointer.positionView.Y)(
          "timestamp", pointer.timestamp)("pointerType", GetPointerDeviceTypeName(pointer.deviceType))(
          "force", pointer.pressure)("isLeftButton", pointer.isLeftButton)("isRightButton", pointer.isRightButton)(
          "isMiddleButton", pointer.isMiddleButton)("isBarrelButtonPressed", pointer.isBarrelButton)(
          "isHorizontalScrollWheel", pointer.isHorizontalScrollWheel)("isEraser", pointer.isEraser)(
          "shiftKey", pointer.shiftKey)("ctrlKey", pointer.ctrlKey)("altKey", pointer.altKey);
  return json;
}

#ifdef USE_FABRIC
// This work should probably be delegated to the ComponentViews
facebook::react::SharedEventEmitter EventEmitterForElement(
    std::shared_ptr<FabricUIManager> &uimanager,
    facebook::react::Tag tag) noexcept {
  auto &registry = uimanager->GetViewRegistry();

  auto descriptor = registry.componentViewDescriptorWithTag(tag);
  auto view = std::static_pointer_cast<BaseComponentView const>(descriptor.view);
  auto emitter = view->GetEventEmitter();
  if (emitter)
    return emitter;

  auto element = view->Element();
  while (auto parent = element.Parent()) {
    if (element = parent.try_as<xaml::FrameworkElement>()) {
      auto boxedTag = element.Tag();
      if (boxedTag) {
        if (tag = winrt::unbox_value<facebook::react::Tag>(element.Tag()))
          return EventEmitterForElement(uimanager, tag);
      }
    }
  }
  return nullptr;
}

facebook::react::Touch TouchEventHandler::TouchForPointer(const ReactPointer &pointer) noexcept {
  facebook::react::Touch t;
  t.force = pointer.pressure;
  t.identifier = static_cast<int>(pointer.identifier);
  t.pagePoint.x = pointer.positionRoot.X; // TODO: This should be relative to the rootview, not the XAML tree
  t.pagePoint.y = pointer.positionRoot.Y; // TODO: This should be relative to the rootview, not the XAML tree
  t.screenPoint.x = pointer.positionRoot.X;
  t.screenPoint.y = pointer.positionRoot.Y;
  t.offsetPoint.x = pointer.positionView.X;
  t.offsetPoint.y = pointer.positionView.Y;
  t.target = static_cast<facebook::react::Tag>(pointer.target);
  t.timestamp = static_cast<facebook::react::Float>(pointer.timestamp);
  return t;
}
#endif

void TouchEventHandler::DispatchTouchEvent(TouchEventType eventType, size_t pointerIndex) {
  folly::dynamic changedIndices = folly::dynamic::array();
  changedIndices.push_back(pointerIndex);

#ifdef USE_FABRIC
  if (auto fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
          winrt::Microsoft::ReactNative::ReactPropertyBag(m_context->Properties()))) {
    std::unordered_set<facebook::react::SharedTouchEventEmitter> uniqueEventEmitters = {};
    std::vector<facebook::react::SharedTouchEventEmitter> emittersForIndex;

    facebook::react::TouchEvent te;

    size_t index = 0;
    for (const auto &pointer : m_pointers) {
      te.touches.insert(TouchForPointer(pointer));
      if (pointerIndex == index++)
        te.changedTouches.insert(TouchForPointer(pointer));

      auto emitter = std::static_pointer_cast<facebook::react::TouchEventEmitter>(
          std::const_pointer_cast<facebook::react::EventEmitter>(
              EventEmitterForElement(fabricuiManager, static_cast<facebook::react::Tag>(pointer.target))));
      emittersForIndex.push_back(emitter);
      if (emitter)
        uniqueEventEmitters.insert(emitter);
    }

    for (const auto emitter : uniqueEventEmitters) {
      te.targetTouches.clear();
      index = 0;
      for (const auto &pointer : m_pointers) {
        auto pointerEmitter = emittersForIndex[index++];
        if (emitter == pointerEmitter)
          te.targetTouches.insert(TouchForPointer(pointer));
      }

      switch (eventType) {
        case TouchEventType::Start:
          emitter->onTouchStart(te);
          break;
        case TouchEventType::Move:
          emitter->onTouchMove(te);
          break;
        case TouchEventType::End:
          emitter->onTouchEnd(te);
          break;
        case TouchEventType::Cancel:
          emitter->onTouchCancel(te);
          break;
      }
    }
  } else
#endif // USE_FABRIC
  {
    auto touches = folly::dynamic::array();
    for (const auto &pointer : m_pointers) {
      folly::dynamic touch = GetPointerJson(pointer, pointer.target);
      touches.push_back(touch);
    }

    // Package up parameters and invoke the JS event emitter
    const char *eventName = GetTouchEventTypeName(eventType);
    if (eventName == nullptr)
      return;
    folly::dynamic params = folly::dynamic::array(eventName, std::move(touches), std::move(changedIndices));

    m_context->CallJSFunction("RCTEventEmitter", "receiveTouches", std::move(params));
  }
}

bool TouchEventHandler::DispatchBackEvent() {
  if (m_context->State() != Mso::React::ReactInstanceState::Loaded)
    return false;

  m_context->CallJSFunction("RCTDeviceEventEmitter", "emit", folly::dynamic::array("hardwareBackPress"));
  return true;
}

const char *TouchEventHandler::GetPointerDeviceTypeName(
    winrt::Windows::Devices::Input::PointerDeviceType deviceType) noexcept {
  const char *deviceTypeName = "unknown";
  switch (deviceType) {
    case winrt::Windows::Devices::Input::PointerDeviceType::Mouse:
      deviceTypeName = "mouse";
      break;
    case winrt::Windows::Devices::Input::PointerDeviceType::Pen:
      deviceTypeName = "pen";
      break;
    case winrt::Windows::Devices::Input::PointerDeviceType::Touch:
      deviceTypeName = "touch";
      break;
    default:
      break;
  }
  return deviceTypeName;
}

const char *TouchEventHandler::GetTouchEventTypeName(TouchEventType eventType) noexcept {
  const char *eventName = nullptr;
  switch (eventType) {
    case TouchEventType::Start:
      eventName = "topTouchStart";
      break;
    case TouchEventType::End:
      eventName = "topTouchEnd";
      break;
    case TouchEventType::Move:
      eventName = "topTouchMove";
      break;
    case TouchEventType::Cancel:
      eventName = "topTouchCancel";
      break;
    default:
      assert(false);
      break;
  }
  return eventName;
}

bool TouchEventHandler::TagFromOriginalSource(
    const winrt::PointerRoutedEventArgs &args,
    int64_t *pTag,
    xaml::UIElement *pSourceElement) {
  assert(pTag != nullptr);
  assert(pSourceElement != nullptr);

  // Find the React element that triggered the input event
  xaml::UIElement sourceElement = args.OriginalSource().try_as<xaml::UIElement>();
  winrt::IPropertyValue tag(nullptr);

  while (sourceElement) {
    auto tagValue = sourceElement.ReadLocalValue(xaml::FrameworkElement::TagProperty());
    if (tagValue != xaml::DependencyProperty::UnsetValue()) {
      tag = tagValue.try_as<winrt::IPropertyValue>();
      // If a TextBlock was the UIElement event source, perform a more accurate hit test,
      // searching for the tag of the nested Run/Span XAML elements that the user actually clicked.
      // This is to support nested <Text> elements in React.
      // Nested React <Text> elements get translated into nested XAML <Span> elements,
      // while the content of the <Text> becomes a list of XAML <Run> elements.
      // However, we should report the Text element as the target, not the contexts of the text.
      if (const auto textBlock = sourceElement.try_as<xaml::Controls::TextBlock>()) {
        if (textBlock.Inlines().Size() == 0) {
          // No need to hit test if TextBlock does not use Inlines
          break;
        }

        const auto pointerPos = args.GetCurrentPoint(textBlock).RawPosition();
        const auto textPointer = GetPositionFromPoint(textBlock, pointerPos);
        if (!textPointer) {
          // Did not find a matching character in the hit test
          break;
        }

        const auto childTag = textPointer.Parent().ReadLocalValue(xaml::FrameworkElement::TagProperty());
        if (childTag != xaml::DependencyProperty::UnsetValue()) {
          const auto finerTag = childTag.try_as<winrt::IPropertyValue>();
          if (finerTag) {
            tag = finerTag;
          }
        }
      }

      break;
    }

    sourceElement = winrt::VisualTreeHelper::GetParent(sourceElement).try_as<xaml::UIElement>();
  }

  if (tag == nullptr) {
    // If the root view fails to be fully created, then the Tag property will
    // never be set. This can happen,
    //  for example, when the red box error box is shown.
    return false;
  }

  *pTag = tag.GetInt64();
  *pSourceElement = sourceElement;
  return true;
}

bool IsCharacterBefore(winrt::TextPointer textPointer, winrt::Point point) {
  const auto rect = textPointer.GetCharacterRect(winrt::LogicalDirection::Forward);
  const auto bottom = rect.Y + rect.Height;

  // The character Rect always has Width = 0, so we need to use the X-dimension
  // of the next character on the same line. If the next character is not on
  // the same line, we use the rightmost boundary of the TextBlock.
  //
  // The side-effect is that wrapped text may have additional valid hit box
  // space at the end of a line, but it's better than the alternative of the
  // hit box excluding the last character on a line.
  const auto nextPointer = textPointer.GetPositionAtOffset(1, winrt::LogicalDirection::Forward);
  auto right = textPointer.VisualParent().Width();
  if (nextPointer != nullptr) {
    const auto nextRect = nextPointer.GetCharacterRect(winrt::LogicalDirection::Forward);
    if (rect.Y == nextRect.Y) {
      right = nextRect.X;
    }
  }

  // The character is before the point if the Y-coordinate of the point is
  // below (greater than) the bottom of the character rect, or if the
  // Y-coordinate is below (greater than) the top of the character rect and
  // the X-coordinate is greater than the right side of the character rect:
  // ┌───────────┐
  // │     ┌─────┘
  // └─────┘ (x,y)
  return point.Y > bottom || (point.Y > rect.Y && point.X > right);
}

bool IsCharacterAfter(winrt::TextPointer textPointer, winrt::Point point) {
  const auto rect = textPointer.GetCharacterRect(winrt::LogicalDirection::Forward);
  const auto bottom = rect.Y + rect.Height;

  // The character is after the point if the Y-coordinate of the point is above
  // (less than) the top of the character rect, or if the Y-coordinate is above
  // (less than) the bottom of the character rect and the X-coordinate is less
  // than the left side of the character rect:
  //       
  // (x,y) ┌─────┐
  // ┌─────┘     │
  // └───────────┘
  return point.Y < rect.Y || (point.Y < bottom && point.X < rect.X);
}

winrt::TextPointer TouchEventHandler::GetPositionFromPoint(winrt::TextBlock textBlock, winrt::Point point) {
  // Since characters in a TextBlock are sorted from top-left to bottom-right,
  // we can use binary search to find the character whose bounds contains the
  // pointer point.
  //
  // This algorithm currently makes the following assumptions:
  // 1. Characters on the same line have the same Rect::X value
  // 2. The text is left-to-right
  auto textPointer = textBlock.ContentStart();
  auto L = 0;
  auto R = /* n - 1 */ textBlock.ContentEnd().Offset();
  while (L <= R) {
    const auto m = /* floor */ (L + R) / 2;
    const auto relativeOffset = m - textPointer.Offset();
    textPointer = textPointer.GetPositionAtOffset(relativeOffset, winrt::LogicalDirection::Forward);
    if (IsCharacterBefore(textPointer, point) /* A[m] < T */) {
      L = m + 1;
    } else if (IsCharacterAfter(textPointer, point) /* A[m] > T */) {
      R = m - 1;
    } else {
      return textPointer;
    }
  }

  return nullptr;
}

//
// Retreives the path of nodes from an element to the root.
// The order of the returned list is from child to parent.
//
std::vector<int64_t> GetTagsForBranch(INativeUIManagerHost *host, int64_t tag) {
  std::vector<int64_t> tags;

  auto *shadowNode = host->FindShadowNodeForTag(tag);
  while (shadowNode != nullptr && tag != -1) {
    tags.push_back(tag);

    tag = shadowNode->m_parent;
    shadowNode = host->FindShadowNodeForTag(tag);
  }

  return tags;
}

} // namespace Microsoft::ReactNative
