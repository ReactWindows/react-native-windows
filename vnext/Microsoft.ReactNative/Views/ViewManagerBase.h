// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <React.h>
#include <Shared/ReactWindowsAPI.h>
#include <Views/ViewManager.h>
#include <XamlView.h>
#include <folly/dynamic.h>
#include <yoga/yoga.h>
#include "Utils/BatchingEventEmitter.h"

namespace Microsoft::ReactNative {

class ExpressionAnimationStore;
struct IReactInstance;
struct ShadowNodeBase;
struct ShadowNode;

struct YogaContext {
  YogaContext(const XamlView &view_) : view(view_) {}

  XamlView view;
};

REACTWINDOWS_EXPORT YGSize DefaultYogaSelfMeasureFunc(
    YGNodeRef node,
    float width,
    YGMeasureMode widthMode,
    float height,
    YGMeasureMode heightMode);

#pragma warning(push)
#pragma warning(disable : 4275) // base is not DLL exported
#pragma warning(disable : 4251) // member is not DLL exported
class REACTWINDOWS_EXPORT ViewManagerBase : public IViewManager {
 public:
  ViewManagerBase(const Mso::React::IReactContext &context);
  virtual ~ViewManagerBase() {}

  virtual XamlView CreateView(int64_t tag, const winrt::Microsoft::ReactNative::JSValueObject &props);

  void GetExportedViewConstants(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;
  void GetCommands(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;

  void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;

  ShadowNode *createShadow() const override;
  void destroyShadow(ShadowNode *node) const override;

  void GetConstants(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;
  void GetExportedCustomBubblingEventTypeConstants(
      const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;
  void GetExportedCustomDirectEventTypeConstants(
      const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;

  virtual void AddView(const XamlView &parent, const XamlView &child, int64_t index);
  virtual void RemoveAllChildren(const XamlView &parent);
  virtual void RemoveChildAt(const XamlView &parent, int64_t index);
  virtual void ReplaceChild(const XamlView &parent, const XamlView &oldChild, const XamlView &newChild);

  virtual void UpdateProperties(ShadowNodeBase *nodeToUpdate, winrt::Microsoft::ReactNative::JSValueObject &props);

  virtual void DispatchCommand(
      const XamlView &viewToUpdate,
      const std::string &commandId,
      winrt::Microsoft::ReactNative::JSValueArray &&commandArgs);

  // Yoga Layout
  virtual void SetLayoutProps(
      ShadowNodeBase &nodeToUpdate,
      const XamlView &viewToUpdate,
      float left,
      float top,
      float width,
      float height);
  virtual YGMeasureFunc GetYogaCustomMeasureFunc() const;
  virtual bool RequiresYogaNode() const;
  bool IsNativeControlWithSelfLayout() const;

  const Mso::React::IReactContext &GetReactContext() const {
    return *m_context;
  }
  std::shared_ptr<ExpressionAnimationStore> GetExpressionAnimationStore() noexcept;
  void DispatchEvent(
      int64_t viewTag,
      winrt::hstring &&eventName,
      const winrt::Microsoft::ReactNative::JSValueArgWriter &eventDataWriter) const noexcept;

  virtual void TransferProperties(const XamlView &oldView, const XamlView &newView);

 protected:
  virtual XamlView CreateViewCore(int64_t tag, const winrt::Microsoft::ReactNative::JSValueObject &props) = 0;
  virtual void OnViewCreated(XamlView view) {}
  virtual bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const winrt::Microsoft::ReactNative::JSValue &propertyValue);
  virtual void NotifyUnimplementedProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const winrt::Microsoft::ReactNative::JSValue &value);
  virtual void OnPropertiesUpdated(ShadowNodeBase *node) {}

 protected:
  Mso::CntPtr<const Mso::React::IReactContext> m_context;
  std::shared_ptr<winrt::Microsoft::ReactNative::BatchingEventEmitter> m_batchingEventEmitter;
};
#pragma warning(pop)

} // namespace Microsoft::ReactNative
