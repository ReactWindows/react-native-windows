// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Threading/BatchingQueueThread.h"
#include <cxxreact/Instance.h>
#include <eventWaitHandle/eventWaitHandle.h>
#include <cassert>

namespace Microsoft::ReactNative {

BatchingQueueCallInvoker::BatchingQueueCallInvoker(
    std::shared_ptr<facebook::react::MessageQueueThread> const &queueThread)
    : m_queueThread(queueThread) {}

void BatchingQueueCallInvoker::invokeAsync(std::function<void()> &&func) noexcept {
  EnsureQueue();
  m_taskQueue->emplace_back(std::move(func));

//#define TRACK_UI_CALLS
#ifdef TRACK_UI_CALLS
  char buffer[1024];
  static uint32_t cCalls = 0;
  _snprintf_s(buffer, _countof(buffer), _TRUNCATE, "BatchingUIMessageQueueThread Calls: %u\r\n", ++cCalls);
  OutputDebugStringA(buffer);
#endif
}

void BatchingQueueCallInvoker::EnsureQueue() noexcept {
  if (!m_taskQueue) {
    m_taskQueue = std::make_shared<WorkItemQueue>();
    m_taskQueue->reserve(2048);
  }
}

void BatchingQueueCallInvoker::PostBatch() noexcept {
  if (m_taskQueue) {
    m_queueThread->runOnQueue([taskQueue{std::move(m_taskQueue)}]() noexcept {
      for (auto &task : *taskQueue) {
        task();
        task = nullptr;
      }
    });
  }
}

void BatchingQueueCallInvoker::onBatchComplete() noexcept {
  PostBatch();
}

void BatchingQueueCallInvoker::quitSynchronous() noexcept {
  PostBatch();
  m_queueThread->quitSynchronous();
}

void BatchingQueueCallInvoker::invokeSync(std::function<void()> &&func) noexcept {
  assert(false && "Not supported");
  std::terminate();
}

BatchingQueueThread::BatchingQueueThread(
    std::shared_ptr<facebook::react::MessageQueueThread> const &queueThread) noexcept {
  m_batchingQueueCallInvoker = std::make_shared<BatchingQueueCallInvoker>(queueThread);
  m_callInvoker = m_batchingQueueCallInvoker;
}

// Called from the JS thread, once the instance has had a chance to setup the NativeToJsBridge and calls to
// getDecoratedNativeCallInvoker will work.
void BatchingQueueThread::decoratedNativeCallInvokerReady(
    std::weak_ptr<facebook::react::Instance> wkInstance) noexcept {
  std::scoped_lock lck(m_mutex);
  m_callInvoker->invokeAsync([wkInstance, this] {
    if (auto instance = wkInstance.lock()) {
      std::scoped_lock lckQuitting(m_mutexQuitting);

      // If we are shutting down, then then the mutex is being held in quitSynchronous
      // Which is waiting for this task to complete, so we cannot take the mutex if quitSynchronous
      // is running. -- and since we are shutting down anyway, we can just skip this work.
      if (!m_quitting) {
        std::scoped_lock lck(m_mutex);
        m_callInvoker = instance->getDecoratedNativeCallInvoker(m_callInvoker);
      }
    }
  });
}

BatchingQueueThread::~BatchingQueueThread() noexcept {}

void BatchingQueueThread::runOnQueue(std::function<void()> &&func) noexcept {
  std::scoped_lock lck(m_mutex);
  m_callInvoker->invokeAsync(std::move(func));
}

void BatchingQueueThread::onBatchComplete() noexcept {
  std::scoped_lock lck(m_mutex);
  m_batchingQueueCallInvoker->onBatchComplete();
}

void BatchingQueueThread::runOnQueueSync(std::function<void()> && /*func*/) noexcept {
  assert(false && "Not supported");
  std::terminate();
}

void BatchingQueueThread::quitSynchronous() noexcept {
  {
    std::scoped_lock lckQuitting(m_mutexQuitting);
    m_quitting = true;
  }
  std::scoped_lock lck(m_mutex);
  m_batchingQueueCallInvoker->quitSynchronous();
}

} // namespace Microsoft::ReactNative
