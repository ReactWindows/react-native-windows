// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "ChakraRuntime.h"
#include "ChakraRuntimeFactory.h"

#include <MemoryTracker.h>
#include <RuntimeOptions.h>
#include "Unicode.h"
#include "Utilities.h"

#include <cxxreact/MessageQueueThread.h>

#include <cstring>
#include <limits>
#include <mutex>
#include <sstream>
#include <unordered_set>

#ifdef CHAKRACORE
#include <ChakraCore.h>
#else
#ifndef USE_EDGEMODE_JSRT
#define USE_EDGEMODE_JSRT
#endif
#include <jsrt.h>
#endif

namespace Microsoft::JSI {

namespace {

struct HostFunctionWrapper final {
  HostFunctionWrapper(facebook::jsi::HostFunctionType &&hostFunction, ChakraRuntime &runtime)
      : m_hostFunction(std::move(hostFunction)), m_runtime(runtime) {}

  facebook::jsi::HostFunctionType &GetHostFunction() {
    return m_hostFunction;
  }

  ChakraRuntime &GetRuntime() {
    return m_runtime;
  }

 private:
  facebook::jsi::HostFunctionType m_hostFunction;
  ChakraRuntime &m_runtime;
};

} // namespace

// ES6 Promise callback
void CALLBACK ChakraRuntime::PromiseContinuationCallback(JsValueRef funcRef, void *callbackState) noexcept {
  ChakraRuntime *runtime = static_cast<ChakraRuntime *>(callbackState);
  runtime->PromiseContinuation(funcRef);
}

ChakraRuntime::ChakraRuntime(ChakraRuntimeArgs &&args) noexcept : m_args{std::move(args)} {}

void ChakraRuntime::Init() noexcept {
  JsRuntimeAttributes runtimeAttributes = JsRuntimeAttributeNone;

  if (!m_args.enableJITCompilation) {
    runtimeAttributes = static_cast<JsRuntimeAttributes>(
        runtimeAttributes | JsRuntimeAttributeDisableNativeCodeGeneration |
        JsRuntimeAttributeDisableExecutablePageAllocation);
  }

  m_runtime = CreateRuntime(runtimeAttributes, nullptr);

  setupMemoryTracker();

  m_context = JsRefHolder{CreateContext(m_runtime)};

  // Note :: We currently assume that the runtime will be created and
  // exclusively used in a single thread.
  // Preserve the current context if it is already associated with the thread.
  m_prevContext = JsRefHolder{GetCurrentContext()};
  SetCurrentContext(m_context);

  startDebuggingIfNeeded();

  setupNativePromiseContinuation();

  std::call_once(s_runtimeVersionInitFlag, initRuntimeVersion);

  m_propertyId.Object = JsRefHolder{GetPropertyIdFromName(L"Object")};
  m_propertyId.Proxy = JsRefHolder{GetPropertyIdFromName(L"Proxy")};
  m_propertyId.Symbol = JsRefHolder{GetPropertyIdFromName(L"Symbol")};
  m_propertyId.byteLength = JsRefHolder{GetPropertyIdFromName(L"byteLength")};
  m_propertyId.configurable = JsRefHolder{GetPropertyIdFromName(L"configurable")};
  m_propertyId.enumerable = JsRefHolder{GetPropertyIdFromName(L"enumerable")};
  m_propertyId.get = JsRefHolder{GetPropertyIdFromName(L"get")};
  m_propertyId.getOwnPropertyDescriptor = JsRefHolder{GetPropertyIdFromName(L"getOwnPropertyDescriptor")};
  m_propertyId.hostFunctionSymbol = JsRefHolder{GetPropertyIdFromSymbol(L"hostFunctionSymbol")};
  m_propertyId.hostObjectSymbol = JsRefHolder{GetPropertyIdFromSymbol(L"hostObjectSymbol")};
  m_propertyId.length = JsRefHolder{GetPropertyIdFromName(L"length")};
  m_propertyId.message = JsRefHolder{GetPropertyIdFromName(L"message")};
  m_propertyId.ownKeys = JsRefHolder{GetPropertyIdFromName(L"ownKeys")};
  m_propertyId.propertyIsEnumerable = JsRefHolder{GetPropertyIdFromName(L"propertyIsEnumerable")};
  m_propertyId.prototype = JsRefHolder{GetPropertyIdFromName(L"prototype")};
  m_propertyId.set = JsRefHolder{GetPropertyIdFromName(L"set")};
  m_propertyId.toString = JsRefHolder{GetPropertyIdFromName(L"toString")};
  m_propertyId.value = JsRefHolder{GetPropertyIdFromName(L"value")};
  m_propertyId.writable = JsRefHolder{GetPropertyIdFromName(L"writable")};

  m_undefinedValue = JsRefHolder{GetUndefinedValue()};
}

/*virtual*/ ChakraRuntime::~ChakraRuntime() noexcept {
  m_undefinedValue = {};
  m_propertyId = {};
  m_proxyConstructor = {};
  m_hostObjectProxyHandler = {};

  m_context = {};
  SetCurrentContext(m_prevContext);
  m_prevContext = {};

  DisposeRuntime(m_runtime);
}

void ChakraRuntime::PromiseContinuation(JsValueRef funcRef) noexcept {
  if (runtimeArgs().jsQueue) {
    JsAddRef(funcRef, nullptr);
    runtimeArgs().jsQueue->runOnQueue([this, funcRef]() {
      JsValueRef undefinedValue;
      JsGetUndefinedValue(&undefinedValue);
      ChakraVerifyJsErrorElseThrow(JsCallFunction(funcRef, &undefinedValue, 1, nullptr));
      JsRelease(funcRef, nullptr);
    });
  }
}

JsValueRef ChakraRuntime::CreatePropertyDescriptor(JsValueRef value, PropertyAttibutes attrs) {
  JsValueRef descriptor = CreateObject();
  SetProperty(descriptor, m_propertyId.value, value);
  if (!(attrs & PropertyAttibutes::ReadOnly)) {
    SetProperty(descriptor, m_propertyId.writable, BoolToBoolean(true));
  }
  if (!(attrs & PropertyAttibutes::DontEnum)) {
    SetProperty(descriptor, m_propertyId.enumerable, BoolToBoolean(true));
  }
  if (!(attrs & PropertyAttibutes::DontDelete)) {
    // The JavaScript 'configurable=true' allows property to be deleted.
    SetProperty(descriptor, m_propertyId.configurable, BoolToBoolean(true));
  }
  return descriptor;
}

#pragma region Functions_inherited_from_Runtime

facebook::jsi::Value ChakraRuntime::evaluateJavaScript(
    const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
    const std::string &sourceURL) {
  // Simple evaluate if scriptStore not available as it's risky to utilize the
  // byte codes without checking the script version.
  if (!runtimeArgs().scriptStore) {
    if (!buffer)
      throw facebook::jsi::JSINativeException("Script buffer is empty!");
    return evaluateJavaScriptSimple(*buffer, sourceURL);
  }

  uint64_t scriptVersion = 0;
  std::shared_ptr<const facebook::jsi::Buffer> scriptBuffer;

  if (buffer) {
    scriptBuffer = buffer;
    scriptVersion = runtimeArgs().scriptStore->getScriptVersion(sourceURL);
  } else {
    auto versionedScript = runtimeArgs().scriptStore->getVersionedScript(sourceURL);
    scriptBuffer = std::move(versionedScript.buffer);
    scriptVersion = versionedScript.version;
  }

  if (!scriptBuffer) {
    throw facebook::jsi::JSINativeException("Script buffer is empty!");
  }

  // Simple evaluate if script version can't be computed.
  if (scriptVersion == 0) {
    return evaluateJavaScriptSimple(*scriptBuffer, sourceURL);
  }

  auto sharedScriptBuffer = std::shared_ptr<const facebook::jsi::Buffer>(std::move(scriptBuffer));

  facebook::jsi::ScriptSignature scriptSignature = {sourceURL, scriptVersion};
  facebook::jsi::JSRuntimeSignature runtimeSignature = {description().c_str(), getRuntimeVersion()};

  auto preparedScript =
      runtimeArgs().preparedScriptStore->tryGetPreparedScript(scriptSignature, runtimeSignature, nullptr);

  std::shared_ptr<const facebook::jsi::Buffer> sharedPreparedScript;
  if (preparedScript) {
    sharedPreparedScript = std::shared_ptr<const facebook::jsi::Buffer>(std::move(preparedScript));
  } else {
    auto genPreparedScript = generatePreparedScript(sourceURL, *sharedScriptBuffer);
    if (!genPreparedScript)
      std::terminate(); // Cache generation can't fail unless something really
                        // wrong. but we should get rid of this abort before
                        // shipping.

    sharedPreparedScript = std::shared_ptr<const facebook::jsi::Buffer>(std::move(genPreparedScript));
    runtimeArgs().preparedScriptStore->persistPreparedScript(
        sharedPreparedScript, scriptSignature, runtimeSignature, nullptr);
  }

  // We are pinning the buffers which are backing the external array buffers to
  // the duration of this. This is not good if the external array buffers have a
  // reduced lifetime compared to the runtime itself. But, it's ok for the script
  // and prepared script buffer as their lifetime is expected to be same as the
  // JSI runtime.
  m_pinnedPreparedScripts.push_back(sharedPreparedScript);
  m_pinnedScripts.push_back(sharedScriptBuffer);

  JsValueRef result;
  if (evaluateSerializedScript(*sharedScriptBuffer, *sharedPreparedScript, sourceURL, &result)) {
    return ToJsiValue(result);
  }

  // If we reach here, fall back to simple evaluation.
  return evaluateJavaScriptSimple(*sharedScriptBuffer, sourceURL);
}

struct ChakraPreparedJavaScript final : facebook::jsi::PreparedJavaScript {
  ChakraPreparedJavaScript(
      std::string sourceUrl,
      const std::shared_ptr<const facebook::jsi::Buffer> &sourceBuffer,
      std::unique_ptr<const facebook::jsi::Buffer> byteCode)
      : m_sourceUrl{std::move(sourceUrl)}, m_sourceBuffer{sourceBuffer}, m_byteCode{std::move(byteCode)} {}

