// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "InstanceCreatedEventArgs.g.h"
#include "InstanceDestroyedEventArgs.g.h"
#include "InstanceLoadedEventArgs.g.h"
#include "ReactInstanceSettings.g.h"
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include "React.h"
#include "ReactPropertyBag.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct InstanceCreatedEventArgs : InstanceCreatedEventArgsT<InstanceCreatedEventArgs> {
  InstanceCreatedEventArgs() = default;
  InstanceCreatedEventArgs(Mso::CntPtr<Mso::React::IReactContext> &&context);

  winrt::Microsoft::ReactNative::IReactContext Context() noexcept;

 private:
  winrt::Microsoft::ReactNative::IReactContext m_context;
};

struct InstanceLoadedEventArgs : InstanceLoadedEventArgsT<InstanceLoadedEventArgs> {
  InstanceLoadedEventArgs() = default;
  InstanceLoadedEventArgs(Mso::CntPtr<Mso::React::IReactContext> &&context, bool failed);

  winrt::Microsoft::ReactNative::IReactContext Context() noexcept;
  bool Failed() noexcept;

 private:
  winrt::Microsoft::ReactNative::IReactContext m_context;
  bool m_failed;
};

struct InstanceDestroyedEventArgs : InstanceDestroyedEventArgsT<InstanceDestroyedEventArgs> {
  InstanceDestroyedEventArgs() = default;
  InstanceDestroyedEventArgs(Mso::CntPtr<Mso::React::IReactContext> &&context);

  winrt::Microsoft::ReactNative::IReactContext Context() noexcept;

 private:
  winrt::Microsoft::ReactNative::IReactContext m_context;
};

struct ReactInstanceSettings : ReactInstanceSettingsT<ReactInstanceSettings> {
  ReactInstanceSettings() noexcept;

  IReactPropertyBag Properties() noexcept;

  IReactNotificationService Notifications() noexcept;

  Windows::Foundation::Collections::IVector<IReactPackageProvider> PackageProviders() noexcept;

  //! This controls the availability of various developer support functionality including
  //! RedBox, and the Developer Menu
  bool UseDeveloperSupport() noexcept;
  void UseDeveloperSupport(bool value) noexcept;

  hstring JavaScriptBundleFile() noexcept;
  void JavaScriptBundleFile(hstring const &value) noexcept;

  //! Should the instance run in a remote environment such as within a browser
  //! By default, this is using a browser navigated to  http://localhost:8081/debugger-ui served
  //! by Metro/Haul. Debugging will start as soon as the React Native instance is loaded.
  bool UseWebDebugger() noexcept;
  void UseWebDebugger(bool value) noexcept;

  //! Should the instance trigger the hot reload logic when it first loads the instance
  //! Most edits should be visible within a second or two without the instance having to reload
  //! Non-compatible changes still cause full reloads
  bool UseFastRefresh() noexcept;
  void UseFastRefresh(bool value) noexcept;

  //! Should the instance monitor for changes to the JS and reload the instance when a change is
  //! detected.  Generally its prefered to use FastFreshed instead of this.  But if there is some
  //! issue with hot reloading in your app, then this can be used instead
  bool UseLiveReload() noexcept;
  void UseLiveReload(bool value) noexcept;

  //! Should the instance setup the JS environment such that a JS debugger can attach to the JS environment
  bool UseDirectDebugger() noexcept;
  void UseDirectDebugger(bool value) noexcept;

  //! Should the instance setup the JS environment to immediately break waiting for a JS debugger
  //! This allows a JS debugger to be able to debug the boot JS code with in the instance
  bool DebuggerBreakOnNextLine() noexcept;
  void DebuggerBreakOnNextLine(bool value) noexcept;

  bool EnableJITCompilation() noexcept;
  void EnableJITCompilation(bool value) noexcept;

  bool EnableByteCodeCaching() noexcept;
  void EnableByteCodeCaching(bool value) noexcept;

  //! Same as UseDeveloperSupport
  bool EnableDeveloperMenu() noexcept;
  void EnableDeveloperMenu(bool value) noexcept;

  hstring ByteCodeFileUri() noexcept;
  void ByteCodeFileUri(hstring const &value) noexcept;

  hstring DebugBundlePath() noexcept;
  void DebugBundlePath(hstring const &value) noexcept;

  hstring BundleRootPath() noexcept;
  void BundleRootPath(hstring const &value) noexcept;

  uint16_t DebuggerPort() noexcept;
  void DebuggerPort(uint16_t value) noexcept;

  IRedBoxHandler RedBoxHandler() noexcept;
  void RedBoxHandler(IRedBoxHandler const &value) noexcept;

  IReactDispatcher UIDispatcher() noexcept;
  void UIDispatcher(IReactDispatcher const &value) noexcept;

  hstring SourceBundleHost() noexcept;
  void SourceBundleHost(hstring const &value) noexcept;

  uint16_t SourceBundlePort() noexcept;
  void SourceBundlePort(uint16_t value) noexcept;

  JSIEngine JSIEngineOverride() noexcept;
  void JSIEngineOverride(JSIEngine value) noexcept;

  winrt::event_token InstanceCreated(
      Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::InstanceCreatedEventArgs> const
          &handler) noexcept;
  void InstanceCreated(winrt::event_token const &token) noexcept;

  winrt::event_token InstanceLoaded(
      Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::InstanceLoadedEventArgs> const
          &handler) noexcept;
  void InstanceLoaded(winrt::event_token const &token) noexcept;

