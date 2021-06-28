// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "JSBundle.h"
#include "RedBoxHandler.h"
#include "dispatchQueue/dispatchQueue.h"
#include "errorCode/errorCode.h"
#include "future/future.h"

#include <NativeModuleProvider.h>
#include <TurboModulesProvider.h>

#ifdef CORE_ABI
#include <folly/dynamic.h>
#undef GetCurrentTime
#endif

#ifndef CORE_ABI
// The IReactInstance.h brings dependency on XAML. Exclude it for the UI technology independent code.
#include <IReactInstance.h>
#endif

#include <Shared/IReactRootView.h>

#include <ViewManagerProvider.h>
#include <winrt/Microsoft.ReactNative.h>

namespace Mso::React {

// Forward declarations
struct IReactInstance;
struct IReactViewHost;
struct ReactOptions;
struct IReactContext;

enum class LogLevel : int32_t {
  Trace = 0,
  Info = 1,
  Warning = 2,
  Error = 3,
  Fatal = 4,
};

enum class JSIEngine : int32_t {
  Chakra = 0, // Use the JSIExecutorFactory with ChakraRuntime
  Hermes = 1, // Use the JSIExecutorFactory with Hermes
  V8 = 2, // Use the JSIExecutorFactory with V8
};

using OnErrorCallback = Mso::Functor<void(const Mso::ErrorCode &)>;
using OnLoggingCallback = Mso::Functor<void(LogLevel logLevel, const char *message)>;
using OnReactInstanceCreatedCallback = Mso::Functor<void(Mso::CntPtr<IReactContext> &&)>;
using OnReactInstanceLoadedCallback = Mso::Functor<void(Mso::CntPtr<IReactContext> &&, const Mso::ErrorCode &)>;
using OnReactInstanceDestroyedCallback = Mso::Functor<void(Mso::CntPtr<IReactContext> &&)>;

//! Returns default OnError handler.
LIBLET_PUBLICAPI OnErrorCallback GetDefaultOnErrorHandler() noexcept;

enum class ReactInstanceState {
  Loading,
  WaitingForDebugger,
  Loaded,
  HasError,
  Unloaded,
};

/**An Office wrapper that extends FB's React Instance and makes it a 1:1 relationship with the bundle,
such that each Office React Instance is capable of loading a single bundle and hosting js in an
isolated JavaScript execution environment.*/
MSO_GUID(IReactInstance, "085d524a-af3b-4839-8056-e5d0e6fc64bc")
struct IReactInstance : IUnknown {
  //! Returns ReactOptions associated with the IReactInstance
  //! The ReactOptions are meant to be immutable and given to IReactInstance at its creation.
  virtual const ReactOptions &Options() const noexcept = 0;

  virtual ReactInstanceState State() const noexcept = 0;

  virtual Mso::React::IReactContext &GetReactContext() const noexcept = 0;

