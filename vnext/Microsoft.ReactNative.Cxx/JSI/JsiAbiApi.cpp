// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "JsiAbiApi.h"
#include <utility>
#include "winrt/Windows.Foundation.Collections.h"

using namespace facebook::jsi;

namespace winrt::Microsoft::ReactNative {

// The macro to simplify recording JSI exceptions.
// It looks strange to keep the normal structure of the try/catch in code.
#define JSI_RUNTIME_SET_ERROR(runtime)                               \
facebook::jsi::JSError const &jsError) {                             \
    JsiAbiRuntime::GetFromJsiRuntime(runtime)->SetJsiError(jsError); \
    throw;                                                           \
  }                                                                  \
  catch (std::exception const &ex) {                                 \
    JsiAbiRuntime::GetFromJsiRuntime(runtime)->SetJsiError(ex);      \
    throw;                                                           \
  } catch (...

//===========================================================================
// AbiJSError implementation
//===========================================================================

struct AbiJSError : facebook::jsi::JSError {
  AbiJSError(JsiAbiRuntime &rt, JsiError &&jsiError) noexcept
      : facebook::jsi::
            JSError{to_string(jsiError.ErrorDetails()), rt, facebook::jsi::Value(rt, JsiAbiRuntime::ValueRef(jsiError.Value()))},
        m_jsiError{std::move(jsiError)} {}

 private:
  JsiError m_jsiError;
};

//===========================================================================
// AbiJSINativeException implementation
//===========================================================================

struct AbiJSINativeException : facebook::jsi::JSINativeException {
  AbiJSINativeException(JsiError &&jsiError) noexcept
      : facebook::jsi::JSINativeException{to_string(jsiError.ErrorDetails())}, m_jsiError{std::move(jsiError)} {}

 private:
  JsiError m_jsiError;
};

//===========================================================================
// JsiBufferWrapper implementation
//===========================================================================

JsiByteBufferWrapper::JsiByteBufferWrapper(
    JsiRuntime const &runtime,
    std::shared_ptr<Buffer const> const &buffer) noexcept
    : m_runtime{runtime}, m_buffer{buffer} {}

JsiByteBufferWrapper::~JsiByteBufferWrapper() = default;

uint32_t JsiByteBufferWrapper::Size() try {
  return static_cast<uint32_t>(m_buffer->size());
} catch (JSI_RUNTIME_SET_ERROR(m_runtime)) {
  throw;
}

void JsiByteBufferWrapper::GetData(JsiByteArrayUser const &useBytes) try {
  useBytes(winrt::array_view<uint8_t const>{m_buffer->data(), m_buffer->data() + m_buffer->size()});
} catch (JSI_RUNTIME_SET_ERROR(m_runtime)) {
  throw;
}

//===========================================================================
// JsiPreparedJavaScriptWrapper implementation
//===========================================================================

JsiPreparedJavaScriptWrapper::JsiPreparedJavaScriptWrapper(JsiPreparedJavaScript const &preparedScript) noexcept
    : m_preparedScript{preparedScript} {}

JsiPreparedJavaScriptWrapper::~JsiPreparedJavaScriptWrapper() = default;

JsiPreparedJavaScript const &JsiPreparedJavaScriptWrapper::Get() const noexcept {
  return m_preparedScript;
}

//===========================================================================
// JsiHostObjectWrapper implementation
//===========================================================================

JsiHostObjectWrapper::JsiHostObjectWrapper(std::shared_ptr<HostObject> &&hostObject) noexcept
    : m_hostObject(std::move(hostObject)) {}

JsiValueRef JsiHostObjectWrapper::GetProperty(JsiRuntime const &runtime, JsiPropertyIdRef const &name) try {
  JsiAbiRuntime *rt{JsiAbiRuntime::GetFromJsiRuntime(runtime)};
  JsiAbiRuntime::PropNameIDRef nameRef{name};
  return JsiAbiRuntime::DetachJsiValueRef(m_hostObject->get(*rt, nameRef));
} catch (JSI_RUNTIME_SET_ERROR(runtime)) {
  throw;
}

void JsiHostObjectWrapper::SetProperty(
    JsiRuntime const &runtime,
    JsiPropertyIdRef const &name,
    JsiValueRef const &value) try {
  JsiAbiRuntime *rt{JsiAbiRuntime::GetFromJsiRuntime(runtime)};
  m_hostObject->set(*rt, JsiAbiRuntime::PropNameIDRef{name}, JsiAbiRuntime::ValueRef(value));
} catch (JSI_RUNTIME_SET_ERROR(runtime)) {
  throw;
}

Windows::Foundation::Collections::IVector<JsiPropertyIdRef> JsiHostObjectWrapper::GetPropertyIds(
    JsiRuntime const &runtime) try {
  JsiAbiRuntime *rt{JsiAbiRuntime::GetFromJsiRuntime(runtime)};
  auto names = m_hostObject->getPropertyNames(*rt);
  std::vector<JsiPropertyIdRef> result;
  result.reserve(names.size());
  for (auto &name : names) {
    result.push_back(JsiAbiRuntime::DetachJsiPropertyIdRef(std::move(name)));
  }

  return winrt::single_threaded_vector<JsiPropertyIdRef>(std::move(result));
} catch (JSI_RUNTIME_SET_ERROR(runtime)) {
  throw;
}

std::shared_ptr<facebook::jsi::HostObject> const &JsiHostObjectWrapper::HostObjectSharedPtr() noexcept {
  return m_hostObject;
}

//===========================================================================
// JsiHostFunctionWrapper implementation
//===========================================================================

JsiHostFunctionWrapper::JsiHostFunctionWrapper(HostFunctionType &&hostFunction) noexcept
    : m_hostFunction{std::move(hostFunction)} {}

JsiValueRef JsiHostFunctionWrapper::operator()(
    JsiRuntime const &runtime,
    JsiValueRef const &thisArg,
    array_view<JsiValueRef const> args) try {
  JsiAbiRuntime *rt{JsiAbiRuntime::GetFromJsiRuntime(runtime)};
  JsiAbiRuntime::ValueRefArray valueRefArgs{args};
  return JsiAbiRuntime::DetachJsiValueRef(
      m_hostFunction(*rt, JsiAbiRuntime::ValueRef{thisArg}, valueRefArgs.Data(), valueRefArgs.Size()));
} catch (JSI_RUNTIME_SET_ERROR(runtime)) {
  throw;
}

/*static*/ HostFunctionType &JsiHostFunctionWrapper::GetHostFunction(JsiHostFunction const &hostFunction) noexcept {
  void *hostFunctionAbi = get_abi(hostFunction);
  JsiHostFunctionWrapper *self =
      static_cast<impl::delegate<JsiHostFunction, JsiHostFunctionWrapper> *>(hostFunctionAbi);
  return self->m_hostFunction;
}

//===========================================================================
// JsiAbiRuntime implementation
//===========================================================================

// The tls_jsiAbiRuntimeMap map allows us to associate JsiAbiRuntime with JsiRuntime.
// The association is thread-specific and DLL-specific.
// It is thread specific because we want to have the safe access only in JS thread.
// It is DLL-specific because JsiAbiRuntime is not ABI-safe and each module DLL will
// have their own JsiAbiRuntime instance.
static thread_local std::map<void *, JsiAbiRuntime *> *tls_jsiAbiRuntimeMap{nullptr};

JsiAbiRuntime::JsiAbiRuntime(JsiRuntime const &runtime) noexcept : m_runtime{runtime} {
  VerifyElseCrashSz(runtime, "JSI runtime is null");
  VerifyElseCrashSz(
      GetFromJsiRuntime(runtime) == nullptr,
      "We can have only one instance of JsiAbiRuntime for JsiRuntime in the thread.");
  if (!tls_jsiAbiRuntimeMap) {
    tls_jsiAbiRuntimeMap = new std::map<void *, JsiAbiRuntime *>();
  }
  tls_jsiAbiRuntimeMap->try_emplace(get_abi(runtime), this);
}

JsiAbiRuntime::~JsiAbiRuntime() {
  VerifyElseCrashSz(
      GetFromJsiRuntime(m_runtime) != nullptr, "JsiAbiRuntime must be called in the same thread where it was created.");
  tls_jsiAbiRuntimeMap->erase(get_abi(m_runtime));
  if (tls_jsiAbiRuntimeMap->empty()) {
    delete tls_jsiAbiRuntimeMap;
    tls_jsiAbiRuntimeMap = nullptr;
  }
}

/*static*/ JsiAbiRuntime *JsiAbiRuntime::GetFromJsiRuntime(JsiRuntime const &runtime) noexcept {
  if (tls_jsiAbiRuntimeMap && runtime) {
    auto it = tls_jsiAbiRuntimeMap->find(get_abi(runtime));
    if (it != tls_jsiAbiRuntimeMap->end()) {
      return it->second;
    }
  }
  return nullptr;
}

Value JsiAbiRuntime::evaluateJavaScript(const std::shared_ptr<const Buffer> &buffer, const std::string &sourceURL) try {
  return MakeValue(
      m_runtime.EvaluateJavaScript(winrt::make<JsiByteBufferWrapper>(m_runtime, buffer), to_hstring(sourceURL)));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

std::shared_ptr<const PreparedJavaScript> JsiAbiRuntime::prepareJavaScript(
    const std::shared_ptr<const Buffer> &buffer,
    std::string sourceURL) try {
  return std::make_shared<JsiPreparedJavaScriptWrapper>(
      m_runtime.PrepareJavaScript(winrt::make<JsiByteBufferWrapper>(m_runtime, buffer), to_hstring(sourceURL)));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Value JsiAbiRuntime::evaluatePreparedJavaScript(const std::shared_ptr<const PreparedJavaScript> &js) try {
  return MakeValue(
      m_runtime.EvaluatePreparedJavaScript(std::static_pointer_cast<JsiPreparedJavaScriptWrapper const>(js)->Get()));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::drainMicrotasks(int maxMicrotasksHint) try {
  return m_runtime.DrainMicrotasks(maxMicrotasksHint);
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Object JsiAbiRuntime::global() try { return MakeObject(m_runtime.Global()); } catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

std::string JsiAbiRuntime::description() try {
  return to_string(m_runtime.Description());
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::isInspectable() try { return m_runtime.IsInspectable(); } catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}
Instrumentation &JsiAbiRuntime::instrumentation() try {
  // TODO: implement
  VerifyElseCrash(false);
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Runtime::PointerValue *JsiAbiRuntime::cloneSymbol(const Runtime::PointerValue *pv) try {
  return new SymbolPointerValue{make_weak(m_runtime), m_runtime.CloneSymbol(AsJsiSymbolRef(pv))};
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Runtime::PointerValue *JsiAbiRuntime::cloneString(const Runtime::PointerValue *pv) try {
  return new StringPointerValue{make_weak(m_runtime), m_runtime.CloneString(AsJsiStringRef(pv))};
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Runtime::PointerValue *JsiAbiRuntime::cloneObject(const Runtime::PointerValue *pv) try {
  return new ObjectPointerValue{make_weak(m_runtime), m_runtime.CloneObject(AsJsiObjectRef(pv))};
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Runtime::PointerValue *JsiAbiRuntime::clonePropNameID(const Runtime::PointerValue *pv) try {
  return new PropNameIDPointerValue{make_weak(m_runtime), m_runtime.ClonePropertyId(AsJsiPropertyIdRef(pv))};
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

PropNameID JsiAbiRuntime::createPropNameIDFromAscii(const char *str, size_t length) try {
  auto data = reinterpret_cast<uint8_t const *>(str);
  return MakePropNameID(m_runtime.CreatePropertyIdFromAscii({data, data + length}));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

PropNameID JsiAbiRuntime::createPropNameIDFromUtf8(const uint8_t *utf8, size_t length) try {
  return MakePropNameID(m_runtime.CreatePropertyIdFromUtf8({utf8, utf8 + length}));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

PropNameID JsiAbiRuntime::createPropNameIDFromString(const String &str) try {
  return MakePropNameID(m_runtime.CreatePropertyIdFromString(AsJsiStringRef(str)));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

std::string JsiAbiRuntime::utf8(const PropNameID &propertyId) try {
  std::string dataResult;
  m_runtime.PropertyIdToUtf8(AsJsiPropertyIdRef(propertyId), [&dataResult](array_view<uint8_t const> utf8) {
    dataResult.assign(reinterpret_cast<char const *>(utf8.data()), utf8.size());
  });
  return dataResult;
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::compare(const PropNameID &left, const PropNameID &right) try {
  return m_runtime.PropertyIdEquals(AsJsiPropertyIdRef(left), AsJsiPropertyIdRef(right));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

std::string JsiAbiRuntime::symbolToString(const Symbol &symbol) try {
  std::string dataResult;
  m_runtime.SymbolToUtf8(AsJsiSymbolRef(symbol), [&dataResult](array_view<uint8_t const> utf8) {
    dataResult.assign(reinterpret_cast<char const *>(utf8.data()), utf8.size());
  });
  return dataResult;
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

String JsiAbiRuntime::createStringFromAscii(const char *str, size_t length) try {
  auto ascii = reinterpret_cast<uint8_t const *>(str);
  return MakeString(m_runtime.CreateStringFromAscii({ascii, ascii + length}));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

String JsiAbiRuntime::createStringFromUtf8(const uint8_t *utf8, size_t length) try {
  return MakeString(m_runtime.CreateStringFromAscii({utf8, utf8 + length}));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

std::string JsiAbiRuntime::utf8(const String &str) try {
  std::string dataResult;
  m_runtime.StringToUtf8(AsJsiStringRef(str), [&dataResult](array_view<uint8_t const> utf8) {
    dataResult.assign(reinterpret_cast<char const *>(utf8.data()), utf8.size());
  });
  return dataResult;
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Value JsiAbiRuntime::createValueFromJsonUtf8(const uint8_t *json, size_t length) try {
  return MakeValue(m_runtime.CreateValueFromJsonUtf8({json, json + length}));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Object JsiAbiRuntime::createObject() try {
  return MakeObject(m_runtime.CreateObject());
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Object JsiAbiRuntime::createObject(std::shared_ptr<HostObject> ho) try {
  auto hostObjectWrapper = winrt::make<JsiHostObjectWrapper>(std::move(ho));
  Object result = MakeObject(m_runtime.CreateObjectWithHostObject(hostObjectWrapper));
  return result;
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

std::shared_ptr<HostObject> JsiAbiRuntime::getHostObject(const Object &obj) try {
  std::shared_ptr<HostObject> result;
  if (auto jsiHostObject = m_runtime.GetHostObject(AsJsiObjectRef(obj))) {
    if (auto wrapper = get_self<JsiHostObjectWrapper>(jsiHostObject)) {
      result = wrapper->HostObjectSharedPtr();
    }
  }
  return result;
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

HostFunctionType &JsiAbiRuntime::getHostFunction(const Function &func) try {
  if (auto jsiHostFunction = m_runtime.GetHostFunction(AsJsiObjectRef(func))) {
    return JsiHostFunctionWrapper::GetHostFunction(jsiHostFunction);
  }
  throw winrt::hresult_invalid_argument(L"Not a host function");
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Value JsiAbiRuntime::getProperty(const Object &obj, const PropNameID &name) try {
  return MakeValue(m_runtime.GetProperty(AsJsiObjectRef(obj), AsJsiPropertyIdRef(name)));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Value JsiAbiRuntime::getProperty(const Object &obj, const String &name) try {
  // TODO: delete propertyId
  auto propertyId = m_runtime.CreatePropertyIdFromString(AsJsiStringRef(name));
  return MakeValue(m_runtime.GetProperty(AsJsiObjectRef(obj), propertyId));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::hasProperty(const Object &obj, const PropNameID &name) try {
  return m_runtime.HasProperty(AsJsiObjectRef(obj), AsJsiPropertyIdRef(name));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::hasProperty(const Object &obj, const String &name) try {
  // TODO: delete propertyId
  auto propertyId = m_runtime.CreatePropertyIdFromString(AsJsiStringRef(name));
  return m_runtime.HasProperty(AsJsiObjectRef(obj), propertyId);
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

void JsiAbiRuntime::setPropertyValue(Object &obj, const PropNameID &name, const Value &value) try {
  m_runtime.SetProperty(AsJsiObjectRef(obj), AsJsiPropertyIdRef(name), AsJsiValueRef(value));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

void JsiAbiRuntime::setPropertyValue(Object &obj, const String &name, const Value &value) try {
  // TODO: delete propertyId
  auto propertyId = m_runtime.CreatePropertyIdFromString(AsJsiStringRef(name));
  m_runtime.SetProperty(AsJsiObjectRef(obj), propertyId, AsJsiValueRef(value));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::isArray(const Object &obj) const try {
  return m_runtime.IsArray(AsJsiObjectRef(obj));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::isArrayBuffer(const Object &obj) const try {
  return m_runtime.IsArrayBuffer(AsJsiObjectRef(obj));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::isFunction(const Object &obj) const try {
  return m_runtime.IsFunction(AsJsiObjectRef(obj));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::isHostObject(const Object &obj) const try {
  return m_runtime.IsHostObject(AsJsiObjectRef(obj));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::isHostFunction(const Function &func) const try {
  return m_runtime.IsHostFunction(AsJsiObjectRef(func));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Array JsiAbiRuntime::getPropertyNames(const Object &obj) try {
  return MakeArray(m_runtime.GetPropertyIdArray(AsJsiObjectRef(obj)));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

WeakObject JsiAbiRuntime::createWeakObject(const Object &obj) try {
  return MakeWeakObject(m_runtime.CreateWeakObject(AsJsiObjectRef(obj)));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Value JsiAbiRuntime::lockWeakObject(WeakObject &weakObj) try {
  return MakeValue(m_runtime.LockWeakObject(AsJsiWeakObjectRef(weakObj)));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Array JsiAbiRuntime::createArray(size_t length) try {
  return MakeArray(m_runtime.CreateArray(static_cast<uint32_t>(length)));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

size_t JsiAbiRuntime::size(const Array &arr) try {
  return m_runtime.GetArraySize(AsJsiObjectRef(arr));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

size_t JsiAbiRuntime::size(const ArrayBuffer &arrayBuffer) try {
  return m_runtime.GetArrayBufferSize(AsJsiObjectRef(arrayBuffer));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

uint8_t *JsiAbiRuntime::data(const ArrayBuffer &arrayBuffer) try {
  uint8_t *dataResult{};
  m_runtime.GetArrayBufferData(AsJsiObjectRef(arrayBuffer), [&dataResult](array_view<uint8_t const> dataView) {
    dataResult = const_cast<uint8_t *>(dataView.data());
  });
  return dataResult;
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Value JsiAbiRuntime::getValueAtIndex(const Array &arr, size_t i) try {
  return MakeValue(m_runtime.GetValueAtIndex(AsJsiObjectRef(arr), static_cast<uint32_t>(i)));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

void JsiAbiRuntime::setValueAtIndexImpl(Array &arr, size_t i, const Value &value) try {
  m_runtime.SetValueAtIndex(AsJsiObjectRef(arr), static_cast<uint32_t>(i), AsJsiValueRef(value));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Function JsiAbiRuntime::createFunctionFromHostFunction(
    const PropNameID &name,
    unsigned int paramCount,
    HostFunctionType func) try {
  return MakeFunction(m_runtime.CreateFunctionFromHostFunction(
      AsJsiPropertyIdRef(name), paramCount, JsiHostFunctionWrapper{std::move(func)}));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Value JsiAbiRuntime::call(const Function &func, const Value &jsThis, const Value *args, size_t count) try {
  VerifyElseCrashSz(count <= MaxCallArgCount, "Argument count must not exceed the supported max arg count.");
  std::array<JsiValueRef, MaxCallArgCount> argsData;
  for (size_t i = 0; i < count; ++i) {
    argsData[i] = AsJsiValueRef(*(args + i));
  }

  return MakeValue(
      m_runtime.Call(AsJsiObjectRef(func), AsJsiValueRef(jsThis), {argsData.data(), argsData.data() + count}));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Value JsiAbiRuntime::callAsConstructor(const Function &func, const Value *args, size_t count) try {
  VerifyElseCrashSz(count <= MaxCallArgCount, "Argument count must not exceed the supported max arg count.");
  std::array<JsiValueRef, MaxCallArgCount> argsData;
  for (size_t i = 0; i < count; ++i) {
    argsData[i] = AsJsiValueRef(*(args + i));
  }

  return MakeValue(m_runtime.CallAsConstructor(AsJsiObjectRef(func), {argsData.data(), argsData.data() + count}));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Runtime::ScopeState *JsiAbiRuntime::pushScope() try {
  return reinterpret_cast<ScopeState *>(m_runtime.PushScope().Data);
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

void JsiAbiRuntime::popScope(ScopeState *scope) try {
  m_runtime.PopScope({reinterpret_cast<uint64_t>(scope)});
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::strictEquals(const Symbol &a, const Symbol &b) const try {
  return m_runtime.SymbolStrictEquals(AsJsiSymbolRef(a), AsJsiSymbolRef(b));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::strictEquals(const String &a, const String &b) const try {
  return m_runtime.StringStrictEquals(AsJsiStringRef(a), AsJsiStringRef(b));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::strictEquals(const Object &a, const Object &b) const try {
  return m_runtime.ObjectStrictEquals(AsJsiObjectRef(a), AsJsiObjectRef(b));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::instanceOf(const Object &o, const Function &f) try {
  return m_runtime.InstanceOf(AsJsiObjectRef(o), AsJsiObjectRef(f));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

template <typename T>
struct AutoRestore {
  AutoRestore(T *var, T value) : m_var{var}, m_value{std::exchange(*var, value)} {}

  ~AutoRestore() {
    *m_var = m_value;
  }

 private:
  T *m_var;
  T m_value;
};

void JsiAbiRuntime::RethrowJsiError() const {
  auto jsiError = m_runtime.GetAndClearError();
  if (!m_pendingJSError && jsiError.ErrorType() == JsiErrorType::JSError) {
    AutoRestore<bool> setValue{const_cast<bool *>(&m_pendingJSError), true};
    throw AbiJSError{*const_cast<JsiAbiRuntime *>(this), std::move(jsiError)};
  } else {
    throw AbiJSINativeException{std::move(jsiError)};
  }
}

void JsiAbiRuntime::SetJsiError(facebook::jsi::JSError const &jsError) noexcept {
  m_runtime.SetError(JsiErrorType::JSError, to_hstring(jsError.what()), AsJsiValueRef(jsError.value()));
}

void JsiAbiRuntime::SetJsiError(std::exception const &nativeException) noexcept {
  m_runtime.SetError(
      JsiErrorType::NativeException,
      to_hstring(nativeException.what()),
      AsJsiValueRef(facebook::jsi::Value::undefined()));
}

//===========================================================================
// JsiAbiRuntime utility functions implementation
//===========================================================================

/*static*/ JsiSymbolRef const &JsiAbiRuntime::AsJsiSymbolRef(PointerValue const *pv) noexcept {
  return SymbolPointerValue::GetData(pv);
}

/*static*/ JsiStringRef const &JsiAbiRuntime::AsJsiStringRef(PointerValue const *pv) noexcept {
  return StringPointerValue::GetData(pv);
}

/*static*/ JsiObjectRef const &JsiAbiRuntime::AsJsiObjectRef(PointerValue const *pv) noexcept {
  return ObjectPointerValue::GetData(pv);
}

/*static*/ JsiPropertyIdRef const &JsiAbiRuntime::AsJsiPropertyIdRef(PointerValue const *pv) noexcept {
  return PropNameIDPointerValue::GetData(pv);
}

/*static*/ JsiSymbolRef const &JsiAbiRuntime::AsJsiSymbolRef(Symbol const &symbol) noexcept {
  return SymbolPointerValue::GetData(getPointerValue(symbol));
}

/*static*/ JsiStringRef const &JsiAbiRuntime::AsJsiStringRef(String const &str) noexcept {
  return StringPointerValue::GetData(getPointerValue(str));
}

/*static*/ JsiObjectRef const &JsiAbiRuntime::AsJsiObjectRef(Object const &obj) noexcept {
  return ObjectPointerValue::GetData(getPointerValue(obj));
}

/*static*/ JsiPropertyIdRef const &JsiAbiRuntime::AsJsiPropertyIdRef(PropNameID const &propertyId) noexcept {
  return PropNameIDPointerValue::GetData(getPointerValue(propertyId));
}

/*static*/ JsiWeakObjectRef const &JsiAbiRuntime::AsJsiWeakObjectRef(WeakObject const &weakObject) noexcept {
  return reinterpret_cast<JsiWeakObjectRef const &>(ObjectPointerValue::GetData(getPointerValue(weakObject)));
}

/*static*/ JsiValueRef JsiAbiRuntime::AsJsiValueRef(Value const &value) noexcept {
  // We assume that the JsiValueRef and Value have the same layout.
  auto valuePtr = reinterpret_cast<JsiValueRef const *>(&value);
  // Fix up the data part
  switch (valuePtr->Kind) {
    case JsiValueKind::Symbol:
      return {valuePtr->Kind, SymbolPointerValue::GetData(getPointerValue(value)).Data};
    case JsiValueKind::String:
      return {valuePtr->Kind, StringPointerValue::GetData(getPointerValue(value)).Data};
    case JsiValueKind::Object:
      return {valuePtr->Kind, ObjectPointerValue::GetData(getPointerValue(value)).Data};
    default: // return a copy for other cases.
      return *valuePtr;
  }
}

/*static*/ JsiPropertyIdRef JsiAbiRuntime::DetachJsiPropertyIdRef(PropNameID &&propertyId) noexcept {
  // This method detaches JsiPropertyIdRef from the PropNameID.
  // It lets the PropNameIDPointerValue destructor run, but it must not destroy the underlying JS engine object.
  return PropNameIDPointerValue::Detach(getPointerValue(propertyId));
}

/*static*/ JsiValueRef JsiAbiRuntime::DetachJsiValueRef(facebook::jsi::Value &&value) noexcept {
  // Here we should move data from Value to JsiValueRef.
  // For Pointer types it means that we should allow running the PointerValue
  // destructor, but it must not call the Release method to keep the underlying
  // data alive. Thus, we must detach the value.
  // We assume that the JsiValueRef and Value have the same layout.
  auto valuePtr = reinterpret_cast<JsiValueRef *>(&value);
  switch (valuePtr->Kind) {
    case JsiValueKind::Symbol:
      return {valuePtr->Kind, SymbolPointerValue::Detach(getPointerValue(value)).Data};
    case JsiValueKind::String:
      return {valuePtr->Kind, StringPointerValue::Detach(getPointerValue(value)).Data};
    case JsiValueKind::Object:
      return {valuePtr->Kind, ObjectPointerValue::Detach(getPointerValue(value)).Data};
    default: // return a copy for other cases.
      return *valuePtr;
  }
}

Runtime::PointerValue *JsiAbiRuntime::MakeSymbolValue(JsiSymbolRef &&symbol) const noexcept {
  return new SymbolPointerValue{make_weak(m_runtime), std::move(symbol)};
}

Runtime::PointerValue *JsiAbiRuntime::MakeStringValue(JsiStringRef &&str) const noexcept {
  return new StringPointerValue{make_weak(m_runtime), std::move(str)};
}

Runtime::PointerValue *JsiAbiRuntime::MakeObjectValue(JsiObjectRef &&obj) const noexcept {
  return new ObjectPointerValue{make_weak(m_runtime), std::move(obj)};
}

Runtime::PointerValue *JsiAbiRuntime::MakePropNameIDValue(JsiPropertyIdRef &&propertyId) const noexcept {
  return new PropNameIDPointerValue{make_weak(m_runtime), std::move(propertyId)};
}

Symbol JsiAbiRuntime::MakeSymbol(JsiSymbolRef &&symbol) const noexcept {
  return make<Symbol>(MakeSymbolValue(std::move(symbol)));
}

String JsiAbiRuntime::MakeString(JsiStringRef &&str) const noexcept {
  return make<String>(MakeStringValue(std::move(str)));
}

Object JsiAbiRuntime::MakeObject(JsiObjectRef &&obj) const noexcept {
  return make<Object>(MakeObjectValue(std::move(obj)));
}

PropNameID JsiAbiRuntime::MakePropNameID(JsiPropertyIdRef &&propertyId) const noexcept {
  return make<PropNameID>(MakePropNameIDValue(std::move(propertyId)));
}

Array JsiAbiRuntime::MakeArray(JsiObjectRef &&arr) noexcept {
  return MakeObject({arr.Data}).getArray(*this);
}

WeakObject JsiAbiRuntime::MakeWeakObject(JsiWeakObjectRef &&weakObject) const noexcept {
  return make<WeakObject>(MakeObjectValue({weakObject.Data}));
}

Function JsiAbiRuntime::MakeFunction(JsiObjectRef &&func) noexcept {
  return MakeObject({func.Data}).getFunction(*this);
}

Value JsiAbiRuntime::MakeValue(JsiValueRef &&value) const noexcept {
  switch (value.Kind) {
    case JsiValueKind::Undefined:
      return Value();
    case JsiValueKind::Null:
      return Value(nullptr);
    case JsiValueKind::Boolean:
      return Value(reinterpret_cast<Value *>(&value)->getBool());
    case JsiValueKind::Number:
      return Value(*reinterpret_cast<double *>(&value.Data));
    case JsiValueKind::Symbol:
      return Value(MakeSymbol(JsiSymbolRef{value.Data}));
    case JsiValueKind::String:
      return Value(MakeString(JsiStringRef{value.Data}));
    case JsiValueKind::Object:
      return Value(MakeObject(JsiObjectRef{value.Data}));
    default:
      VerifyElseCrashSz(false, "Unexpected JsiValueKind value");
  }
}

//===========================================================================
// JsiAbiRuntime::DataPointerValue implementation
//===========================================================================

JsiAbiRuntime::DataPointerValue::DataPointerValue(winrt::weak_ref<JsiRuntime> &&weakRuntime, uint64_t data) noexcept
    : m_data{data}, m_weakRuntime{std::move(weakRuntime)} {}

JsiAbiRuntime::DataPointerValue::DataPointerValue(uint64_t data) noexcept : m_data{data} {}

void JsiAbiRuntime::DataPointerValue::invalidate() {}

//===========================================================================
// JsiAbiRuntime::SymbolPointerValue implementation
//===========================================================================

JsiAbiRuntime::SymbolPointerValue::SymbolPointerValue(
    winrt::weak_ref<JsiRuntime> &&weakRuntime,
    JsiSymbolRef &&symbol) noexcept
    : DataPointerValue{std::move(weakRuntime), std::exchange(symbol.Data, 0)} {}

void JsiAbiRuntime::SymbolPointerValue::invalidate() {
  if (m_data) {
    if (auto runtime = m_weakRuntime.get()) {
      m_weakRuntime = nullptr;
      runtime.ReleaseSymbol({m_data});
    }
  }
}

/*static*/ JsiSymbolRef const &JsiAbiRuntime::SymbolPointerValue::GetData(PointerValue const *pv) noexcept {
  return *reinterpret_cast<JsiSymbolRef const *>(&static_cast<DataPointerValue const *>(pv)->m_data);
}

/*static*/ JsiSymbolRef JsiAbiRuntime::SymbolPointerValue::Detach(PointerValue const *pv) noexcept {
  return {std::exchange(static_cast<DataPointerValue *>(const_cast<PointerValue *>(pv))->m_data, 0)};
}

//===========================================================================
// JsiAbiRuntime::StringPointerValue implementation
//===========================================================================

JsiAbiRuntime::StringPointerValue::StringPointerValue(
    winrt::weak_ref<JsiRuntime> &&weakRuntime,
    JsiStringRef &&str) noexcept
    : DataPointerValue{std::move(weakRuntime), std::exchange(str.Data, 0)} {}

void JsiAbiRuntime::StringPointerValue::invalidate() {
  if (m_data) {
    if (auto runtime = m_weakRuntime.get()) {
      m_weakRuntime = nullptr;
      runtime.ReleaseString({m_data});
    }
  }

  delete this;
}

/*static*/ JsiStringRef const &JsiAbiRuntime::StringPointerValue::GetData(PointerValue const *pv) noexcept {
  return *reinterpret_cast<JsiStringRef const *>(&static_cast<DataPointerValue const *>(pv)->m_data);
}

/*static*/ JsiStringRef JsiAbiRuntime::StringPointerValue::Detach(PointerValue const *pv) noexcept {
  return {std::exchange(static_cast<DataPointerValue *>(const_cast<PointerValue *>(pv))->m_data, 0)};
}

//===========================================================================
// JsiAbiRuntime::ObjectPointerValue implementation
//===========================================================================

JsiAbiRuntime::ObjectPointerValue::ObjectPointerValue(
    winrt::weak_ref<JsiRuntime> &&weakRuntime,
    JsiObjectRef &&obj) noexcept
    : DataPointerValue{std::move(weakRuntime), std::exchange(obj.Data, 0)} {}

void JsiAbiRuntime::ObjectPointerValue::invalidate() {
  if (m_data) {
    if (auto runtime = m_weakRuntime.get()) {
      m_weakRuntime = nullptr;
      runtime.ReleaseObject({m_data});
    }
  }

  delete this;
}

/*static*/ JsiObjectRef const &JsiAbiRuntime::ObjectPointerValue::GetData(PointerValue const *pv) noexcept {
  return *reinterpret_cast<JsiObjectRef const *>(&static_cast<DataPointerValue const *>(pv)->m_data);
}

/*static*/ JsiObjectRef JsiAbiRuntime::ObjectPointerValue::Detach(PointerValue const *pv) noexcept {
  return {std::exchange(static_cast<DataPointerValue *>(const_cast<PointerValue *>(pv))->m_data, 0)};
}

//===========================================================================
// JsiAbiRuntime::PropNameIDPointerValue implementation
//===========================================================================

JsiAbiRuntime::PropNameIDPointerValue::PropNameIDPointerValue(
    winrt::weak_ref<JsiRuntime> &&weakRuntime,
    JsiPropertyIdRef &&propertyId) noexcept
    : DataPointerValue{std::move(weakRuntime), std::exchange(propertyId.Data, 0)} {}

void JsiAbiRuntime::PropNameIDPointerValue::invalidate() {
  if (m_data) {
    if (auto runtime = m_weakRuntime.get()) {
      m_weakRuntime = nullptr;
      runtime.ReleasePropertyId({m_data});
    }
  }

  delete this;
}

/*static*/ JsiPropertyIdRef const &JsiAbiRuntime::PropNameIDPointerValue::GetData(PointerValue const *pv) noexcept {
  return *reinterpret_cast<JsiPropertyIdRef const *>(&static_cast<DataPointerValue const *>(pv)->m_data);
}

/*static*/ JsiPropertyIdRef JsiAbiRuntime::PropNameIDPointerValue::Detach(PointerValue const *pv) noexcept {
  return {std::exchange(static_cast<DataPointerValue *>(const_cast<PointerValue *>(pv))->m_data, 0)};
}

//===========================================================================
// JsiAbiRuntime::ValueRef implementation
//===========================================================================

JsiAbiRuntime::ValueRef::ValueRef(JsiValueRef const &data) noexcept {
  InitValueRef(data, &m_value, std::addressof(m_pointerStore));
}

JsiAbiRuntime::ValueRef::~ValueRef() noexcept {}

JsiAbiRuntime::ValueRef::operator Value const &() const noexcept {
  return m_value;
}

/*static*/ void
JsiAbiRuntime::ValueRef::InitValueRef(JsiValueRef const &data, Value *value, StoreType *store) noexcept {
  // We assume that the JsiValueRef and Value have the same layout.
  auto valueAsDataPtr = reinterpret_cast<JsiValueRef *>(value);
  valueAsDataPtr->Kind = data.Kind;
  switch (valueAsDataPtr->Kind) {
    case JsiValueKind::Symbol:
    case JsiValueKind::String:
    case JsiValueKind::Object:
      // Do in-place initialization in 'store'
      valueAsDataPtr->Data = reinterpret_cast<uint64_t>(new (store) DataPointerValue(data.Data));
      break;
    default:
      valueAsDataPtr->Data = data.Data;
      break;
  }
}

//===========================================================================
// JsiAbiRuntime::ValueRefArray implementation
//===========================================================================

JsiAbiRuntime::ValueRefArray::ValueRefArray(array_view<JsiValueRef const> args) noexcept : m_size{args.size()} {
  VerifyElseCrashSz(m_size <= MaxCallArgCount, "Argument count must not exceed the MaxCallArgCount");
  for (uint32_t i = 0; i < args.size(); ++i) {
    ValueRef::InitValueRef(args[i], &m_valueArray[i], std::addressof(m_pointerStoreArray[i]));
  }
}

facebook::jsi::Value const *JsiAbiRuntime::ValueRefArray::Data() const noexcept {
  return m_valueArray.data();
}

size_t JsiAbiRuntime::ValueRefArray::Size() const noexcept {
  return m_size;
}

//===========================================================================
// JsiAbiRuntime::PropNameIDRef implementation
//===========================================================================

JsiAbiRuntime::PropNameIDRef::PropNameIDRef(JsiPropertyIdRef const &data) noexcept
    : m_propertyId{make<PropNameID>(new (std::addressof(m_pointerStore)) DataPointerValue(data.Data))} {}

JsiAbiRuntime::PropNameIDRef::~PropNameIDRef() noexcept {}

JsiAbiRuntime::PropNameIDRef::operator facebook::jsi::PropNameID const &() const noexcept {
  return m_propertyId;
}

} // namespace winrt::Microsoft::ReactNative