  winrt::event_token InstanceDestroyed(
      Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::InstanceDestroyedEventArgs> const
          &handler) noexcept;
  void InstanceDestroyed(winrt::event_token const &token) noexcept;

  static void RaiseInstanceCreated(
      IReactNotificationService const &notificationService,
      winrt::Microsoft::ReactNative::InstanceCreatedEventArgs const &args) noexcept;
  static void RaiseInstanceLoaded(
      IReactNotificationService const &notificationService,
      winrt::Microsoft::ReactNative::InstanceLoadedEventArgs const &args) noexcept;
  static void RaiseInstanceDestroyed(
      IReactNotificationService const &notificationService,
      winrt::Microsoft::ReactNative::InstanceDestroyedEventArgs const &args) noexcept;

 private:
  IReactPropertyBag m_properties{ReactPropertyBagHelper::CreatePropertyBag()};
  IReactNotificationService m_notifications{ReactNotificationServiceHelper::CreateNotificationService()};
  Windows::Foundation::Collections::IVector<IReactPackageProvider> m_packageProviders{
      single_threaded_vector<IReactPackageProvider>()};
  hstring m_javaScriptBundleFile{};
  bool m_enableJITCompilation{true};
  bool m_enableByteCodeCaching{false};
  hstring m_byteCodeFileUri{};
  hstring m_debugBundlePath{};
  hstring m_bundleRootPath{};
  uint16_t m_debuggerPort{9229};
  IRedBoxHandler m_redBoxHandler{nullptr};
  hstring m_sourceBundleHost{};
  uint16_t m_sourceBundlePort{0};

#if USE_HERMES
  JSIEngine m_jSIEngineOverride{JSIEngine::Hermes};
#elif USE_V8
  JSIEngine m_jSIEngineOverride{JSIEngine::V8};
#else
  JSIEngine m_jSIEngineOverride{JSIEngine::Chakra};
#endif
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct ReactInstanceSettings : ReactInstanceSettingsT<ReactInstanceSettings, implementation::ReactInstanceSettings> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation

namespace winrt::Microsoft::ReactNative::implementation {

//=============================================================================================
// ReactInstanceSettings inline implementation
//=============================================================================================

inline IReactPropertyBag ReactInstanceSettings::Properties() noexcept {
  return m_properties;
}

inline IReactNotificationService ReactInstanceSettings::Notifications() noexcept {
  return m_notifications;
}

inline Windows::Foundation::Collections::IVector<IReactPackageProvider>
ReactInstanceSettings::PackageProviders() noexcept {
  return m_packageProviders;
}

inline hstring ReactInstanceSettings::JavaScriptBundleFile() noexcept {
  return m_javaScriptBundleFile;
}

inline void ReactInstanceSettings::JavaScriptBundleFile(hstring const &value) noexcept {
  m_javaScriptBundleFile = value;
}

inline bool ReactInstanceSettings::EnableJITCompilation() noexcept {
  return m_enableJITCompilation;
}

inline void ReactInstanceSettings::EnableJITCompilation(bool value) noexcept {
  m_enableJITCompilation = value;
}

inline bool ReactInstanceSettings::EnableByteCodeCaching() noexcept {
  return m_enableByteCodeCaching;
}

inline void ReactInstanceSettings::EnableByteCodeCaching(bool value) noexcept {
  m_enableByteCodeCaching = value;
}

inline hstring ReactInstanceSettings::ByteCodeFileUri() noexcept {
  return m_byteCodeFileUri;
}

inline void ReactInstanceSettings::ByteCodeFileUri(hstring const &value) noexcept {
  m_byteCodeFileUri = value;
}

inline hstring ReactInstanceSettings::DebugBundlePath() noexcept {
  return m_debugBundlePath;
}

inline void ReactInstanceSettings::DebugBundlePath(hstring const &value) noexcept {
  m_debugBundlePath = value;
}

inline hstring ReactInstanceSettings::BundleRootPath() noexcept {
  return m_bundleRootPath;
}

inline void ReactInstanceSettings::BundleRootPath(hstring const &value) noexcept {
  m_bundleRootPath = value;
}

inline uint16_t ReactInstanceSettings::DebuggerPort() noexcept {
  return m_debuggerPort;
}

inline void ReactInstanceSettings::DebuggerPort(uint16_t value) noexcept {
  m_debuggerPort = value;
}

inline IRedBoxHandler ReactInstanceSettings::RedBoxHandler() noexcept {
  return m_redBoxHandler;
}

inline void ReactInstanceSettings::RedBoxHandler(IRedBoxHandler const &value) noexcept {
  m_redBoxHandler = value;
}

inline hstring ReactInstanceSettings::SourceBundleHost() noexcept {
  return m_sourceBundleHost;
}

inline void ReactInstanceSettings::SourceBundleHost(hstring const &value) noexcept {
  m_sourceBundleHost = value;
}

inline uint16_t ReactInstanceSettings::SourceBundlePort() noexcept {
  return m_sourceBundlePort;
}

inline void ReactInstanceSettings::SourceBundlePort(uint16_t value) noexcept {
  m_sourceBundlePort = value;
}

inline JSIEngine ReactInstanceSettings::JSIEngineOverride() noexcept {
  return m_jSIEngineOverride;
}

inline void ReactInstanceSettings::JSIEngineOverride(JSIEngine value) noexcept {
  m_jSIEngineOverride = value;
}

} // namespace winrt::Microsoft::ReactNative::implementation