  virtual void AttachMeasuredRootView(
      facebook::react::IReactRootView *rootView,
      folly::dynamic &&initialProps,
      bool useFabric) noexcept = 0;
  virtual void DetachRootView(facebook::react::IReactRootView *rootView, bool useFabric) noexcept = 0;
};

MSO_GUID(IReactSettingsSnapshot, "6652bb2e-4c5e-49f7-b642-e817b0fef4de")
struct IReactSettingsSnapshot : IUnknown {
  virtual bool UseWebDebugger() const noexcept = 0;
  virtual bool UseFastRefresh() const noexcept = 0;
  virtual bool UseDirectDebugger() const noexcept = 0;
  virtual bool DebuggerBreakOnNextLine() const noexcept = 0;
  virtual uint16_t DebuggerPort() const noexcept = 0;
  virtual std::string DebugBundlePath() const noexcept = 0;
  virtual std::string BundleRootPath() const noexcept = 0;
  virtual std::string SourceBundleHost() const noexcept = 0;
  virtual uint16_t SourceBundlePort() const noexcept = 0;
  virtual std::string JavaScriptBundleFile() const noexcept = 0;
  virtual bool UseDeveloperSupport() const noexcept = 0;
};

MSO_GUID(IReactContext, "a4309a29-8fc5-478e-abea-0ddb9ecc5e40")
struct IReactContext : IUnknown {
  virtual winrt::Microsoft::ReactNative::IReactNotificationService Notifications() const noexcept = 0;
  virtual winrt::Microsoft::ReactNative::IReactPropertyBag Properties() const noexcept = 0;
  virtual void CallJSFunction(std::string &&module, std::string &&method, folly::dynamic &&params) const noexcept = 0;
  virtual void DispatchEvent(int64_t viewTag, std::string &&eventName, folly::dynamic &&eventData) const noexcept = 0;
  virtual winrt::Microsoft::ReactNative::JsiRuntime JsiRuntime() const noexcept = 0;
#ifndef CORE_ABI
  virtual ReactInstanceState State() const noexcept = 0;
  virtual bool IsLoaded() const noexcept = 0;
  virtual std::shared_ptr<facebook::react::Instance> GetInnerInstance() const noexcept = 0;
  virtual IReactSettingsSnapshot const &SettingsSnapshot() const noexcept = 0;
#endif
};

//! Settings per each IReactViewHost associated with an IReactHost instance.
struct ReactViewOptions {
  //! Name of a component registered in JavaScript via AppRegistry.registerComponent('ModuleName', () => ModuleName);
  std::string ComponentName;

  //! Set of initial component properties. It is a JSON string.
  folly::dynamic InitialProps;

  //! Use Fabric for this ReactView
  bool UseFabric{false};
};

struct ReactDevOptions {
  //! For direct debugging, allow specifying the port to use for the debugger.
  uint16_t DebuggerPort{0};

  //! For direct debugging, specifies a name to associate with the JavaScript runtime instance.
  std::string DebuggerRuntimeName;

  //! When using web debugging and/or live reload, the source is obtained from the packager.
  //! The source url for the bundle is "http://{HOST}:{PORT}/{NAME}{EXTENSION}?platform=..."
  //! which defaults to:
  //! {HOST} = "localhost",
  //! {PORT} = "8081",
  //! {NAME} = "index.{PLATFORM}" where {PLATFORM} = "ios", "android", "win32", etc.
  //! {EXTENSION} = ".bundle"
  //! Specify a value for a component, or leave empty to use the default.
  std::string SourceBundleHost; // Host domain (without port) for the bundler server. Default: "localhost".
  uint16_t SourceBundlePort{0}; // Host port for the bundler server. Default: "8081".
  std::string SourceBundleName; // Bundle name without any extension (e.g. "index.win32"). Default: "index.{PLATFORM}"
  std::string SourceBundleExtension; // Bundle name extension. Default: ".bundle".

  //! Module name used for loading the debug bundle.
  //! e.g. The modules name registered in the jsbundle via AppRegistry.registerComponent('ModuleName', () =>
  //! ModuleName);
  std::string SourceBundleModuleName;
};

struct NativeModuleProvider2 {
  virtual std::vector<facebook::react::NativeModuleDescription> GetModules(
      Mso::CntPtr<IReactContext> const &reactContext,
      std::shared_ptr<facebook::react::MessageQueueThread> const &defaultQueueThread) = 0;
};

struct ViewManagerProvider2 {
  virtual std::vector<std::unique_ptr<::Microsoft::ReactNative::IViewManager>> GetViewManagers(
      Mso::CntPtr<IReactContext> const &reactContext) = 0;
};

//! A simple struct that describes the basic properties/needs of an SDX. Whenever a new SDX is
//! getting hosted in React, properties here will be used to construct the SDX.
struct ReactOptions {
  winrt::Microsoft::ReactNative::IReactPropertyBag Properties;

  winrt::Microsoft::ReactNative::IReactNotificationService Notifications;

  std::shared_ptr<NativeModuleProvider2> ModuleProvider;
  std::shared_ptr<ViewManagerProvider2> ViewManagerProvider;
  std::shared_ptr<winrt::Microsoft::ReactNative::TurboModulesProvider> TurboModuleProvider;

  //! Identity of the SDX. Must uniquely describe the SDX across the installed product.
  std::string Identity;

