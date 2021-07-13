// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "RefreshControlManager.h"

#include <JSValueWriter.h>
#include <UI.Xaml.Controls.h>
#include <Utils/Helpers.h>
#include <Views/ShadowNodeBase.h>

namespace winrt {
using namespace xaml;
using namespace xaml::Controls;
using namespace Windows::Foundation;
} // namespace winrt

namespace Microsoft::ReactNative {

class RefreshControlShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  RefreshControlShadowNode(){};
  void createView(const winrt::Microsoft::ReactNative::JSValueObject &) override;
  void updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) override;

 private:
  winrt::RefreshContainer::RefreshRequested_revoker m_refreshRequestedRevoker{};
  winrt::Deferral m_refreshDeferral{nullptr};
};

void RefreshControlShadowNode::createView(const winrt::Microsoft::ReactNative::JSValueObject &props) {
  Super::createView(props);
  if (auto refreshContainer = GetView().try_as<winrt::RefreshContainer>()) {
    m_refreshRequestedRevoker =
        refreshContainer.RefreshRequested(winrt::auto_revoke, [this](auto &&, winrt::RefreshRequestedEventArgs args) {
          m_refreshDeferral = args.GetDeferral();
          winrt::Microsoft::ReactNative::JSValueObject eventData{};
          GetViewManager()->DispatchEvent(m_tag, L"topOnRefresh", MakeJSValueWriter(std::move(eventData)));
        });
  }
}

void RefreshControlShadowNode::updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) {
  if (auto refreshContainer = GetView().try_as<winrt::RefreshContainer>()) {
    for (const auto &pair : props) {
      const std::string &propertyName = pair.first;
      if (propertyName == "flexDirection") {
        if (pair.second.Type() == winrt::Microsoft::ReactNative::JSValueType::String &&
            pair.second.AsString() == "column") { // vertical scrollView
          refreshContainer.PullDirection(winrt::RefreshPullDirection::TopToBottom);
        } else {
          refreshContainer.PullDirection(winrt::RefreshPullDirection::LeftToRight);
        }
      } else if (propertyName == "refreshing") {
        if (pair.second.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean) {
          bool refreshing = pair.second.AsBoolean();
          if (!refreshing && m_refreshDeferral) {
            m_refreshDeferral.Complete();
            m_refreshDeferral = nullptr;
          }
        }
      }
    }
  }

  Super::updateProperties(props);
}

RefreshControlViewManager::RefreshControlViewManager(const Mso::React::IReactContext &context) : Super(context) {}

ShadowNode *RefreshControlViewManager::createShadow() const {
  return new RefreshControlShadowNode();
}

const wchar_t *RefreshControlViewManager::GetName() const {
  return L"RCTRefreshControl";
}

XamlView RefreshControlViewManager::CreateViewCore(
    int64_t /*tag*/,
    const winrt::Microsoft::ReactNative::JSValueObject &) {
  if (IsRS4OrHigher()) {
    // refreshContainer is supported >= RS4
    return winrt::RefreshContainer();
  } else {
    // just return a grid if refreshContainer is not supported
    return winrt::Grid();
  }
}

void RefreshControlViewManager::AddView(const XamlView &parent, const XamlView &child, int64_t /*index*/) {
  if (auto refreshContainer = parent.try_as<winrt::RefreshContainer>()) {
    refreshContainer.Content(child.as<winrt::ScrollViewer>());
  } else if (auto grid = parent.try_as<winrt::Grid>()) {
    grid.Children().Append(child.as<winrt::ScrollViewer>());
  }
}

void RefreshControlViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetNativeProps(writer);
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"refreshing", L"boolean");
}

void RefreshControlViewManager::GetExportedCustomDirectEventTypeConstants(
    const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetExportedCustomDirectEventTypeConstants(writer);

  writer.WritePropertyName(L"topOnRefresh");
  writer.WriteObjectBegin();
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"registrationName", L"onRefresh");
  writer.WriteObjectEnd();
}
} // namespace Microsoft::ReactNative
