// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <INativeUIManager.h>
#include <ShadowNodeBase.h>
#include <UI.Xaml.Documents.h>
#include <Utils/TextTransform.h>
#include <Views/FrameworkElementViewManager.h>

namespace Microsoft::ReactNative {

struct VirtualTextShadowNode final : public ShadowNodeBase {
  using Super = ShadowNodeBase;
  TextTransform textTransform{TextTransform::Undefined};

  void AddView(ShadowNode &child, int64_t index) override;
  void onDropViewInstance() override;

  void AddToPressableCount(int pressableCount);
  void SetPressable(bool isPressable);

  static void ApplyTextTransform(ShadowNodeBase &node, TextTransform transform, bool forceUpdate, bool isRoot);

  static xaml::DependencyObject
  HitTest(const ShadowNodeBase &node, const winrt::Point &point, bool hasPressableParent);

  struct HighlightData {
    std::vector<HighlightData> data;
    size_t spanIdx = 0;
    std::optional<winrt::Windows::UI::Color> backgroundColor;
    std::optional<winrt::Windows::UI::Color> foregroundColor;
  };

  HighlightData m_highlightData;
  int m_pressableCount{0};
  bool m_isPressable{false};
};

class VirtualTextViewManager : public ViewManagerBase {
  using Super = ViewManagerBase;

 public:
  VirtualTextViewManager(const Mso::React::IReactContext &context);

  const wchar_t *GetName() const override;
  ShadowNode *createShadow() const override {
    return new VirtualTextShadowNode();
  }

  void AddView(const XamlView &parent, const XamlView &child, int64_t index) override;
  void RemoveAllChildren(const XamlView &parent) override;
  void RemoveChildAt(const XamlView &parent, int64_t index) override;

  bool RequiresYogaNode() const override;

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const winrt::Microsoft::ReactNative::JSValue &propertyValue) override;

  XamlView CreateViewCore(int64_t tag, const winrt::Microsoft::ReactNative::JSValueObject &) override;
};

} // namespace Microsoft::ReactNative