  const std::string &SourceUrl() const {
    return m_sourceUrl;
  }

  const facebook::jsi::Buffer &SourceBuffer() const {
    return *m_sourceBuffer;
  }

  const facebook::jsi::Buffer &ByteCode() const {
    return *m_byteCode;
  }

 private:
  std::string m_sourceUrl;
  std::shared_ptr<const facebook::jsi::Buffer> m_sourceBuffer;
  std::unique_ptr<const facebook::jsi::Buffer> m_byteCode;
};

std::shared_ptr<const facebook::jsi::PreparedJavaScript> ChakraRuntime::prepareJavaScript(
    const std::shared_ptr<const facebook::jsi::Buffer> &sourceBuffer,
    std::string sourceURL) {
  return std::make_shared<ChakraPreparedJavaScript>(
      sourceURL, sourceBuffer, generatePreparedScript(sourceURL, *sourceBuffer));
}

facebook::jsi::Value ChakraRuntime::evaluatePreparedJavaScript(
    const std::shared_ptr<const facebook::jsi::PreparedJavaScript> &preparedJS) {
  const ChakraPreparedJavaScript &chakraPreparedJS = *static_cast<const ChakraPreparedJavaScript *>(preparedJS.get());
  JsValueRef result;
  if (evaluateSerializedScript(
          chakraPreparedJS.SourceBuffer(), chakraPreparedJS.ByteCode(), chakraPreparedJS.SourceUrl(), &result)) {
    return ToJsiValue(result);
  } else {
    return facebook::jsi::Value::undefined();
  }
}

bool ChakraRuntime::drainMicrotasks(int /*maxMicrotasksHint*/) {
  // Not implemented
  return true;
}

facebook::jsi::Object ChakraRuntime::global() {
  return MakePointer<facebook::jsi::Object>(GetGlobalObject());
}

std::string ChakraRuntime::description() {
  return "ChakraRuntime";
}

bool ChakraRuntime::isInspectable() {
  return false;
}

facebook::jsi::Runtime::PointerValue *ChakraRuntime::cloneSymbol(
    const facebook::jsi::Runtime::PointerValue *pointerValue) {
  return CloneChakraPointerValue(pointerValue);
}

facebook::jsi::Runtime::PointerValue *ChakraRuntime::cloneString(
    const facebook::jsi::Runtime::PointerValue *pointerValue) {
  return CloneChakraPointerValue(pointerValue);
}

facebook::jsi::Runtime::PointerValue *ChakraRuntime::cloneObject(
    const facebook::jsi::Runtime::PointerValue *pointerValue) {
  return CloneChakraPointerValue(pointerValue);
}

facebook::jsi::Runtime::PointerValue *ChakraRuntime::clonePropNameID(
    const facebook::jsi::Runtime::PointerValue *pointerValue) {
  return CloneChakraPointerValue(pointerValue);
}

facebook::jsi::PropNameID ChakraRuntime::createPropNameIDFromAscii(const char *str, size_t length) {
  const JsPropertyIdRef propertyId = GetPropertyIdFromName(std::string_view{str, length});
  return MakePointer<facebook::jsi::PropNameID>(propertyId);
}

facebook::jsi::PropNameID ChakraRuntime::createPropNameIDFromUtf8(const uint8_t *utf8, size_t length) {
  const JsPropertyIdRef propertyId =
      GetPropertyIdFromName(std::string_view{reinterpret_cast<const char *>(utf8), length});
  return MakePointer<facebook::jsi::PropNameID>(propertyId);
}

facebook::jsi::PropNameID ChakraRuntime::createPropNameIDFromString(const facebook::jsi::String &str) {
  const JsPropertyIdRef propertyId = GetPropertyIdFromName(StringToPointer(GetJsRef(str)).data());
  return MakePointer<facebook::jsi::PropNameID>(propertyId);
}

std::string ChakraRuntime::utf8(const facebook::jsi::PropNameID &id) {
  return Common::Unicode::Utf16ToUtf8(GetPropertyNameFromId(GetJsRef(id)));
}

bool ChakraRuntime::compare(const facebook::jsi::PropNameID &lhs, const facebook::jsi::PropNameID &rhs) {
  return GetJsRef(lhs) == GetJsRef(rhs);
}

std::string ChakraRuntime::symbolToString(const facebook::jsi::Symbol &s) {
  const JsValueRef symbol = GetJsRef(s);
  const JsValueRef symbolCtor = GetProperty(GetGlobalObject(), m_propertyId.Symbol);
  const JsValueRef symbolPrototype = GetProperty(symbolCtor, m_propertyId.prototype);
  const JsValueRef symbolToString = GetProperty(symbolPrototype, m_propertyId.toString);
  const JsValueRef jsString = CallFunction(symbolToString, {symbol});
  return StringToStdString(jsString);
}

facebook::jsi::String ChakraRuntime::createStringFromAscii(const char *str, size_t length) {
  return MakePointer<facebook::jsi::String>(PointerToString({str, length}));
}

facebook::jsi::String ChakraRuntime::createStringFromUtf8(const uint8_t *str, size_t length) {
  return MakePointer<facebook::jsi::String>(PointerToString({reinterpret_cast<char const *>(str), length}));
}

std::string ChakraRuntime::utf8(const facebook::jsi::String &str) {
  return StringToStdString(GetJsRef(str));
}

facebook::jsi::Object ChakraRuntime::createObject() {
  return MakePointer<facebook::jsi::Object>(CreateObject());
}

facebook::jsi::Object ChakraRuntime::createObject(std::shared_ptr<facebook::jsi::HostObject> hostObject) {
  // The hostObjectHolder keeps the hostObject as external data.
  // Then the hostObjectHolder is wrapped up by a Proxy object to provide access to hostObject's
  // get, set, and getPropertyNames methods.
  // There is a special symbol property ID 'hostObjectSymbol' to access the hostObjectWrapper from the Proxy.
  JsValueRef hostObjectHolder =
      CreateExternalObject(std::make_unique<std::shared_ptr<facebook::jsi::HostObject>>(std::move(hostObject)));
  if (!m_proxyConstructor) {
    m_proxyConstructor = JsRefHolder{GetProperty(GetGlobalObject(), m_propertyId.Proxy)};
  }
  JsValueRef proxy =
      ConstructObject(m_proxyConstructor, {m_undefinedValue, hostObjectHolder, GetHostObjectProxyHandler()});
  return MakePointer<facebook::jsi::Object>(proxy);
}

std::shared_ptr<facebook::jsi::HostObject> ChakraRuntime::getHostObject(const facebook::jsi::Object &obj) {
  JsValueRef hostObjectHolder = GetProperty(GetJsRef(obj), m_propertyId.hostObjectSymbol);
  if (GetValueType(hostObjectHolder) == JsValueType::JsObject) {
    return *static_cast<std::shared_ptr<facebook::jsi::HostObject> *>(GetExternalData(hostObjectHolder));
  } else {
    throw facebook::jsi::JSINativeException("getHostObject() can only be called with HostObjects.");
  }
}

facebook::jsi::HostFunctionType &ChakraRuntime::getHostFunction(const facebook::jsi::Function &func) {
  JsValueRef hostFunctionHolder = GetProperty(GetJsRef(func), m_propertyId.hostFunctionSymbol);
  if (GetValueType(hostFunctionHolder) == JsValueType::JsObject) {
    return static_cast<HostFunctionWrapper *>(GetExternalData(hostFunctionHolder))->GetHostFunction();
  } else {
    throw facebook::jsi::JSINativeException("getHostFunction() can only be called with HostFunction.");
  }
}

facebook::jsi::Value ChakraRuntime::getProperty(
    const facebook::jsi::Object &obj,
    const facebook::jsi::PropNameID &name) {
  return ToJsiValue(GetProperty(GetJsRef(obj), GetJsRef(name)));
}

facebook::jsi::Value ChakraRuntime::getProperty(const facebook::jsi::Object &obj, const facebook::jsi::String &name) {
  return ToJsiValue(GetProperty(GetJsRef(obj), GetPropertyIdFromString(GetJsRef(name))));
}

bool ChakraRuntime::hasProperty(const facebook::jsi::Object &obj, const facebook::jsi::PropNameID &name) {
  return HasProperty(GetJsRef(obj), GetJsRef(name));
}

bool ChakraRuntime::hasProperty(const facebook::jsi::Object &obj, const facebook::jsi::String &name) {
  return HasProperty(GetJsRef(obj), GetPropertyIdFromString(GetJsRef(name)));
}

void ChakraRuntime::setPropertyValue(
    facebook::jsi::Object &object,
    const facebook::jsi::PropNameID &name,
    const facebook::jsi::Value &value) {
  SetProperty(GetJsRef(object), GetJsRef(name), ToJsValueRef(value));
}

void ChakraRuntime::setPropertyValue(
    facebook::jsi::Object &object,
    const facebook::jsi::String &name,
    const facebook::jsi::Value &value) {
  SetProperty(GetJsRef(object), GetPropertyIdFromString(GetJsRef(name)), ToJsValueRef(value));
}

bool ChakraRuntime::isArray(const facebook::jsi::Object &obj) const {
  return GetValueType(GetJsRef(obj)) == JsArray;
}

bool ChakraRuntime::isArrayBuffer(const facebook::jsi::Object &obj) const {
  return GetValueType(GetJsRef(obj)) == JsArrayBuffer;
}

bool ChakraRuntime::isFunction(const facebook::jsi::Object &obj) const {
  return GetValueType(GetJsRef(obj)) == JsFunction;
}

bool ChakraRuntime::isHostObject(const facebook::jsi::Object &obj) const {
  JsValueRef hostObjectHolder = GetProperty(GetJsRef(obj), m_propertyId.hostObjectSymbol);
  if (GetValueType(hostObjectHolder) == JsValueType::JsObject) {
    return GetExternalData(hostObjectHolder) != nullptr;
  } else {
    return false;
  }
}

bool ChakraRuntime::isHostFunction(const facebook::jsi::Function &func) const {
  JsValueRef hostFunctionHolder = GetProperty(GetJsRef(func), m_propertyId.hostFunctionSymbol);
  if (GetValueType(hostFunctionHolder) == JsValueType::JsObject) {
    return GetExternalData(hostFunctionHolder) != nullptr;
  } else {
    return false;
  }
}

facebook::jsi::Array ChakraRuntime::getPropertyNames(const facebook::jsi::Object &object) {
  // Handle to the null JS value.
  JsValueRef jsNull = GetNullValue();

  // Handle to the Object constructor.
  JsValueRef objectConstructor = GetProperty(GetGlobalObject(), m_propertyId.Object);

  // Handle to the Object.prototype Object.
  JsValueRef objectPrototype = GetProperty(objectConstructor, m_propertyId.prototype);

  // Handle to the Object.prototype.propertyIsEnumerable() Function.
  JsValueRef objectPrototypePropertyIsEnumerable = GetProperty(objectPrototype, m_propertyId.propertyIsEnumerable);

  // We now traverse the object's property chain and collect all enumerable property names.
  std::vector<JsValueRef> enumerablePropNames{};
  JsValueRef currentObjectOnPrototypeChain = GetJsRef(object);

  // We have a small optimization here where we stop traversing the prototype
  // chain as soon as we hit Object.prototype. However, we still need to check
  // for null here, as one can create an Object with no prototype through
  // Object.create(null).
  while (!StrictEquals(currentObjectOnPrototypeChain, objectPrototype) &&
         !StrictEquals(currentObjectOnPrototypeChain, jsNull)) {
    JsValueRef propNames = GetOwnPropertyNames(currentObjectOnPrototypeChain);
    int propNamesSize = NumberToInt(GetProperty(propNames, m_propertyId.length));

    enumerablePropNames.reserve(enumerablePropNames.size() + propNamesSize);
    for (int i = 0; i < propNamesSize; ++i) {
      JsValueRef propName = GetIndexedProperty(propNames, i);
      bool propIsEnumerable =
          BooleanToBool(CallFunction(objectPrototypePropertyIsEnumerable, {currentObjectOnPrototypeChain, propName}));
      if (propIsEnumerable) {
        enumerablePropNames.push_back(propName);
      }
    }

    currentObjectOnPrototypeChain = GetPrototype(currentObjectOnPrototypeChain);
  }

  size_t enumerablePropNamesSize = enumerablePropNames.size();
  JsValueRef result = CreateArray(enumerablePropNamesSize);

  for (size_t i = 0; i < enumerablePropNamesSize; ++i) {
    SetIndexedProperty(result, static_cast<int32_t>(i), enumerablePropNames[i]);
  }

  return MakePointer<facebook::jsi::Object>(result).getArray(*this);
}

// Only ChakraCore supports weak reference semantics, so ChakraRuntime
// WeakObjects are in fact strong references.

facebook::jsi::WeakObject ChakraRuntime::createWeakObject(const facebook::jsi::Object &object) {
  return make<facebook::jsi::WeakObject>(CloneChakraPointerValue(getPointerValue(object)));
}

facebook::jsi::Value ChakraRuntime::lockWeakObject(facebook::jsi::WeakObject &weakObject) {
  // We need to make a copy of the ChakraObjectRef held within weakObj's
  // member PointerValue for the returned jsi::Value here.
  return ToJsiValue(GetJsRef(weakObject));
}

facebook::jsi::Array ChakraRuntime::createArray(size_t length) {
  assert(length <= (std::numeric_limits<unsigned int>::max)());

  return MakePointer<facebook::jsi::Object>(CreateArray(length)).asArray(*this);
}

size_t ChakraRuntime::size(const facebook::jsi::Array &arr) {
  assert(isArray(arr));

  const int result = NumberToInt(GetProperty(GetJsRef(arr), m_propertyId.length));
  ChakraVerifyElseThrow(result >= 0, "Invalid JS array length detected.");
  return static_cast<size_t>(result);
}

size_t ChakraRuntime::size(const facebook::jsi::ArrayBuffer &arrBuf) {
  assert(isArrayBuffer(arrBuf));

  const int result = NumberToInt(GetProperty(GetJsRef(arrBuf), m_propertyId.byteLength));
  ChakraVerifyElseThrow(result >= 0, "Invalid JS array buffer byteLength detected.");
  return static_cast<size_t>(result);
}

uint8_t *ChakraRuntime::data(const facebook::jsi::ArrayBuffer &arrBuf) {
  assert(isArrayBuffer(arrBuf));
  return reinterpret_cast<uint8_t *>(GetArrayBufferStorage(GetJsRef(arrBuf)).begin());
}

facebook::jsi::Value ChakraRuntime::getValueAtIndex(const facebook::jsi::Array &arr, size_t index) {
  assert(isArray(arr));
  assert(index <= static_cast<size_t>((std::numeric_limits<int>::max)()));

  return ToJsiValue(GetIndexedProperty(GetJsRef(arr), static_cast<int>(index)));
}

void ChakraRuntime::setValueAtIndexImpl(facebook::jsi::Array &arr, size_t index, const facebook::jsi::Value &value) {
  assert(isArray(arr));
  assert(index <= static_cast<size_t>((std::numeric_limits<int>::max)()));

  SetIndexedProperty(GetJsRef(arr), static_cast<int>(index), ToJsValueRef(value));
}

facebook::jsi::Function ChakraRuntime::createFunctionFromHostFunction(
    const facebook::jsi::PropNameID &name,
    unsigned int paramCount,
    facebook::jsi::HostFunctionType func) {
  auto hostFunctionWrapper = std::make_unique<HostFunctionWrapper>(std::move(func), *this);
  JsValueRef function = CreateExternalFunction(
      GetJsRef(name), static_cast<int32_t>(paramCount), HostFunctionCall, hostFunctionWrapper.get());

  const auto hostFunctionHolder = CreateExternalObject(std::move(hostFunctionWrapper));
  DefineProperty(
      function,
      m_propertyId.hostFunctionSymbol,
      CreatePropertyDescriptor(hostFunctionHolder, PropertyAttibutes::DontEnumAndFrozen));

  return MakePointer<facebook::jsi::Object>(function).getFunction(*this);
}

facebook::jsi::Value ChakraRuntime::call(
    const facebook::jsi::Function &func,
    const facebook::jsi::Value &jsThis,
    const facebook::jsi::Value *args,
    size_t count) {
  return ToJsiValue(CallFunction(GetJsRef(func), JsValueArgs(*this, jsThis, Span(args, count))));
}

facebook::jsi::Value
ChakraRuntime::callAsConstructor(const facebook::jsi::Function &func, const facebook::jsi::Value *args, size_t count) {
  return ToJsiValue(
      ConstructObject(GetJsRef(func), JsValueArgs(*this, facebook::jsi::Value::undefined(), Span(args, count))));
}

facebook::jsi::Runtime::ScopeState *ChakraRuntime::pushScope() {
  return nullptr;
}

void ChakraRuntime::popScope([[maybe_unused]] Runtime::ScopeState *state) {
  assert(state == nullptr);
  ChakraVerifyJsErrorElseThrow(JsCollectGarbage(m_runtime));
}

bool ChakraRuntime::strictEquals(const facebook::jsi::Symbol &a, const facebook::jsi::Symbol &b) const {
  return StrictEquals(GetJsRef(a), GetJsRef(b));
}

bool ChakraRuntime::strictEquals(const facebook::jsi::String &a, const facebook::jsi::String &b) const {
  return StrictEquals(GetJsRef(a), GetJsRef(b));
}

bool ChakraRuntime::strictEquals(const facebook::jsi::Object &a, const facebook::jsi::Object &b) const {
  return StrictEquals(GetJsRef(a), GetJsRef(b));
}

bool ChakraRuntime::instanceOf(const facebook::jsi::Object &obj, const facebook::jsi::Function &func) {
  return InstanceOf(GetJsRef(obj), GetJsRef(func));
}

#pragma endregion Functions_inherited_from_Runtime

// Sets variable in the constructor and then restores its value in the destructor.
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

[[noreturn]] void ChakraRuntime::ThrowJsExceptionOverride(JsErrorCode errorCode, JsValueRef jsError) {
  if (!m_pendingJSError && (errorCode == JsErrorScriptException || GetValueType(jsError) == JsError)) {
    AutoRestore<bool> setValue{const_cast<bool *>(&m_pendingJSError), true};
    RewriteErrorMessage(jsError);
    throw facebook::jsi::JSError(*this, ToJsiValue(jsError));
  } else {
    std::ostringstream errorString;
    errorString << "A call to Chakra API returned error code 0x" << std::hex << errorCode << '.';
    throw facebook::jsi::JSINativeException(errorString.str().c_str());
  }
}

[[noreturn]] void ChakraRuntime::ThrowNativeExceptionOverride(char const *errorMessage) {
  throw facebook::jsi::JSINativeException(errorMessage);
}

void ChakraRuntime::RewriteErrorMessage(JsValueRef jsError) {
  // The code below must work correctly even if the 'message' getter throws.
  // In case when it throws, we ignore that exception.
  JsValueRef message{JS_INVALID_REFERENCE};
  JsErrorCode errorCode = JsGetProperty(jsError, m_propertyId.message, &message);
  if (errorCode != JsNoError) {
    // If the 'message' property getter throws, then we clear the exception and ignore it.
    JsValueRef ignoreJSError{JS_INVALID_REFERENCE};
    JsGetAndClearException(&ignoreJSError);
  } else if (GetValueType(message) == JsValueType::JsString) {
    // JSI unit tests expect V8 or JSC like message for stack overflow.
    if (StringToPointer(message) == L"Out of stack space") {
      SetProperty(jsError, m_propertyId.message, PointerToString(L"RangeError : Maximum call stack size exceeded"));
    }
  }
}

facebook::jsi::Value ChakraRuntime::ToJsiValue(JsValueRef ref) {
  switch (GetValueType(ref)) {
    case JsUndefined:
      return facebook::jsi::Value::undefined();
    case JsNull:
      return facebook::jsi::Value::null();
    case JsNumber:
      return facebook::jsi::Value(NumberToDouble(ref));
    case JsString:
      return facebook::jsi::Value(*this, MakePointer<facebook::jsi::String>(ref));
    case JsBoolean:
      return facebook::jsi::Value(BooleanToBool(ref));
    case JsSymbol:
      return facebook::jsi::Value(*this, MakePointer<facebook::jsi::Symbol>(ref));
    case JsObject:
    case JsFunction:
    case JsError:
    case JsArray:
    case JsArrayBuffer:
    case JsTypedArray:
    case JsDataView:
      return facebook::jsi::Value(*this, MakePointer<facebook::jsi::Object>(ref));
    default:
      throw facebook::jsi::JSINativeException("Unexpected value type");
  }
}

JsValueRef ChakraRuntime::ToJsValueRef(const facebook::jsi::Value &value) {
  if (value.isUndefined()) {
    return m_undefinedValue;
  } else if (value.isNull()) {
    return GetNullValue();
  } else if (value.isBool()) {
    return BoolToBoolean(value.getBool());
  } else if (value.isNumber()) {
    return DoubleToNumber(value.getNumber());
  } else if (value.isSymbol()) {
    return GetJsRef(value.getSymbol(*this));
  } else if (value.isString()) {
    return GetJsRef(value.getString(*this));
  } else if (value.isObject()) {
    return GetJsRef(value.getObject(*this));
  } else {
    throw facebook::jsi::JSINativeException("Unexpected jsi::Value type");
  }
}

JsValueRef ChakraRuntime::CreateExternalFunction(
    JsPropertyIdRef name,
    int32_t paramCount,
    JsNativeFunction nativeFunction,
    void *callbackState) {
  JsValueRef nameString = GetPropertyStringFromId(name);
  JsValueRef function = CreateNamedFunction(nameString, nativeFunction, callbackState);
  DefineProperty(
      function,
      m_propertyId.length,
      CreatePropertyDescriptor(IntToNumber(paramCount), PropertyAttibutes::DontEnumAndFrozen));
  return function;
}

JsValueRef CALLBACK ChakraRuntime::HostFunctionCall(
    JsValueRef /*callee*/,
    bool isConstructCall,
    JsValueRef *args,
    unsigned short argCount,
    void *callbackState) {
  HostFunctionWrapper *hostFuncWraper = static_cast<HostFunctionWrapper *>(callbackState);
  ChakraRuntime &chakraRuntime = hostFuncWraper->GetRuntime();
  return chakraRuntime.HandleCallbackExceptions([&]() {
    ChakraVerifyElseThrow(argCount > 0, "There must be at least 'this' argument.");
    const JsiValueView jsiThisArg{*args};
    const JsiValueViewArgs jsiArgs{args + 1, argCount - 1u};

    const facebook::jsi::HostFunctionType &hostFunc = hostFuncWraper->GetHostFunction();
    return RunInMethodContext("HostFunction", [&]() {
      return chakraRuntime.ToJsValueRef(hostFunc(chakraRuntime, jsiThisArg, jsiArgs.Data(), jsiArgs.Size()));
    });
  });
}

/*static*/ JsValueRef CALLBACK ChakraRuntime::HostObjectGetTrap(
    JsValueRef /*callee*/,
    bool isConstructCall,
    JsValueRef *args,
    unsigned short argCount,
    void *callbackState) noexcept {
  ChakraRuntime *chakraRuntime = static_cast<ChakraRuntime *>(callbackState);
  return chakraRuntime->HandleCallbackExceptions([&]() {
    // args[0] - the Proxy handler object (this) (unused).
    // args[1] - the Proxy target object.
    // args[2] - the name of the property to set.
    // args[3] - the Proxy object (unused).
    ChakraVerifyElseThrow(argCount == 4, "HostObjectGetTrap() requires 4 arguments.");
    const JsValueRef target = args[1];
    const JsValueRef propertyName = args[2];
    if (GetValueType(propertyName) == JsValueType::JsString) {
      auto const &hostObject = *static_cast<std::shared_ptr<facebook::jsi::HostObject> *>(GetExternalData(target));
      const PropNameIDView propertyId{GetPropertyIdFromName(StringToPointer(propertyName).data())};
      return RunInMethodContext("HostObject::get", [&]() {
        return chakraRuntime->ToJsValueRef(hostObject->get(*chakraRuntime, propertyId));
      });
    } else if (GetValueType(propertyName) == JsValueType::JsSymbol) {
      const auto chakraPropertyId = GetPropertyIdFromSymbol(propertyName);
      if (chakraPropertyId == chakraRuntime->m_propertyId.hostObjectSymbol) {
        // The special property to retrieve the target object.
        return target;
      } else {
        auto const &hostObject = *static_cast<std::shared_ptr<facebook::jsi::HostObject> *>(GetExternalData(target));
        const PropNameIDView propertyId{chakraPropertyId};
        return RunInMethodContext("HostObject::get", [&]() {
          return chakraRuntime->ToJsValueRef(hostObject->get(*chakraRuntime, propertyId));
        });
      }
    }

    return static_cast<JsValueRef>(chakraRuntime->m_undefinedValue);
  });
} // namespace Microsoft::JSI

/*static*/ JsValueRef CALLBACK ChakraRuntime::HostObjectSetTrap(
    JsValueRef /*callee*/,
    bool isConstructCall,
    JsValueRef *args,
    unsigned short argCount,
    void *callbackState) noexcept {
  ChakraRuntime *chakraRuntime = static_cast<ChakraRuntime *>(callbackState);
  return chakraRuntime->HandleCallbackExceptions([&]() {
    // args[0] - the Proxy handler object (this) (unused).
    // args[1] - the Proxy target object.
    // args[2] - the name of the property to set.
    // args[3] - the new value of the property to set.
    // args[4] - the Proxy object (unused).
    ChakraVerifyElseThrow(argCount == 5, "HostObjectSetTrap() requires 5 arguments.");

    const JsValueRef target = args[1];
    const JsValueRef propertyName = args[2];
    if (GetValueType(propertyName) == JsValueType::JsString) {
      auto const &hostObject = *static_cast<std::shared_ptr<facebook::jsi::HostObject> *>(GetExternalData(target));
      const PropNameIDView propertyId{GetPropertyIdFromName(StringToPointer(propertyName).data())};
      const JsiValueView value{args[3]};
      RunInMethodContext("HostObject::set", [&]() { hostObject->set(*chakraRuntime, propertyId, value); });
    }

    return static_cast<JsValueRef>(chakraRuntime->m_undefinedValue);
  });
}

/*static*/ JsValueRef CALLBACK ChakraRuntime::HostObjectOwnKeysTrap(
    JsValueRef /*callee*/,
    bool isConstructCall,
    JsValueRef *args,
    unsigned short argCount,
    void *callbackState) noexcept {
  ChakraRuntime *chakraRuntime = static_cast<ChakraRuntime *>(callbackState);
  return chakraRuntime->HandleCallbackExceptions([&]() {
    ChakraVerifyElseThrow(!isConstructCall, "Constructor call for HostObjectOwnKeysTrap() is not supported.");

    // args[0] - the Proxy handler object (this) (unused).
    // args[1] - the Proxy target object.
    ChakraVerifyElseThrow(argCount == 2, "HostObjectOwnKeysTrap() requires 2 arguments.");
    const JsValueRef target = args[1];
    auto const &hostObject = *static_cast<std::shared_ptr<facebook::jsi::HostObject> *>(GetExternalData(target));

    const std::vector<facebook::jsi::PropNameID> ownKeys = RunInMethodContext(
        "HostObject::getPropertyNames", [&]() { return hostObject->getPropertyNames(*chakraRuntime); });

    std::unordered_set<JsPropertyIdRef> dedupedOwnKeys{};
    dedupedOwnKeys.reserve(ownKeys.size());
    for (facebook::jsi::PropNameID const &key : ownKeys) {
      dedupedOwnKeys.insert(GetJsRef(key));
    }

    const JsValueRef result = CreateArray(dedupedOwnKeys.size());
    int32_t index = 0;
    for (JsPropertyIdRef key : dedupedOwnKeys) {
      SetIndexedProperty(result, index, GetPropertyStringFromId(key));
      ++index;
    }

    return result;
  });
}

/*static*/ JsValueRef CALLBACK ChakraRuntime::HostObjectGetOwnPropertyDescriptorTrap(
    JsValueRef callee,
    bool isConstructCall,
    JsValueRef *args,
    unsigned short argCount,
    void *callbackState) noexcept {
  ChakraRuntime *chakraRuntime = static_cast<ChakraRuntime *>(callbackState);
  return chakraRuntime->HandleCallbackExceptions([&]() {
    ChakraVerifyElseThrow(
        !isConstructCall, "Constructor call for HostObjectGetOwnPropertyDescriptorTrap() is not supported.");

    // args[0] - the Proxy handler object (this) (unused).
    // args[1] - the Proxy target object.
    // args[2] - the property
    ChakraVerifyElseThrow(argCount == 3, "HostObjectGetOwnPropertyDescriptorTrap() requires 3 arguments.");
    const JsValueRef target = args[1];
    auto const &hostObject = *static_cast<std::shared_ptr<facebook::jsi::HostObject> *>(GetExternalData(target));

    const JsValueRef propertyName = args[2];
    if (GetValueType(propertyName) == JsValueType::JsString) {
      const PropNameIDView propertyId{GetPropertyIdFromName(StringToPointer(propertyName).data())};
      return RunInMethodContext("HostObject::getOwnPropertyDescriptor", [&]() {
        auto value = chakraRuntime->ToJsValueRef(hostObject->get(*chakraRuntime, propertyId));
        auto descriptor = chakraRuntime->CreatePropertyDescriptor(value, PropertyAttibutes::None);
        return descriptor;
      });
    }
    return static_cast<JsValueRef>(chakraRuntime->m_undefinedValue);
  });
}

JsValueRef ChakraRuntime::GetHostObjectProxyHandler() {
  if (!m_hostObjectProxyHandler) {
    const JsValueRef handler = CreateObject();
    SetProperty(handler, m_propertyId.get, CreateExternalFunction(m_propertyId.get, 2, HostObjectGetTrap, this));
    SetProperty(handler, m_propertyId.set, CreateExternalFunction(m_propertyId.set, 3, HostObjectSetTrap, this));
    SetProperty(
        handler, m_propertyId.ownKeys, CreateExternalFunction(m_propertyId.ownKeys, 1, HostObjectOwnKeysTrap, this));
    SetProperty(
        handler,
        m_propertyId.getOwnPropertyDescriptor,
        CreateExternalFunction(m_propertyId.getOwnPropertyDescriptor, 3, HostObjectGetOwnPropertyDescriptorTrap, this));
    m_hostObjectProxyHandler = JsRefHolder{handler};
  }

  return m_hostObjectProxyHandler;
}

/*virtual*/ void ChakraRuntime::setupNativePromiseContinuation() noexcept {}

void ChakraRuntime::setupMemoryTracker() noexcept {
  if (runtimeArgs().memoryTracker) {
    size_t initialMemoryUsage = 0;
    JsGetRuntimeMemoryUsage(m_runtime, &initialMemoryUsage);
    runtimeArgs().memoryTracker->Initialize(initialMemoryUsage);

    if (runtimeArgs().runtimeMemoryLimit > 0)
      JsSetRuntimeMemoryLimit(m_runtime, runtimeArgs().runtimeMemoryLimit);

    JsSetRuntimeMemoryAllocationCallback(
        m_runtime,
        runtimeArgs().memoryTracker.get(),
        [](void *callbackState, JsMemoryEventType allocationEvent, size_t allocationSize) -> bool {
          auto memoryTrackerPtr = static_cast<facebook::react::MemoryTracker *>(callbackState);
          switch (allocationEvent) {
            case JsMemoryAllocate:
              memoryTrackerPtr->OnAllocation(allocationSize);
              break;

            case JsMemoryFree:
              memoryTrackerPtr->OnDeallocation(allocationSize);
              break;

            case JsMemoryFailure:
            default:
              break;
          }

          return true;
        });
  }
}

//===========================================================================
// ChakraRuntime::JsValueArgs implementation
//===========================================================================

ChakraRuntime::JsValueArgs::JsValueArgs(
    ChakraRuntime &rt,
    facebook::jsi::Value const &firstArg,
    Span<facebook::jsi::Value const> args)
    : m_count{args.size() + 1},
      m_heapArgs{m_count > MaxStackArgCount ? std::make_unique<JsValueRef[]>(m_count) : nullptr} {
  JsValueRef *const jsArgs = m_heapArgs ? m_heapArgs.get() : m_stackArgs.data();
  jsArgs[0] = rt.ToJsValueRef(firstArg);
  for (size_t i = 1; i < m_count; ++i) {
    jsArgs[i] = rt.ToJsValueRef(args.begin()[i - 1]);
  }
  if (m_heapArgs) {
    // Chakra GC cannot see values on heap. They must be ref-counted.
    for (size_t i = 0; i < m_count; ++i) {
      AddRef(m_heapArgs[i]);
    }
  }
}

ChakraRuntime::JsValueArgs::~JsValueArgs() {
  if (m_heapArgs) {
    for (size_t i = 0; i < m_count; ++i) {
      Release(m_heapArgs[i]);
    }
  }
}

ChakraRuntime::JsValueArgs::operator ChakraApi::Span<JsValueRef>() {
  if (m_count > MaxStackArgCount) {
    return Span<JsValueRef>(m_heapArgs.get(), m_count);
  } else {
    return Span<JsValueRef>(m_stackArgs.data(), m_count);
  }
}

//===========================================================================
// ChakraRuntime::JsiValueView implementation
//===========================================================================

ChakraRuntime::JsiValueView::JsiValueView(JsValueRef jsValue)
    : m_value{InitValue(jsValue, std::addressof(m_pointerStore))} {}

ChakraRuntime::JsiValueView::~JsiValueView() noexcept {}

ChakraRuntime::JsiValueView::operator facebook::jsi::Value const &() const noexcept {
  return m_value;
}

/*static*/ facebook::jsi::Value ChakraRuntime::JsiValueView::InitValue(JsValueRef jsValue, StoreType *store) {
  switch (GetValueType(jsValue)) {
    case JsUndefined:
      return facebook::jsi::Value::undefined();
    case JsNull:
      return facebook::jsi::Value::null();
    case JsNumber:
      return facebook::jsi::Value(NumberToDouble(jsValue));
    case JsString:
      return make<facebook::jsi::String>(new (store) ChakraPointerValueView(jsValue));
    case JsBoolean:
      return facebook::jsi::Value(BooleanToBool(jsValue));
    case JsSymbol:
      return make<facebook::jsi::Symbol>(new (store) ChakraPointerValueView(jsValue));
    case JsObject:
    case JsFunction:
    case JsError:
    case JsArray:
    case JsArrayBuffer:
    case JsTypedArray:
    case JsDataView:
      return make<facebook::jsi::Object>(new (store) ChakraPointerValueView(jsValue));
    default:
      throw facebook::jsi::JSINativeException("Unexpected JsValueType");
  }
}

//===========================================================================
// ChakraRuntime::JsiValueViewArray implementation
//===========================================================================

ChakraRuntime::JsiValueViewArgs::JsiValueViewArgs(JsValueRef *args, size_t argCount) noexcept
    : m_size{argCount},
      m_heapPointerStore{m_size > MaxStackArgCount ? std::make_unique<JsiValueView::StoreType[]>(m_size) : nullptr},
      m_heapArgs{m_size > MaxStackArgCount ? std::make_unique<facebook::jsi::Value[]>(m_size) : nullptr} {
  JsiValueView::StoreType *const pointerStore =
      m_heapPointerStore ? m_heapPointerStore.get() : m_stackPointerStore.data();
  facebook::jsi::Value *const jsiArgs = m_heapArgs ? m_heapArgs.get() : m_stackArgs.data();
  for (uint32_t i = 0; i < m_size; ++i) {
    jsiArgs[i] = JsiValueView::InitValue(args[i], std::addressof(pointerStore[i]));
  }
}

facebook::jsi::Value const *ChakraRuntime::JsiValueViewArgs::Data() const noexcept {
  return m_heapArgs ? m_heapArgs.get() : m_stackArgs.data();
}

size_t ChakraRuntime::JsiValueViewArgs::Size() const noexcept {
  return m_size;
}

//===========================================================================
// ChakraRuntime::PropNameIDView implementation
//===========================================================================

ChakraRuntime::PropNameIDView::PropNameIDView(JsPropertyIdRef propertyId) noexcept
    : m_propertyId{
          make<facebook::jsi::PropNameID>(new (std::addressof(m_pointerStore)) ChakraPointerValueView(propertyId))} {}

ChakraRuntime::PropNameIDView::~PropNameIDView() noexcept {}

ChakraRuntime::PropNameIDView::operator facebook::jsi::PropNameID const &() const noexcept {
  return m_propertyId;
}

//===========================================================================
// ChakraRuntime miscellaneous
//===========================================================================

std::once_flag ChakraRuntime::s_runtimeVersionInitFlag;
uint64_t ChakraRuntime::s_runtimeVersion = 0;

std::unique_ptr<facebook::jsi::Runtime> makeChakraRuntime(ChakraRuntimeArgs &&args) noexcept {
#ifdef CHAKRACORE
  if (React::GetRuntimeOptionBool("JSI.ForceSystemChakra")) {
    return MakeSystemChakraRuntime(std::move(args));
  } else {
    return MakeChakraCoreRuntime(std::move(args));
  }
#else
  return MakeSystemChakraRuntime(std::move(args));
#endif // CHAKRACORE
}

} // namespace Microsoft::JSI
