// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <IReactInstance.h>
#include <JSValue.h>
#include <UI.Xaml.Documents.h>
#include <winrt/Windows.Devices.Input.h>
#include <optional>
#include <set>
#include "Utils/BatchingEventEmitter.h"
#include "XamlView.h"

#ifdef USE_FABRIC
#include <react/renderer/components/view/Touch.h>
#endif

namespace winrt {
using namespace Windows::UI;
using namespace xaml;
using namespace xaml::Controls;
using namespace xaml::Documents;
using namespace xaml::Input;
using namespace Windows::Foundation;
using namespace xaml::Media;
} // namespace winrt

namespace Microsoft::ReactNative {

class TouchEventHandler {
 public:
  TouchEventHandler(const Mso::React::IReactContext &context, bool fabric);
  virtual ~TouchEventHandler();

  void AddTouchHandlers(
      XamlView xamlView,
      std::function<bool()> shouldCancelOnCaptureLost = nullptr,
      bool findRoot = false,
      bool handledEventsToo = false);
  void RemoveTouchHandlers();
  winrt::Microsoft::ReactNative::BatchingEventEmitter &BatchingEmitter() noexcept;

 private:
  void OnPointerPressed(const winrt::IInspectable &, const winrt::PointerRoutedEventArgs &args);
  void OnPointerReleased(const winrt::IInspectable &, const winrt::PointerRoutedEventArgs &args);
  void OnPointerCanceled(const winrt::IInspectable &, const winrt::PointerRoutedEventArgs &args);
  void OnPointerCaptureLost(const winrt::IInspectable &, const winrt::PointerRoutedEventArgs &args);
  void OnPointerExited(const winrt::IInspectable &, const winrt::PointerRoutedEventArgs &args);
  void OnPointerMoved(const winrt::IInspectable &, const winrt::PointerRoutedEventArgs &args);
  winrt::IInspectable m_pressedHandler;
  winrt::IInspectable m_releasedHandler;
  winrt::IInspectable m_canceledHandler;
  winrt::IInspectable m_captureLostHandler;
  winrt::IInspectable m_exitedHandler;
  winrt::IInspectable m_movedHandler;
  std::function<bool()> m_shouldCancelOnCaptureLost;
  bool m_findRoot{false};

  struct ReactPointer {
    int64_t target = 0;
    int64_t identifier = 0;
    uint32_t pointerId = 0;
    uint64_t timestamp = 0;
    winrt::Point positionRoot = {0, 0};
    winrt::Point positionView = {0, 0};
    winrt::Windows::Devices::Input::PointerDeviceType deviceType{
        winrt::Windows::Devices::Input::PointerDeviceType::Mouse};
    float pressure = 0;
    bool isLeftButton = false;
    bool isRightButton = false;
    bool isMiddleButton = false;
    bool isBarrelButton = false;
    bool isHorizontalScrollWheel = false;
    bool isEraser = false;
    bool shiftKey = false;
    bool ctrlKey = false;
    bool altKey = false;
  };
  size_t AddReactPointer(const winrt::PointerRoutedEventArgs &args, int64_t tag, xaml::UIElement sourceElement);
  ReactPointer
  CreateReactPointer(const winrt::PointerRoutedEventArgs &args, int64_t tag, xaml::UIElement sourceElement);
  void
  UpdateReactPointer(ReactPointer &pointer, const winrt::PointerRoutedEventArgs &args, xaml::UIElement sourceElement);
  void UpdatePointersInViews(const winrt::PointerRoutedEventArgs &args, int64_t tag, xaml::UIElement sourceElement);

#ifdef USE_FABRIC
  facebook::react::Touch TouchForPointer(const ReactPointer &pointer) noexcept;
#endif
  enum class TouchEventType { Start = 0, End, Move, Cancel, PointerEntered, PointerExited, PointerMove };
  void OnPointerConcluded(TouchEventType eventType, const winrt::PointerRoutedEventArgs &args);
  void DispatchTouchEvent(TouchEventType eventType, size_t pointerIndex);
  bool DispatchBackEvent();
  const char *GetPointerDeviceTypeName(winrt::Windows::Devices::Input::PointerDeviceType deviceType) noexcept;
  const wchar_t *GetTouchEventTypeName(TouchEventType eventType) noexcept;

  std::optional<size_t> IndexOfPointerWithId(uint32_t pointerId);
  winrt::Microsoft::ReactNative::JSValue GetPointerJson(const ReactPointer &pointer, int64_t target);

  struct TagSet {
    std::unordered_set<int64_t> tags;
    std::vector<int64_t> orderedTags;
  };

  std::vector<ReactPointer> m_pointers;
  std::unordered_map<uint32_t /*pointerId*/, TagSet /*tags*/> m_pointersInViews;
  int64_t m_touchId = 0;

  bool TagFromOriginalSource(const winrt::PointerRoutedEventArgs &args, int64_t *pTag, xaml::UIElement *pSourceElement);
  winrt::IPropertyValue TestHit(
      const winrt::Collections::IVectorView<xaml::Documents::Inline> &inlines,
      const winrt::Point &pointerPos,
      bool &isHit);

  XamlView m_xamlView;
  XamlView m_rootView;
  Mso::CntPtr<const Mso::React::IReactContext> m_context;
  bool m_fabric;
  std::shared_ptr<winrt::Microsoft::ReactNative::BatchingEventEmitter> m_batchingEventEmitter;
};

} // namespace Microsoft::ReactNative
