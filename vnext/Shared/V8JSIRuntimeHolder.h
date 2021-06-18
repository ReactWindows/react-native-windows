// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <DevSettings.h>

#include <JSI/Shared/RuntimeHolder.h>
#include <JSI/Shared/ScriptStore.h>

namespace facebook {
namespace react {

class V8JSIRuntimeHolder : public facebook::jsi::RuntimeHolderLazyInit {
 public:
  std::shared_ptr<facebook::jsi::Runtime> getRuntime() noexcept override;

  V8JSIRuntimeHolder(
      std::shared_ptr<facebook::react::DevSettings> devSettings,
      std::shared_ptr<facebook::react::MessageQueueThread> jsQueue,
      std::unique_ptr<facebook::jsi::PreparedScriptStore> &&preparedScriptStore) noexcept
      : useDirectDebugger_(devSettings->useDirectDebugger),
        debuggerBreakOnNextLine_(devSettings->debuggerBreakOnNextLine),
        debuggerRuntimeName_(devSettings->debuggerRuntimeName),
        debuggerPort_(devSettings->debuggerPort),
        jsQueue_(std::move(jsQueue)),
        preparedScriptStore_(std::move(preparedScriptStore)) {}

 private:
  void initRuntime() noexcept;

  std::shared_ptr<facebook::jsi::Runtime> runtime_;
  std::shared_ptr<facebook::react::MessageQueueThread> jsQueue_;

  std::unique_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore_;

  std::once_flag once_flag_;
  std::thread::id own_thread_id_;

  uint16_t debuggerPort_;
  bool useDirectDebugger_;
  bool debuggerBreakOnNextLine_;
  std::string debuggerRuntimeName_;
};

} // namespace react
} // namespace facebook
