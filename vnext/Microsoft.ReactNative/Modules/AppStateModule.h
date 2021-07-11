// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <NativeModules.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>

namespace Microsoft::ReactNative {

REACT_STRUCT(AppStateChangeArgs)
struct AppStateChangeArgs {
  REACT_FIELD(app_state)
  std::string app_state;
};

REACT_MODULE(AppState)
struct AppState : public std::enable_shared_from_this<AppState> {
  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_METHOD(GetCurrentAppState, L"getCurrentAppState")
  void GetCurrentAppState(
      std::function<void(React::JSValue const &)> const &success,
      std::function<void(React::JSValue const &)> const &error) noexcept;

  REACT_METHOD(AddListener, L"addListener")
  void AddListener(std::string &&eventName) noexcept;

  REACT_METHOD(RemoveListeners, L"removeListeners")
  void RemoveListeners(double count) noexcept;

  REACT_CONSTANT(initialAppState)
  const std::string initialAppState{"active"};

  REACT_EVENT(AppStateDidChange, L"appStateDidChange")
  std::function<void(AppStateChangeArgs const &)> AppStateDidChange;

 private:
  void SetActive(bool active) noexcept;

  std::mutex m_stateMutex;
  std::atomic<bool> m_active;
  char const *m_lastState{nullptr};
  React::ReactContext m_context;
#ifndef USE_WINUI3
  xaml::Application::EnteredBackground_revoker m_enteredBackgroundRevoker;
  xaml::Application::LeavingBackground_revoker m_leavingBackgroundRevoker;
#endif
};

} // namespace Microsoft::ReactNative
