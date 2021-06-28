// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <mutex>
#include <unordered_map>
#include "AsyncActionQueue.h"
#include "IReactInstanceInternal.h"
#include "InstanceFactory.h"
#include "MsoUtils.h"
#include "React.h"
#include "activeObject/activeObject.h"
#include "object/refCountedObject.h"

namespace Mso::React {

class ReactViewHost;

//! ReactHost manages lifetime of ReactNative instance.
//! It is associated with a native queue that is used modify its state.
//! - It can reload or unload react instance using Reload and Unload.
//! - ReactInstance() returns current React Native instance.
//! - NativeQueue() returns the associate native queue.
//! - Options() - returns options used for creating ReactInstance.
//!
//! About the RNH closing sequence:
//! - Every time a new RNH is created it is added to the global registry.
//! - During registration we associate Mso::Future with each RNH
//! - The Mso::Future deletes the RNH from the registry.
//! - When the Liblet::Uninit starts, it turns the registry into the uninited state.
//! - In the uninited state new RNH cannot be registered. They are created as already closed.
//! - All operations in closed RNH are canceled.
//! - All registered RNHs are closed. No new work can be done.
//! - We wait in Liblet::Uninit when all pending Futures to indicate closed RNH state are completed.
class ReactHost final : public Mso::ActiveObject<IReactHost> {
  using Super = ActiveObjectType;

 public: // IReactHost
  ReactOptions Options() const noexcept override;
  Mso::CntPtr<IReactInstance> Instance() const noexcept override;
  Mso::DispatchQueue const &NativeQueue() const noexcept override;
  Mso::Future<void> ReloadInstance() noexcept override;
  Mso::Future<void> ReloadInstanceWithOptions(ReactOptions &&options) noexcept override;
  Mso::Future<void> UnloadInstance() noexcept override;
  Mso::CntPtr<IReactViewHost> MakeViewHost(ReactViewOptions &&options) noexcept override;
  Mso::Future<std::vector<Mso::CntPtr<IReactViewHost>>> GetViewHostList() noexcept override;

 public:
  void AttachViewHost(ReactViewHost &viewHost) noexcept;
  void DetachViewHost(ReactViewHost &viewHost) noexcept;

  Mso::CntPtr<AsyncActionQueue> ActionQueue() const noexcept;

  Mso::Future<void> LoadInQueue(ReactOptions &&options) noexcept;
  Mso::Future<void> UnloadInQueue(size_t unloadActionId) noexcept;

  void Close() noexcept;
  bool IsClosed() const noexcept;

  size_t PendingUnloadActionId() const noexcept;
  bool IsInstanceUnloading() const noexcept;

  template <class TCallback>
  Mso::Future<void> PostInQueue(TCallback &&callback) noexcept;

 private:
  friend MakePolicy;
  ReactHost(Mso::DispatchQueue const &queue) noexcept;
  ~ReactHost() noexcept override;
  void Finalize() noexcept override;

  static Mso::DispatchQueue EnsureSerialQueue(Mso::DispatchQueue const &queue) noexcept;

  void ForEachViewHost(const Mso::FunctorRef<void(ReactViewHost &)> &action) noexcept;

  AsyncAction MakeLoadInstanceAction(ReactOptions &&options) noexcept;
  AsyncAction MakeUnloadInstanceAction() noexcept;

 private:
  mutable std::mutex m_mutex;
  const Mso::InvokeElsePostExecutor m_executor{Queue()};
  const Mso::ActiveReadableField<Mso::CntPtr<AsyncActionQueue>> m_actionQueue{
      Mso::Make<AsyncActionQueue>(Queue()),
      Queue(),
      m_mutex};
  const Mso::ActiveField<Mso::ErrorCode> m_lastError{Queue()};
  const Mso::ActiveReadableField<ReactOptions> m_options{Queue(), m_mutex};
  const Mso::ActiveField<std::unordered_map<uintptr_t, Mso::WeakPtr<ReactViewHost>>> m_viewHosts{Queue()};
  const Mso::ActiveReadableField<Mso::CntPtr<IReactInstanceInternal>> m_reactInstance{Queue(), m_mutex};
  const Mso::ActiveReadableField<Mso::Promise<void>> m_notifyWhenClosed{nullptr, Queue(), m_mutex};
  size_t m_pendingUnloadActionId{0};
  size_t m_nextUnloadActionId{0};
  const Mso::ActiveField<bool> m_isInstanceUnloading{false, Queue()};
};

//! Implements a cross-platform host for a React view
class ReactViewHost final : public ActiveObject<IReactViewHost> {
  using Super = ActiveObjectType;

