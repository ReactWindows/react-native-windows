// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <DevSettings.h>

#include <JSI/RuntimeHolder.h>
#include <JSI/ScriptStore.h>

#include <Logging.h>

namespace facebook {
namespace react {

class V8JSIRuntimeHolder : public facebook::jsi::RuntimeHolderLazyInit {
 public:
  std::shared_ptr<facebook::jsi::Runtime> getRuntime() noexcept override;

  V8JSIRuntimeHolder(
      std::shared_ptr<facebook::react::DevSettings> devSettings,
      std::shared_ptr<facebook::react::MessageQueueThread> jsQueue,
      std::unique_ptr<facebook::jsi::ScriptStore> &&scriptStore,
      std::unique_ptr<facebook::jsi::PreparedScriptStore> &&preparedScriptStore) noexcept
      : useDirectDebugger_(devSettings->useDirectDebugger),
        debuggerBreakOnNextLine_(devSettings->debuggerBreakOnNextLine),
        debuggerPort_(devSettings->debuggerPort),
        debuggerRuntimeName_(devSettings->debuggerRuntimeName),
        jsQueue_(std::move(jsQueue)),
        scriptStore_(std::move(scriptStore)),
        preparedScriptStore_(std::move(preparedScriptStore)) {}

 private:
  void initRuntime() noexcept;

  std::shared_ptr<facebook::jsi::Runtime> runtime_;
  std::shared_ptr<facebook::react::MessageQueueThread> jsQueue_;

  std::unique_ptr<facebook::jsi::ScriptStore> scriptStore_;
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