  //! List of names of Cxx modules that all ReactIntances hosting this SDX should be configured with.
  //!
  //! In addition to this list, there are following hardcoded modules.
  //!
  //! Cxx Modules Provided by ReactNativeHost:
  //! RekaBridge: Implemented by Mso::React::RekaNativeModule
  //!
  //! Cxx Modules Provided by ReactNative:
  //! WebSocketModule: Implemented by facebook::react::WebSocketModule
  //! Timing: Implemented by facebook::react::TimingModule
  //! SourceCode: Implemented by facebook::react::SourceCodeModule
  //! ExceptionsManager: Implemented by facebook::react::ExceptionsManagerModule
  std::vector<std::string> CxxModuleNames;

  //! List of names of Java modules that all ReactIntances hosting this SDX should be configured with.
  std::vector<std::string> JavaModuleNames;

  //! (Optional) Name of the registered IDataServiceProvider to use when creating data services
  //! for this SDX.  Providers are registered using RegisterDataServiceProvider().
  //! Or, specify the reserved name DefaultDataServiceProviderName to delegate to
  //! the DataServiceRegistration facility.
  std::string DataServiceProviderName;

  //! Base path of the SDX. The absolute path of the SDX can be constructed from this and the Identity.
  std::string BundleRootPath;

  //! Javascript Bundles
  //! This List includes both Platform and User Javascript Bundles
  //! Bundles are loaded into Javascript engine in the same order
  //! as they are specified in this list.
  std::vector<Mso::CntPtr<IJSBundle>> JSBundles;

  //! ReactNative Infrastructure Error
  //! Error types include:
  //! * Any call to Javascript function after Global Exception has been raised
  //! * Any WebServer error when DeveloperSettings.UseWebDebugger is true
  //! Note: Default callback generates ShipAssert.
  OnErrorCallback OnError{GetDefaultOnErrorHandler()};

  //! The callback is called when JavaScript code logs a message through the console.log class of APIs.
  //! Note: this is currently only used in Win32 (Chakra Executor)
  OnLoggingCallback OnLogging;

  //! Ability to override the default redbox handling, which is used
  //! during development to report JavaScript errors to users
  std::shared_ptr<Mso::React::IRedBoxHandler> RedBoxHandler;

  //! Flag to suggest sdx owner's preference on enabling Bytecode caching in Javascript Engine for corresponding SDX.
  bool EnableBytecode{true};

  //! Flag controlling whether the JavaScript engine uses JIT compilation.
  bool EnableJITCompilation{true};

  std::string ByteCodeFileUri;
  bool EnableByteCodeCaching{true};
  JSIEngine JsiEngine{JSIEngine::Chakra};

  //! Enable function nativePerformanceNow.
  //! Method nativePerformanceNow() returns high resolution time info.
  //! It is not safe to expose to Custom Function. Add this flag so we can turn it off for Custom Function.
  bool EnableNativePerformanceNow{true};

  ReactDevOptions DeveloperSettings = {};

  //! This controls the availability of various developer support functionality including
  //! RedBox, and the Developer Menu
  void SetUseDeveloperSupport(bool enable) noexcept;
  bool UseDeveloperSupport() const noexcept;
  static void SetUseDeveloperSupport(
      winrt::Microsoft::ReactNative::IReactPropertyBag const &properties,
      bool value) noexcept;
  static bool UseDeveloperSupport(winrt::Microsoft::ReactNative::IReactPropertyBag const &properties) noexcept;

  //! Should the instance trigger the hot reload logic when it first loads the instance
  //! Most edits should be visible within a second or two without the instance having to reload
  //! Non-compatible changes still cause full reloads
  void SetUseFastRefresh(bool enable) noexcept;
  bool UseFastRefresh() const noexcept;
  static void SetUseFastRefresh(
      winrt::Microsoft::ReactNative::IReactPropertyBag const &properties,
      bool value) noexcept;
  static bool UseFastRefresh(winrt::Microsoft::ReactNative::IReactPropertyBag const &properties) noexcept;

