// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "BatchingEventEmitter.h"
#include "DynamicWriter.h"
#include "JSValueWriter.h"

namespace winrt::Microsoft::ReactNative {

BatchingEventEmitter::BatchingEventEmitter(Mso::CntPtr<const Mso::React::IReactContext> &&context) noexcept
    : m_context(std::move(context)) {
  m_uiDispatcher = m_context->Properties().Get(ReactDispatcherHelper::UIDispatcherProperty()).as<IReactDispatcher>();
}

void BatchingEventEmitter::DispatchEvent(
    int64_t tag,
    winrt::hstring &&eventName,
    const JSValueArgWriter &eventDataWriter) noexcept {
  return EmitJSEvent(
      L"RCTEventEmitter",
      L"receiveEvent",
      [tag, eventName = std::move(eventName), eventDataWriter](const IJSValueWriter &paramsWriter) mutable {
        paramsWriter.WriteArrayBegin();
        WriteValue(paramsWriter, tag);
        WriteValue(paramsWriter, std::move(eventName));
        eventDataWriter(paramsWriter);
        paramsWriter.WriteArrayEnd();
      });
}

void BatchingEventEmitter::EmitJSEvent(
    winrt::hstring &&eventEmitterName,
    winrt::hstring &&emitterMethod,
    const JSValueArgWriter &eventDataWriter) noexcept {
  VerifyElseCrash(m_uiDispatcher.HasThreadAccess());

  implementation::BatchedEvent newEvent{
      std::move(eventEmitterName), std::move(emitterMethod), L"", 0, DynamicWriter::ToDynamic(eventDataWriter)};
  bool isFirstEventInBatch = false;

  {
    std::scoped_lock lock(m_eventQueueMutex);

    isFirstEventInBatch = m_eventQueue.size() == 0;
    m_eventQueue.push_back(std::move(newEvent));
  }

  if (isFirstEventInBatch) {
    RegisterFrameCallback();
  }
}

void BatchingEventEmitter::DispatchCoalescingEvent(
    int64_t tag,
    winrt::hstring &&eventName,
    const JSValueArgWriter &eventDataWriter) noexcept {
  EmitCoalescingJSEvent(
      L"RCTEventEmitter",
      L"receiveEvent",
      std::move(eventName),
      tag,
      [tag, eventName = std::move(eventName), &eventDataWriter](const IJSValueWriter &paramsWriter) {
        paramsWriter.WriteArrayBegin();
        WriteValue(paramsWriter, tag);
        WriteValue(paramsWriter, std::move(eventName));
        eventDataWriter(paramsWriter);
        paramsWriter.WriteArrayEnd();
      });
}

void BatchingEventEmitter::EmitCoalescingJSEvent(
    winrt::hstring &&eventEmitterName,
    winrt::hstring &&emitterMethod,
    winrt::hstring &&eventName,
    int64_t coalescingKey,
    const JSValueArgWriter &params) noexcept {
  VerifyElseCrash(m_uiDispatcher.HasThreadAccess());

  implementation::BatchedEvent newEvent{
      std::move(eventEmitterName),
      std::move(emitterMethod),
      std::move(eventName),
      coalescingKey,
      DynamicWriter::ToDynamic(params)};
  bool isFirstEventInBatch = false;

  {
    std::scoped_lock lock(m_eventQueueMutex);

    isFirstEventInBatch = m_eventQueue.size() == 0;

    AddOrCoalesceEvent(std::move(newEvent));
  }

  if (isFirstEventInBatch) {
    RegisterFrameCallback();
  }
}

void BatchingEventEmitter::RegisterFrameCallback() noexcept {
  VerifyElseCrash(!m_renderingRevoker);

  m_renderingRevoker = xaml::Media::CompositionTarget::Rendering(
      winrt::auto_revoke, [weakThis{weak_from_this()}](auto const &, auto const &) {
        if (auto strongThis = weakThis.lock()) {
          strongThis->OnFrameUI();
        }
      });
}

size_t BatchingEventEmitter::GetCoalescingEventKey(
    const winrt::hstring &eventEmitterName,
    const winrt::hstring &emitterMethod,
    const winrt::hstring &eventName) {
  const auto iter = m_coalescingEventIds.find(std::forward_as_tuple(eventEmitterName, emitterMethod, eventName));
  if (iter == m_coalescingEventIds.end()) {
    const auto size = m_coalescingEventIds.size();
    m_coalescingEventIds.insert({std::make_tuple(eventEmitterName, emitterMethod, eventName), size});
    return size;
  }

  return iter->second;
}

void BatchingEventEmitter::AddOrCoalesceEvent(implementation::BatchedEvent &&evt) {
  const auto eventId = GetCoalescingEventKey(evt.eventEmitterName, evt.emitterMethod, evt.eventName);
  const std::tuple<int64_t, size_t> lastEventKey{evt.coalescingKey, eventId};
  const auto iter = m_lastEventIndex.find(lastEventKey);
  if (iter == m_lastEventIndex.end()) {
    const auto index = m_eventQueue.size();
    m_eventQueue.push_back(std::move(evt));
    m_lastEventIndex.insert({lastEventKey, index});
  } else {
    m_eventQueue.at(iter->second).params = std::move(evt.params);
  }
}

void BatchingEventEmitter::OnFrameUI() noexcept {
  auto jsDispatcher = m_context->Properties().Get(ReactDispatcherHelper::JSDispatcherProperty()).as<IReactDispatcher>();

  jsDispatcher.Post([weakThis{weak_from_this()}]() noexcept {
    if (auto strongThis = weakThis.lock()) {
      strongThis->OnFrameJS();
    }
  });

  // Don't leave the callback continuously registered as it can waste power.
  // See https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.media.compositiontarget.rendering?view=winrt-19041
  m_renderingRevoker.revoke();
}

void BatchingEventEmitter::OnFrameJS() noexcept {
  std::deque<implementation::BatchedEvent> currentBatch;

  {
    std::scoped_lock lock(m_eventQueueMutex);
    currentBatch.swap(m_eventQueue);
    m_lastEventIndex.clear();
  }

  while (!currentBatch.empty()) {
    auto &evt = currentBatch.front();
    m_context->CallJSFunction(
        winrt::to_string(evt.eventEmitterName), winrt::to_string(evt.emitterMethod), std::move(evt.params));
    currentBatch.pop_front();
  }
}

} // namespace winrt::Microsoft::ReactNative