 public: // IReactViewHost
  ReactViewOptions Options() const noexcept override;
  IReactHost &ReactHost() const noexcept override;
  Mso::Future<void> ReloadViewInstance() noexcept override;
  Mso::Future<void> ReloadViewInstanceWithOptions(ReactViewOptions &&options) noexcept override;
  Mso::Future<void> UnloadViewInstance() noexcept override;
  Mso::Future<void> AttachViewInstance(IReactViewInstance &viewInstance) noexcept override;
  Mso::Future<void> DetachViewInstance() noexcept override;

 public:
  Mso::Future<void> InitViewInstanceInQueue() noexcept;
  Mso::Future<void> InitViewInstanceInQueue(ReactViewOptions &&options) noexcept;
  Mso::Future<void> UpdateViewInstanceInQueue() noexcept;
  Mso::Future<void> UninitViewInstanceInQueue(size_t unloadActionId) noexcept;

 private:
  friend MakePolicy;
  ReactViewHost(Mso::React::ReactHost &reactHost, ReactViewOptions &&options) noexcept;
  ~ReactViewHost() noexcept override;

  void SetOptions(ReactViewOptions &&options) noexcept;

  AsyncAction MakeInitViewInstanceAction() noexcept;
  AsyncAction MakeInitViewInstanceAction(ReactViewOptions &&options) noexcept;
  AsyncAction MakeUninitViewInstanceAction() noexcept;

  template <class TCallback>
  Mso::Future<void> PostInQueue(TCallback &&callback) noexcept;

 private:
  mutable std::mutex m_mutex;
  const Mso::InvokeElsePostExecutor m_executor{Queue()};
  const Mso::ActiveField<Mso::CntPtr<AsyncActionQueue>> m_actionQueue{Queue()};
  const Mso::CntPtr<Mso::React::ReactHost> m_reactHost;
  const Mso::ActiveReadableField<ReactViewOptions> m_options{Queue(), m_mutex};
  const Mso::ActiveField<Mso::CntPtr<IReactViewInstance>> m_viewInstance{Queue()};
  const Mso::ActiveField<size_t> m_pendingUninitActionId{0, Queue()};
  const Mso::ActiveField<size_t> m_nextUninitActionId{0, Queue()};
  const Mso::ActiveField<bool> m_isViewInstanceInited{false, Queue()};
};

//! ReactHostRegistry helps with closing of all ReactHosts on Liblet::Uninit.
class ReactHostRegistry final : public Mso::RefCountedObjectNoVTable<ReactHostRegistry> {
 public:
  ReactHostRegistry() = default;
  MSO_NO_COPY_CTOR_AND_ASSIGNMENT(ReactHostRegistry);

  ~ReactHostRegistry() noexcept;

  // Adds ReactHost to the global registry and returns promise to be set when closed.
  // It returns an empty Mso::Promise in case if RNH cannot be registered.
  static Mso::Promise<void> Register(ReactHost &reactHost) noexcept;

  static void OnLibletInit() noexcept;
  static void OnLibletUninit() noexcept;

 private:
  struct Entry {
    Mso::WeakPtr<ReactHost> WeakReactHost;
    Mso::Future<void> WhenReactHostClosed;
  };

 private:
  // Removes the ReactHost from the global registry
  void Unregister(uintptr_t reactHostKey) noexcept;

 private:
  static std::mutex s_mutex; // protects access to s_registry and m_entries.
  static Mso::CntPtr<ReactHostRegistry> s_registry;

 private:
  // m_entries is an instance field to ensure that we have correct behavior
  // in case when tests run multiple Liblet Init/Uninit.
  // The instance field ensures that all Unregister calls use the correct set on entries.
  std::unordered_map<uintptr_t, Entry> m_entries;
};

//=============================================================================================
// Inline implementations
//=============================================================================================

template <class TCallback>
Mso::Future<void> ReactHost::PostInQueue(TCallback &&callback) noexcept {
  using Callback = std::decay_t<TCallback>;
  return Mso::PostFuture(
      m_executor,
      [weakThis = Mso::WeakPtr{this}, callback = Callback{std::forward<TCallback>(callback)}]() mutable noexcept {
        if (auto strongThis = weakThis.GetStrongPtr()) {
          return callback();
        }

        return Mso::MakeCanceledFuture();
      });
}

template <class TCallback>
Mso::Future<void> ReactViewHost::PostInQueue(TCallback &&callback) noexcept {
  using Callback = std::decay_t<TCallback>;
  return m_reactHost->PostInQueue(
      [weakThis = Mso::WeakPtr{this}, callback = Callback{std::forward<TCallback>(callback)}]() mutable noexcept {
        if (auto strongThis = weakThis.GetStrongPtr()) {
          return callback();
        }

        return Mso::MakeCanceledFuture();
      });
}

} // namespace Mso::React