  //! Enable live reload to load the source bundle from the React Native packager.
  //! When the file is saved, the packager will trigger reloading.
  void SetUseLiveReload(bool enable) noexcept;
  bool UseLiveReload() const noexcept;
  static void SetUseLiveReload(winrt::Microsoft::ReactNative::IReactPropertyBag const &properties, bool value) noexcept;
  static bool UseLiveReload(winrt::Microsoft::ReactNative::IReactPropertyBag const &properties) noexcept;

  //! Should the instance run in a remote environment such as within a browser
  //! By default, this is using a browser navigated to  http://localhost:8081/debugger-ui served
  //! by Metro/Haul. Debugging will start as soon as the React Native instance is loaded.
  void SetUseWebDebugger(bool enabled) noexcept;
  bool UseWebDebugger() const noexcept;
  static void SetUseWebDebugger(
      winrt::Microsoft::ReactNative::IReactPropertyBag const &properties,
      bool value) noexcept;
  static bool UseWebDebugger(winrt::Microsoft::ReactNative::IReactPropertyBag const &properties) noexcept;

  //! For direct debugging, whether to break on the next line of JavaScript that is executed.
  void SetDebuggerBreakOnNextLine(bool enable) noexcept;
  bool DebuggerBreakOnNextLine() const noexcept;
  static void SetDebuggerBreakOnNextLine(
      winrt::Microsoft::ReactNative::IReactPropertyBag const &properties,
      bool value) noexcept;
  static bool DebuggerBreakOnNextLine(winrt::Microsoft::ReactNative::IReactPropertyBag const &properties) noexcept;

  //! Enables debugging in the JavaScript engine (if supported).
  void SetUseDirectDebugger(bool enable) noexcept;
  bool UseDirectDebugger() const noexcept;
  static void SetUseDirectDebugger(
      winrt::Microsoft::ReactNative::IReactPropertyBag const &properties,
      bool value) noexcept;
  static bool UseDirectDebugger(winrt::Microsoft::ReactNative::IReactPropertyBag const &properties) noexcept;

  //! Adds registered JS bundle to JSBundles.
  LIBLET_PUBLICAPI ReactOptions &AddRegisteredJSBundle(std::string_view jsBundleId) noexcept;

  //! Adds JS bundle based on a file name to JSBundles.
  LIBLET_PUBLICAPI ReactOptions &AddFileJSBundle(std::string_view jsBundleId, std::string_view fileName) noexcept;

  //! The callback is called when IReactInstance is created, and JS bundles are started to load.
  //! It is called from the native queue.
  OnReactInstanceCreatedCallback OnInstanceCreated;

  //! The callback is called when IReactInstance finished loading JS bundles.
  //! If there were errors, then the provided Mso::ErrorCode is not empty.
  //! The callback is always called regardless if JSBundles were loaded or not.
  //! This callback is called from the native queue.
  //!
  //! Error types include:
  //! * Javascript Syntax Error
  //! * Global Javascript Exception Thrown
  //!    Including Javascript Exceptions during initialization
  OnReactInstanceLoadedCallback OnInstanceLoaded;

  //! The callback is called when IReactInstance is destroyed and must not be used anymore.
  //! It is called from the native queue.
  OnReactInstanceDestroyedCallback OnInstanceDestroyed;
};

//! IReactHost manages a RactNative instance.
MSO_GUID(IReactHost, "2a2474ff-264f-449a-9852-0463e6ac6bbf")
struct DECLSPEC_NOVTABLE IReactHost : IUnknown {
  //! Returns a copy of react options.
  virtual ReactOptions Options() const noexcept = 0;

  //! Returns current ReactInstance. The result can be null if instance is unloaded.
  virtual Mso::CntPtr<IReactInstance> Instance() const noexcept = 0;

  //! A native sequential queue associated with IReactHost.
  //! All internal operations are done using this queue to synchronize internal state.
  virtual Mso::DispatchQueue const &NativeQueue() const noexcept = 0;

  //! Reloads the ReactNative instance and associated ReactViews.
  virtual Mso::Future<void> ReloadInstance() noexcept = 0;

  //! Updates ReactOptions associated with the IReactHost and reloads the ReactNative instance.
  virtual Mso::Future<void> ReloadInstanceWithOptions(ReactOptions &&options) noexcept = 0;

