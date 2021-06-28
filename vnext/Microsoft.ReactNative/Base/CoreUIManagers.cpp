// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <IReactInstance.h>
#include <Modules/NativeUIManager.h>
#include <ViewManagerProvider.h>

// Standard View Managers
#include <GlyphViewManager.h>
#include <Views/ActivityIndicatorViewManager.h>
#include <Views/FlyoutViewManager.h>
#include <Views/Image/ImageViewManager.h>
#include <Views/PickerViewManager.h>
#include <Views/PopupViewManager.h>
#include <Views/RawTextViewManager.h>
#include <Views/RefreshControlManager.h>
#include <Views/RootViewManager.h>
#include <Views/ScrollContentViewManager.h>
#include <Views/ScrollViewManager.h>
#include <Views/SliderViewManager.h>
#include <Views/SwitchViewManager.h>
#include <Views/TextInputViewManager.h>
#include <Views/TextViewManager.h>
#include <Views/ViewViewManager.h>
#include <Views/VirtualTextViewManager.h>

namespace Microsoft::ReactNative {

void AddStandardViewManagers(
    std::vector<std::unique_ptr<Microsoft::ReactNative::IViewManager>> &viewManagers,
    const Mso::React::IReactContext &context) noexcept {
  viewManagers.push_back(std::make_unique<ActivityIndicatorViewManager>(context));
  viewManagers.push_back(std::make_unique<FlyoutViewManager>(context));
  viewManagers.push_back(std::make_unique<ImageViewManager>(context));
  viewManagers.push_back(std::make_unique<PickerViewManager>(context));
  viewManagers.push_back(std::make_unique<PopupViewManager>(context));
  viewManagers.push_back(std::make_unique<RawTextViewManager>(context));
  viewManagers.push_back(std::make_unique<RootViewManager>(context));
  viewManagers.push_back(std::make_unique<ScrollContentViewManager>(context));
  viewManagers.push_back(std::make_unique<SliderViewManager>(context));
  viewManagers.push_back(std::make_unique<ScrollViewManager>(context));
  viewManagers.push_back(std::make_unique<SwitchViewManager>(context));
  viewManagers.push_back(std::make_unique<TextViewManager>(context));
  viewManagers.push_back(std::make_unique<TextInputViewManager>(context));
  viewManagers.push_back(std::make_unique<ViewViewManager>(context));
  viewManagers.push_back(std::make_unique<VirtualTextViewManager>(context));
  viewManagers.push_back(std::make_unique<RefreshControlViewManager>(context));
  viewManagers.push_back(std::make_unique<GlyphViewManager>(context));
}

} // namespace Microsoft::ReactNative
