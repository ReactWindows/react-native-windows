// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactNativeHost.h"
#include "ReactNativeHost.g.cpp"

#include "ReactPackageBuilder.h"
#include "RedBox.h"
#include "TurboModulesProvider.h"

#include <future/futureWinRT.h>
#include <winrt/Windows.Foundation.Collections.h>
#include "IReactContext.h"
#include "ReactInstanceSettings.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

#ifndef CORE_ABI
using namespace xaml;
using namespace xaml::Controls;
#endif

namespace winrt::Microsoft::ReactNative::implementation {

ReactNativeHost::ReactNativeHost() noexcept : m_reactHost{Mso::React::MakeReactHost()} {
#if _DEBUG
  facebook::react::InitializeLogging([](facebook::react::RCTLogLevel /*logLevel*/, const char *message) {
    std::string str = std::string("ReactNative:") + message;
    OutputDebugStringA(str.c_str());
  });
#endif
}

/*static*/ ReactNative::ReactNativeHost ReactNativeHost::FromContext(
    ReactNative::IReactContext const &reactContext) noexcept {
  return GetReactNativeHost(ReactPropertyBag{reactContext.Properties()});
}

IVector<IReactPackageProvider> ReactNativeHost::PackageProviders() noexcept {
  return InstanceSettings().PackageProviders();
}

ReactNative::ReactInstanceSettings ReactNativeHost::InstanceSettings() noexcept {
  if (!m_instanceSettings) {
    m_instanceSettings = make<ReactInstanceSettings>();
  }

  return m_instanceSettings;
}

void ReactNativeHost::InstanceSettings(ReactNative::ReactInstanceSettings const &value) noexcept {
  m_instanceSettings = value;
}

IAsyncAction ReactNativeHost::LoadInstance() noexcept {
  return ReloadInstance();
}

winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<winrt::weak_ref<ReactNativeHost>>>
ReactNativeHostProperty() noexcept {
  static winrt::Microsoft::ReactNative::ReactPropertyId<
      winrt::Microsoft::ReactNative::ReactNonAbiValue<winrt::weak_ref<ReactNativeHost>>>
      propId{L"ReactNative.ReactNativeHost", L"ReactNativeHost"};
  return propId;
}

/*static*/ winrt::Microsoft::ReactNative::ReactNativeHost ReactNativeHost::GetReactNativeHost(
    ReactPropertyBag const &properties) noexcept {
  if (auto wkHost = properties.Get(ReactNativeHostProperty()).Value()) {
    if (auto abiHost = wkHost.get()) {
      return abiHost.as<winrt::Microsoft::ReactNative::ReactNativeHost>();
    }
  }
  return nullptr;
}

IAsyncAction ReactNativeHost::ReloadInstance() noexcept {
  auto modulesProvider = std::make_shared<NativeModulesProvider>();

#ifndef CORE_ABI
  auto viewManagersProvider = std::make_shared<ViewManagersProvider>();
#endif

  auto turboModulesProvider = std::make_shared<TurboModulesProvider>();

  m_packageBuilder = make<ReactPackageBuilder>(
      modulesProvider,
#ifndef CORE_ABI
      viewManagersProvider,
#endif
      turboModulesProvider);

  if (auto packageProviders = InstanceSettings().PackageProviders()) {
    for (auto const &packageProvider : packageProviders) {
      packageProvider.CreatePackage(m_packageBuilder);
    }
  }

  ReactPropertyBag(m_instanceSettings.Properties()).Set(ReactNativeHostProperty(), get_weak());

  Mso::React::ReactOptions reactOptions{};
  reactOptions.Properties = m_instanceSettings.Properties();
  reactOptions.Notifications = m_instanceSettings.Notifications();
  reactOptions.SetUseDeveloperSupport(m_instanceSettings.UseDeveloperSupport());
  reactOptions.DeveloperSettings.SourceBundleName = to_string(m_instanceSettings.DebugBundlePath());
  reactOptions.SetUseWebDebugger(m_instanceSettings.UseWebDebugger());
  reactOptions.SetUseDirectDebugger(m_instanceSettings.UseDirectDebugger());
  reactOptions.SetDebuggerBreakOnNextLine(m_instanceSettings.DebuggerBreakOnNextLine());
  reactOptions.SetUseFastRefresh(m_instanceSettings.UseFastRefresh());
  reactOptions.SetUseLiveReload(m_instanceSettings.UseLiveReload());
  reactOptions.EnableJITCompilation = m_instanceSettings.EnableJITCompilation();
  reactOptions.BundleRootPath = to_string(m_instanceSettings.BundleRootPath());
  reactOptions.DeveloperSettings.DebuggerPort = m_instanceSettings.DebuggerPort();
  if (m_instanceSettings.RedBoxHandler()) {
    reactOptions.RedBoxHandler = Mso::React::CreateRedBoxHandler(m_instanceSettings.RedBoxHandler());
  }
  reactOptions.DeveloperSettings.SourceBundleHost = to_string(m_instanceSettings.SourceBundleHost());
  reactOptions.DeveloperSettings.SourceBundlePort = m_instanceSettings.SourceBundlePort();

  reactOptions.ByteCodeFileUri = to_string(m_instanceSettings.ByteCodeFileUri());
  reactOptions.EnableByteCodeCaching = m_instanceSettings.EnableByteCodeCaching();
  reactOptions.JsiEngine = static_cast<Mso::React::JSIEngine>(m_instanceSettings.JSIEngineOverride());

  reactOptions.ModuleProvider = modulesProvider;
#ifndef CORE_ABI
  reactOptions.ViewManagerProvider = viewManagersProvider;
#endif
  reactOptions.TurboModuleProvider = turboModulesProvider;

  reactOptions.OnInstanceCreated = [](Mso::CntPtr<Mso::React::IReactContext> &&context) {
    auto notifications = context->Notifications();
    ReactInstanceSettings::RaiseInstanceCreated(
        notifications, winrt::make<InstanceCreatedEventArgs>(std::move(context)));
  };
  reactOptions.OnInstanceLoaded = [](Mso::CntPtr<Mso::React::IReactContext> &&context, const Mso::ErrorCode &err) {
    auto notifications = context->Notifications();
    ReactInstanceSettings::RaiseInstanceLoaded(
        notifications, winrt::make<InstanceLoadedEventArgs>(std::move(context), !!err));
  };
  reactOptions.OnInstanceDestroyed = [](Mso::CntPtr<Mso::React::IReactContext> &&context) {
    auto notifications = context->Notifications();
    ReactInstanceSettings::RaiseInstanceDestroyed(
        notifications, winrt::make<InstanceDestroyedEventArgs>(std::move(context)));
  };

  std::string jsBundleFile = to_string(m_instanceSettings.JavaScriptBundleFile());
  if (jsBundleFile.empty()) {
    jsBundleFile = "index.windows";
  }

  reactOptions.Identity = jsBundleFile;
  return make<Mso::AsyncActionFutureAdapter>(m_reactHost->ReloadInstanceWithOptions(std::move(reactOptions)));
}

IAsyncAction ReactNativeHost::UnloadInstance() noexcept {
  return make<Mso::AsyncActionFutureAdapter>(m_reactHost->UnloadInstance());
}

Mso::React::IReactHost *ReactNativeHost::ReactHost() noexcept {
  return m_reactHost.Get();
}

} // namespace winrt::Microsoft::ReactNative::implementation
