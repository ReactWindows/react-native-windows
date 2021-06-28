// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <ReactCommon/CallInvoker.h>
#include <Shared/BatchingMessageQueueThread.h>
#include <thread>

namespace facebook::react {
class Instance;
}

namespace Microsoft::ReactNative {

struct BatchingQueueCallInvoker : facebook::react::CallInvoker {
  BatchingQueueCallInvoker(std::shared_ptr<facebook::react::MessageQueueThread> const &queueThread);

  void invokeAsync(std::function<void()> &&func) noexcept override;
  void EnsureQueue() noexcept;
  void onBatchComplete() noexcept;
  void quitSynchronous() noexcept;
  void PostBatch() noexcept;
  void invokeSync(std::function<void()> &&func) noexcept override;

 private:
  std::shared_ptr<facebook::react::MessageQueueThread> m_queueThread;

  using WorkItemQueue = std::vector<std::function<void()>>;
  std::shared_ptr<WorkItemQueue> m_taskQueue;
};

// Executes the function on the provided UI Dispatcher
struct BatchingQueueThread final : facebook::react::BatchingMessageQueueThread {
  BatchingQueueThread(std::shared_ptr<facebook::react::MessageQueueThread> const &queueThread) noexcept;
  ~BatchingQueueThread() noexcept override;

  BatchingQueueThread() = delete;
  BatchingQueueThread(const BatchingQueueThread &other) = delete;

  void decoratedNativeCallInvokerReady(std::weak_ptr<facebook::react::Instance> wkInstance) noexcept override;

 public: // facebook::react::MessageQueueThread
  void runOnQueue(std::function<void()> &&func) noexcept override;
  void runOnQueueSync(std::function<void()> &&func) noexcept override;
  void quitSynchronous() noexcept override;

 public: // facebook::react::BatchingMessageQueueThread
  void onBatchComplete() noexcept override;

 private:
  std::mutex m_mutex;
  std::mutex m_mutexQuitting;
  bool m_quitting{false};
  std::shared_ptr<facebook::react::CallInvoker> m_callInvoker;
  std::shared_ptr<BatchingQueueCallInvoker> m_batchingQueueCallInvoker;
};

} // namespace Microsoft::ReactNative
