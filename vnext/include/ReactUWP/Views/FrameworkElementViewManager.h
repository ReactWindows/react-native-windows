// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.UI.Composition.h>
#include "ViewManagerBase.h"

namespace react {
namespace uwp {

class REACTWINDOWS_EXPORT FrameworkElementViewManager : public ViewManagerBase {
  using Super = ViewManagerBase;

 public:
  FrameworkElementViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  folly::dynamic GetNativeProps() const override;

  // Helper functions related to setting/updating TransformMatrix
  void RefreshTransformMatrix(ShadowNodeBase *shadowNode);
  void StartTransformAnimation(
      xaml::UIElement uielement,
      comp::CompositionPropertySet transformPS);

  virtual void TransferProperties(const XamlView &oldView, const XamlView &newView) override;

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const folly::dynamic &propertyValue) override;

  void
  TransferProperty(const XamlView &oldView, const XamlView &newView, xaml::DependencyProperty dp);

  void TransferProperty(
      const XamlView &oldView,
      const XamlView &newView,
      winrt::DependencyProperty oldViewDP,
      winrt::DependencyProperty newViewDP);

 private:
  void ApplyTransformMatrix(
      xaml::UIElement uielement,
      ShadowNodeBase *shadowNode,
      winrt::Windows::Foundation::Numerics::float4x4 transformMatrix);
};

} // namespace uwp
} // namespace react