  //! Unloads the ReactNative instance and associated ReactViews.
  virtual Mso::Future<void> UnloadInstance() noexcept = 0;

  //! Creates a new instance of IReactViewHost.
  //! The IReactViewHost is added to the list of view hosts only after a IReactViewInstance is attached to it.
  virtual Mso::CntPtr<IReactViewHost> MakeViewHost(ReactViewOptions &&options) noexcept = 0;

  //! Returns a snapshot of IReactViewHost instances associated with the IReactHost.
  virtual Mso::Future<std::vector<Mso::CntPtr<IReactViewHost>>> GetViewHostList() noexcept = 0;
};

//! Each platform has a ReactViewInstance where it hosts the React UI tree.
//! This ReactViewInstance implements the IReactViewInstance interface.
//! When the ReactViewInstance is created, it is given a IReactViewHost instance.
//! The IReactViewInstance should attach itself to the IReactViewHost instance at creation time, and then
//! detach from it when destroyed.
//! After the IReactViewInstance is attached to the IReactViewHost, the IReactViewHost is attached to its
//! IReactHost. The IReactViewHost is detached from IReactHost after when the IReactViewInstance is detached.
//! The IReactViewHost has a set of Options associated with it that are used by IReactViewInstance to create
//! the React UI tree.
//! The IReactViewHost is responsible for calling IReactViewInstance Reload() and Unload() methods to
//! reload and unload the UI tree.
//! The IReactViewHost also provides an access to IReactHost associated with it.

//! This interface is to be implemented by a platform specific ReactViewInstance that hosts React UI tree.
MSO_GUID(IReactViewInstance, "29e04f14-9fc9-4dd7-a543-e59db0d57bd2")
struct IReactViewInstance : IUnknown {
  //! Initialize ReactRootView with the reactInstance and view-specific settings.
  virtual Mso::Future<void> InitRootView(
      Mso::CntPtr<Mso::React::IReactInstance> &&reactInstance,
      ReactViewOptions &&viewOptions) noexcept = 0;

  //! Update ReactRootView with changes in ReactInstance.
  virtual Mso::Future<void> UpdateRootView() noexcept = 0;

  //! Uninitialize ReactRootView and destroy UI tree.
  virtual Mso::Future<void> UninitRootView() noexcept = 0;
};

//! A host object for ReactView.
//! Each IReactViewHost has association with its parent IReactHost.
//! IReactHost has a weak reference to IReactViewHost when the IReactViewHost is bound to IReactViewInstance.
//! Typically platform-specific implementation of IReactViewInstance is created using
//! IReactViewHost instance options, and then attaches itself to the IReactViewHost.
//! When IReactViewInstance is destroyed, it detaches itself from the IReactViewHost.
MSO_GUID(IReactViewHost, "cc9f0c95-ad43-43af-a834-0ef9adc411c4")
struct IReactViewHost : IUnknown {
  //! Returns a copy of current react view options.
  virtual ReactViewOptions Options() const noexcept = 0;

  //! Returns IReactHost associated with this IReactViewHost.
  virtual IReactHost &ReactHost() const noexcept = 0;

  //! Reloads the IReactViewInstance if it is attached.
  virtual Mso::Future<void> ReloadViewInstance() noexcept = 0;

  //! Reloads IReactViewInstance if it is attached with a new set of options.
  virtual Mso::Future<void> ReloadViewInstanceWithOptions(ReactViewOptions &&options) noexcept = 0;

  //! Unloads the attached IReactViewInstance.
  virtual Mso::Future<void> UnloadViewInstance() noexcept = 0;

  //! Attaches IReactViewInstance to the IReactViewHost.
  virtual Mso::Future<void> AttachViewInstance(IReactViewInstance &viewInstance) noexcept = 0;

  //! Detaches IReactViewInstance from the IReactViewHost.
  virtual Mso::Future<void> DetachViewInstance() noexcept = 0;
};

LIBLET_PUBLICAPI Mso::CntPtr<IReactHost> MakeReactHost() noexcept;

} // namespace Mso::React
