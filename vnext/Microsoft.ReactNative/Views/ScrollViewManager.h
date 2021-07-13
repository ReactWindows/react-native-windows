// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Utils/BatchingEventEmitter.h>
#include <Views/ControlViewManager.h>
#include "Impl/ScrollViewUWPImplementation.h"

namespace Microsoft::ReactNative {

class ScrollViewManager : public ControlViewManager {
  using Super = ControlViewManager;

 public:
  ScrollViewManager(const Mso::React::IReactContext &context);

  const wchar_t *GetName() const override;
  void GetCommands(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;
  void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;
  void GetExportedCustomDirectEventTypeConstants(
      const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;

  ShadowNode *createShadow() const override;

  void AddView(const XamlView &parent, const XamlView &child, int64_t index) override;
  void RemoveAllChildren(const XamlView &parent) override;
  void RemoveChildAt(const XamlView &parent, int64_t index) override;

  void SnapToInterval(const XamlView &parent, float interval);
  void SnapToOffsets(const XamlView &parent, const winrt::IVectorView<float> &offsets);

  winrt::Microsoft::ReactNative::BatchingEventEmitter &BatchingEmitter() noexcept;

 protected:
  XamlView CreateViewCore(int64_t tag, const winrt::Microsoft::ReactNative::JSValueObject &) override;

 private:
  friend class ScrollViewShadowNode;
};

} // namespace Microsoft::ReactNative
