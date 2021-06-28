// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactInstanceSettings.h"
#if __has_include("ReactInstanceSettings.g.cpp")
#include "ReactInstanceSettings.g.cpp"
#endif

#include "IReactContext.h"
#include "ReactHost/React.h"
#include "ReactNotificationService.h"

namespace winrt::Microsoft::ReactNative::implementation {

InstanceCreatedEventArgs::InstanceCreatedEventArgs(Mso::CntPtr<Mso::React::IReactContext> &&context)
    : m_context(winrt::make<ReactContext>(std::move(context))) {}

winrt::Microsoft::ReactNative::IReactContext InstanceCreatedEventArgs::Context() noexcept {
  return m_context;
}

InstanceLoadedEventArgs::InstanceLoadedEventArgs(Mso::CntPtr<Mso::React::IReactContext> &&context, bool failed)
    : m_context(winrt::make<ReactContext>(std::move(context))), m_failed(failed) {}

bool InstanceLoadedEventArgs::Failed() noexcept {
  return m_failed;
}

winrt::Microsoft::ReactNative::IReactContext InstanceLoadedEventArgs::Context() noexcept {
  return m_context;
}

InstanceDestroyedEventArgs::InstanceDestroyedEventArgs(Mso::CntPtr<Mso::React::IReactContext> &&context)
    : m_context(winrt::make<ReactContext>(std::move(context))) {}

winrt::Microsoft::ReactNative::IReactContext InstanceDestroyedEventArgs::Context() noexcept {
  return m_context;
}

ReactInstanceSettings::ReactInstanceSettings() noexcept {
  // Use current thread dispatcher as a default UI dispatcher.
  m_properties.Set(ReactDispatcherHelper::UIDispatcherProperty(), ReactDispatcherHelper::UIThreadDispatcher());
}

IReactDispatcher ReactInstanceSettings::UIDispatcher() noexcept {
  return m_properties.Get(ReactDispatcherHelper::UIDispatcherProperty()).try_as<IReactDispatcher>();
}

void ReactInstanceSettings::UIDispatcher(IReactDispatcher const &value) noexcept {
  m_properties.Set(ReactDispatcherHelper::UIDispatcherProperty(), value);
}

bool ReactInstanceSettings::UseDeveloperSupport() noexcept {
  return Mso::React::ReactOptions::UseDeveloperSupport(m_properties);
}

void ReactInstanceSettings::UseDeveloperSupport(bool value) noexcept {
  Mso::React::ReactOptions::SetUseDeveloperSupport(m_properties, value);
}

bool ReactInstanceSettings::UseFastRefresh() noexcept {
  return Mso::React::ReactOptions::UseFastRefresh(m_properties);
}

void ReactInstanceSettings::UseFastRefresh(bool value) noexcept {
  Mso::React::ReactOptions::SetUseFastRefresh(m_properties, value);
}

bool ReactInstanceSettings::UseLiveReload() noexcept {
  return Mso::React::ReactOptions::UseLiveReload(m_properties);
}

void ReactInstanceSettings::UseLiveReload(bool value) noexcept {
  Mso::React::ReactOptions::SetUseLiveReload(m_properties, value);
}

bool ReactInstanceSettings::UseWebDebugger() noexcept {
  return Mso::React::ReactOptions::UseWebDebugger(m_properties);
}

void ReactInstanceSettings::UseWebDebugger(bool value) noexcept {
  Mso::React::ReactOptions::SetUseWebDebugger(m_properties, value);
}

bool ReactInstanceSettings::UseDirectDebugger() noexcept {
  return Mso::React::ReactOptions::UseDirectDebugger(m_properties);
}

void ReactInstanceSettings::UseDirectDebugger(bool value) noexcept {
  Mso::React::ReactOptions::SetUseDirectDebugger(m_properties, value);
}

bool ReactInstanceSettings::DebuggerBreakOnNextLine() noexcept {
  return Mso::React::ReactOptions::DebuggerBreakOnNextLine(m_properties);
}

void ReactInstanceSettings::DebuggerBreakOnNextLine(bool value) noexcept {
  Mso::React::ReactOptions::SetDebuggerBreakOnNextLine(m_properties, value);
}

bool ReactInstanceSettings::EnableDeveloperMenu() noexcept {
  return UseDeveloperSupport();
}

void ReactInstanceSettings::EnableDeveloperMenu(bool value) noexcept {
  UseDeveloperSupport(value);
}

IReactPropertyNamespace InstanceSettingsNamespace() noexcept {
  static IReactPropertyNamespace value = ReactPropertyBagHelper::GetNamespace(L"ReactNative.InstanceSettings");
  return value;
}

IReactPropertyName InstanceCreatedEventName() noexcept {
  static IReactPropertyName propName = ReactPropertyBagHelper::GetName(InstanceSettingsNamespace(), L"InstanceCreated");
  return propName;
}

IReactPropertyName InstanceLoadedEventName() noexcept {
  static IReactPropertyName propName = ReactPropertyBagHelper::GetName(InstanceSettingsNamespace(), L"InstanceLoaded");
  return propName;
}

IReactPropertyName InstanceDestroyedEventName() noexcept {
  static IReactPropertyName propName =
      ReactPropertyBagHelper::GetName(InstanceSettingsNamespace(), L"InstanceDestroyed");
  return propName;
}

/*static*/ void ReactInstanceSettings::RaiseInstanceCreated(
    IReactNotificationService const &notificationService,
    winrt::Microsoft::ReactNative::InstanceCreatedEventArgs const &args) noexcept {
  notificationService.SendNotification(InstanceCreatedEventName(), nullptr, args);
}

/*static*/ void ReactInstanceSettings::RaiseInstanceLoaded(
    IReactNotificationService const &notificationService,
    winrt::Microsoft::ReactNative::InstanceLoadedEventArgs const &args) noexcept {
  notificationService.SendNotification(InstanceLoadedEventName(), nullptr, args);
}

/*static*/ void ReactInstanceSettings::RaiseInstanceDestroyed(
    IReactNotificationService const &notificationService,
    winrt::Microsoft::ReactNative::InstanceDestroyedEventArgs const &args) noexcept {
  notificationService.SendNotification(InstanceDestroyedEventName(), nullptr, args);
}

// Subscribes to a notification from a ReactNotificationService, then returns the IReactNotificationSubscription as a
// event_token This allows easy implementation of winrt events which are implemented using the ReactNotifictionService.
template <typename argsT>
winrt::event_token subscribeToNotifications(
    IReactNotificationService const &notifications,
    IReactPropertyName const &property,
    Windows::Foundation::EventHandler<argsT> const &handler) {
  auto subscription = notifications.Subscribe(
      property,
      nullptr,
      [handler](winrt::Windows::Foundation::IInspectable sender, const IReactNotificationArgs &args) {
        handler(sender, args.Data().as<argsT>());
      });

  winrt::event_token token;
  token.value = reinterpret_cast<int64_t>(winrt::detach_abi(subscription));
  return token;
}

// Unsubscribe from a ReactNotification using an event_token that was returned from subscribeToNotifications
void unsubscribeFromNotifications(winrt::event_token const &token) {
  if (token) {
    winrt::Windows::Foundation::IUnknown unk;
    IReactNotificationSubscription subscription;
    winrt::attach_abi(unk, reinterpret_cast<void *>(token.value));
    unk.as(subscription);
    subscription.Unsubscribe();
  }
}

winrt::event_token ReactInstanceSettings::InstanceCreated(
    Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::InstanceCreatedEventArgs> const
        &handler) noexcept {
  return subscribeToNotifications(Notifications(), InstanceCreatedEventName(), handler);
}

void ReactInstanceSettings::InstanceCreated(winrt::event_token const &token) noexcept {
  unsubscribeFromNotifications(token);
}

winrt::event_token ReactInstanceSettings::InstanceLoaded(
    Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::InstanceLoadedEventArgs> const &handler) noexcept {
  return subscribeToNotifications(Notifications(), InstanceLoadedEventName(), handler);
}

void ReactInstanceSettings::InstanceLoaded(winrt::event_token const &token) noexcept {
  unsubscribeFromNotifications(token);
}

winrt::event_token ReactInstanceSettings::InstanceDestroyed(
    Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::InstanceDestroyedEventArgs> const
        &handler) noexcept {
  return subscribeToNotifications(Notifications(), InstanceDestroyedEventName(), handler);
}

void ReactInstanceSettings::InstanceDestroyed(winrt::event_token const &token) noexcept {
  unsubscribeFromNotifications(token);
}

} // namespace winrt::Microsoft::ReactNative::implementation
