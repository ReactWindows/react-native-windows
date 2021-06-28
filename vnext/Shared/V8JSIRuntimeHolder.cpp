// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <V8JsiRuntime.h>
#include "V8JSIRuntimeHolder.h"

#include <atomic>
#include <queue>

using namespace facebook;
using namespace facebook::react;

namespace facebook {
namespace react {

class TaskRunnerAdapter : public v8runtime::JSITaskRunner {
 public:
  TaskRunnerAdapter(std::shared_ptr<facebook::react::MessageQueueThread> jsQueue) : jsQueue_(std::move(jsQueue)) {}

  void postTask(std::unique_ptr<v8runtime::JSITask> task) override {
    std::shared_ptr<v8runtime::JSITask> shared_task(task.release());
    jsQueue_->runOnQueue([shared_task2 = std::move(shared_task)]() { shared_task2->run(); });
  }

 private:
  TaskRunnerAdapter(const TaskRunnerAdapter &) = delete;
  TaskRunnerAdapter &operator=(const TaskRunnerAdapter &) = delete;

  std::shared_ptr<facebook::react::MessageQueueThread> jsQueue_;
};

std::shared_ptr<facebook::jsi::Runtime> V8JSIRuntimeHolder::getRuntime() noexcept {
  std::call_once(once_flag_, [this]() { initRuntime(); });

  if (!runtime_)
    std::terminate();

  // V8JsiRuntime is not thread safe as of now.
  if (own_thread_id_ != std::this_thread::get_id())
    std::terminate();

  return runtime_;
}

void V8JSIRuntimeHolder::initRuntime() noexcept {
  v8runtime::V8RuntimeArgs args{};

  if (debuggerPort_ > 0)
    args.inspectorPort = debuggerPort_;

  args.flags.enableInspector = useDirectDebugger_;
  args.flags.waitForDebugger = debuggerBreakOnNextLine_;
  args.debuggerRuntimeName = debuggerRuntimeName_;

  args.foreground_task_runner = std::make_shared<TaskRunnerAdapter>(jsQueue_);
  args.preparedScriptStore = std::move(preparedScriptStore_);

  runtime_ = v8runtime::makeV8Runtime(std::move(args));

  own_thread_id_ = std::this_thread::get_id();
}

} // namespace react
} // namespace facebook
