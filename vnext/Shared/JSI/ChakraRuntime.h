// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// Do not include this file directly.
// To obtain a chakra runtime instance, include <ChakraRuntimeFactory.h>.
#pragma once

#include "ChakraApi.h"
#include "ChakraRuntimeArgs.h"

#include <jsi/jsi.h>

#include <array>
#include <mutex>
#include <sstream>

namespace Microsoft::JSI {

// Implementation of Chakra JSI Runtime
class ChakraRuntime : public facebook::jsi::Runtime, public ChakraApi, ChakraApi::IExceptionThrower {
 public:
  ChakraRuntime(ChakraRuntimeArgs &&args) noexcept;
  void Init() noexcept;
  virtual ~ChakraRuntime() noexcept;

#pragma region Functions_inherited_from_Runtime

  facebook::jsi::Value evaluateJavaScript(
      const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
      const std::string &sourceURL) override;

  std::shared_ptr<const facebook::jsi::PreparedJavaScript> prepareJavaScript(
      const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
      std::string sourceURL) override;

  facebook::jsi::Value evaluatePreparedJavaScript(
      const std::shared_ptr<const facebook::jsi::PreparedJavaScript> &js) override;

  bool drainMicrotasks(int maxMicrotasksHint = -1) override;

  facebook::jsi::Object global() override;

  std::string description() override;

  bool isInspectable() override;

  // We use the default instrumentation() implementation that returns an
  // Instrumentation instance which returns no metrics.

 private:
  // Despite the name "clone" suggesting a deep copy, a return value of these
  // functions points to a new heap allocated ChakraPointerValue whose member
  // JsRefHolder refers to the same JavaScript object as the member
  // JsRefHolder of *pointerValue. This behavior is consistent with that of
  // HermesRuntime and JSCRuntime. Also, Like all ChakraPointerValues, the
  // return value must only be used as an argument to the constructor of
  // jsi::Pointer or one of its derived classes.
  PointerValue *cloneSymbol(const PointerValue *pointerValue) override;
  PointerValue *cloneString(const PointerValue *pointerValue) override;
  PointerValue *cloneObject(const PointerValue *pointerValue) override;
  PointerValue *clonePropNameID(const PointerValue *pointerValue) override;

  facebook::jsi::PropNameID createPropNameIDFromAscii(const char *str, size_t length) override;
  facebook::jsi::PropNameID createPropNameIDFromUtf8(const uint8_t *utf8, size_t length) override;
  facebook::jsi::PropNameID createPropNameIDFromString(const facebook::jsi::String &str) override;
  std::string utf8(const facebook::jsi::PropNameID &id) override;
  bool compare(const facebook::jsi::PropNameID &lhs, const facebook::jsi::PropNameID &rhs) override;

  std::string symbolToString(const facebook::jsi::Symbol &s) override;

  // Despite its name, createPropNameIDFromAscii is the same function as
  // createStringFromUtf8.
  facebook::jsi::String createStringFromAscii(const char *str, size_t length) override;
  facebook::jsi::String createStringFromUtf8(const uint8_t *utf8, size_t length) override;
  std::string utf8(const facebook::jsi::String &str) override;

  facebook::jsi::Object createObject() override;
  facebook::jsi::Object createObject(std::shared_ptr<facebook::jsi::HostObject> ho) override;
  std::shared_ptr<facebook::jsi::HostObject> getHostObject(const facebook::jsi::Object &) override;
  facebook::jsi::HostFunctionType &getHostFunction(const facebook::jsi::Function &) override;

  facebook::jsi::Value getProperty(const facebook::jsi::Object &obj, const facebook::jsi::PropNameID &name) override;
  facebook::jsi::Value getProperty(const facebook::jsi::Object &obj, const facebook::jsi::String &name) override;
  bool hasProperty(const facebook::jsi::Object &obj, const facebook::jsi::PropNameID &name) override;
  bool hasProperty(const facebook::jsi::Object &obj, const facebook::jsi::String &name) override;
  void setPropertyValue(
      facebook::jsi::Object &obj,
      const facebook::jsi::PropNameID &name,
      const facebook::jsi::Value &value) override;
  void setPropertyValue(
      facebook::jsi::Object &obj,
      const facebook::jsi::String &name,
      const facebook::jsi::Value &value) override;

  bool isArray(const facebook::jsi::Object &obj) const override;
  bool isArrayBuffer(const facebook::jsi::Object &obj) const override;
  bool isFunction(const facebook::jsi::Object &obj) const override;
  bool isHostObject(const facebook::jsi::Object &obj) const override;
  bool isHostFunction(const facebook::jsi::Function &func) const override;
  // Returns the names of all enumerable properties of an object. This
  // corresponds the properties iterated through by the JavaScript for..in loop.
  facebook::jsi::Array getPropertyNames(const facebook::jsi::Object &obj) override;

  facebook::jsi::WeakObject createWeakObject(const facebook::jsi::Object &obj) override;
  facebook::jsi::Value lockWeakObject(facebook::jsi::WeakObject &weakObj) override;

  facebook::jsi::Array createArray(size_t length) override;
  size_t size(const facebook::jsi::Array &arr) override;
  size_t size(const facebook::jsi::ArrayBuffer &arrBuf) override;
  // The lifetime of the buffer returned is the same as the lifetime of the
  // ArrayBuffer. The returned buffer pointer does not count as a reference to
  // the ArrayBuffer for the purpose of garbage collection.
  uint8_t *data(const facebook::jsi::ArrayBuffer &arrBuf) override;
  facebook::jsi::Value getValueAtIndex(const facebook::jsi::Array &arr, size_t index) override;
  void setValueAtIndexImpl(facebook::jsi::Array &arr, size_t index, const facebook::jsi::Value &value) override;

  facebook::jsi::Function createFunctionFromHostFunction(
      const facebook::jsi::PropNameID &name,
      unsigned int paramCount,
      facebook::jsi::HostFunctionType func) override;
  facebook::jsi::Value call(
      const facebook::jsi::Function &func,
      const facebook::jsi::Value &jsThis,
      const facebook::jsi::Value *args,
      size_t count) override;
  facebook::jsi::Value
  callAsConstructor(const facebook::jsi::Function &func, const facebook::jsi::Value *args, size_t count) override;

  // For now, pushing a scope does nothing, and popping a scope forces the
  // JavaScript garbage collector to run.
  ScopeState *pushScope() override;
  void popScope(ScopeState *) override;

  bool strictEquals(const facebook::jsi::Symbol &a, const facebook::jsi::Symbol &b) const override;
  bool strictEquals(const facebook::jsi::String &a, const facebook::jsi::String &b) const override;
  bool strictEquals(const facebook::jsi::Object &a, const facebook::jsi::Object &b) const override;

  bool instanceOf(const facebook::jsi::Object &obj, const facebook::jsi::Function &func) override;

#pragma endregion Functions_inherited_from_Runtime

 protected:
  ChakraRuntimeArgs &runtimeArgs() {
    return m_args;
  }

  // Promise Helpers
  static void CALLBACK PromiseContinuationCallback(JsValueRef funcRef, void *callbackState) noexcept;
  static void CALLBACK
  PromiseRejectionTrackerCallback(JsValueRef promise, JsValueRef reason, bool handled, void *callbackState);

  // Note: For simplicity, We are pinning the script and serialized script
  // buffers in the facebook::jsi::Runtime instance assuming as these buffers
  // are needed to stay alive for the lifetime of the facebook::jsi::Runtime
  // implementation. This approach doesn't make sense for other external buffers
  // which may get created during the execution as that will stop the backing
  // buffer from getting released when the JSValue gets collected.
  JsRuntimeHandle m_runtime;
  std::string m_debugRuntimeName;
  int m_debugPort{0};
  constexpr static char DebuggerDefaultRuntimeName[] = "runtime1";
  constexpr static int DebuggerDefaultPort = 9229;

  // These buffers are kept to serve the source callbacks when evaluating
  // serialized scripts.
  std::vector<std::shared_ptr<const facebook::jsi::Buffer>> m_pinnedScripts;

  // These three functions only performs shallow copies.
  facebook::jsi::Value ToJsiValue(JsValueRef ref);
  JsValueRef ToJsValueRef(const facebook::jsi::Value &value);

 private: //  ChakraApi::IExceptionThrower members
  [[noreturn]] void ThrowJsExceptionOverride(JsErrorCode errorCode, JsValueRef jsError) override;
  [[noreturn]] void ThrowNativeExceptionOverride(char const *errorMessage) override;
  void RewriteErrorMessage(JsValueRef jsError);

 private:
  // ChakraPointerValueView is the base class for ChakraPointerValue.
  // It holds the JsRef, but unlike the ChakraPointerValue does nothing in the
  // invalidate() method.
  // It is used by the JsiValueView, JsiValueViewArray, and JsiPropNameIDView classes
  // to keep temporary PointerValues on the call stack to avoid extra memory allocations.
  struct ChakraPointerValueView : PointerValue {
    ChakraPointerValueView(JsRef jsRef) noexcept : m_jsRef{jsRef} {}

    ChakraPointerValueView(ChakraPointerValueView const &) = delete;
    ChakraPointerValueView &operator=(ChakraPointerValueView const &) = delete;

    void invalidate() noexcept override {}

    JsRef GetRef() const noexcept {
      return m_jsRef;
    }

   private:
    JsRef m_jsRef;
  };

  // ChakraPointerValue is needed for working with Facebook's jsi::Pointer class
  // and must only be used for this purpose. Every instance of
  // ChakraPointerValue should be allocated on the heap and be used as an
  // argument to the constructor of jsi::Pointer or one of its derived classes.
  // Pointer makes sure that invalidate(), which frees the heap allocated
  // ChakraPointerValue, is called upon destruction. Since the constructor of
  // jsi::Pointer is protected, we usually have to invoke it through
  // jsi::Runtime::make. The code should look something like:
  //
  //     make<Pointer>(new ChakraPointerValue(...));
  //
  // or you can use the helper function MakePointer(), as defined below.
  struct ChakraPointerValue final : ChakraPointerValueView {
    ChakraPointerValue(JsRef ref) noexcept : ChakraPointerValueView{ref} {
      if (ref) {
        AddRef(ref);
      }
    }

    void invalidate() noexcept override {
      delete this;
    }

   private:
    // ~ChakraPointerValue() should only be invoked by invalidate().
    // Hence we make it private.
    ~ChakraPointerValue() noexcept override {
      if (JsRef ref = GetRef()) {
        // JSI allows the destructor to be run on a random thread.
        // To handle it correctly we must schedule a task to the thread associated
        // with the Chakra context. For now we just leak the value.
        // TODO: Implement proper ref count release if it is called on a wrong thread.
        JsRelease(ref, nullptr); // We ignore the error until we fix the TODO above.
      }
    }
  };

  template <typename T, std::enable_if_t<std::is_base_of_v<facebook::jsi::Pointer, T>, int> = 0>
  static T MakePointer(JsRef ref) {
    return make<T>(new ChakraPointerValue(ref));
  }

  // The pointer passed to this function must point to a ChakraPointerValue.
  static ChakraPointerValue *CloneChakraPointerValue(const PointerValue *pointerValue) {
    return new ChakraPointerValue(static_cast<const ChakraPointerValue *>(pointerValue)->GetRef());
  }

  // The jsi::Pointer passed to this function must hold a ChakraPointerValue.
  static JsRef GetJsRef(const facebook::jsi::Pointer &p) {
    return static_cast<const ChakraPointerValueView *>(getPointerValue(p))->GetRef();
  }

  JsValueRef CreateExternalFunction(
      JsPropertyIdRef name,
      int32_t paramCount,
      JsNativeFunction nativeFunction,
      void *callbackState);

  // Host function helper
  static JsValueRef CALLBACK HostFunctionCall(
      JsValueRef callee,
      bool isConstructCall,
      JsValueRef *args,
      unsigned short argCount,
      void *callbackState);

  // Host object helpers; runtime must be referring to a ChakraRuntime.
  static JsValueRef CALLBACK HostObjectGetTrap(
      JsValueRef callee,
      bool isConstructCall,
      JsValueRef *args,
      unsigned short argCount,
      void *callbackState) noexcept;

  static JsValueRef CALLBACK HostObjectSetTrap(
      JsValueRef callee,
      bool isConstructCall,
      JsValueRef *args,
      unsigned short argCount,
      void *callbackState) noexcept;

  static JsValueRef CALLBACK HostObjectOwnKeysTrap(
      JsValueRef callee,
      bool isConstructCall,
      JsValueRef *args,
      unsigned short argCount,
      void *callbackState) noexcept;

  static JsValueRef CALLBACK HostObjectGetOwnPropertyDescriptorTrap(
      JsValueRef callee,
      bool isConstructCall,
      JsValueRef *args,
      unsigned short argCount,
      void *callbackState) noexcept;

  JsValueRef GetHostObjectProxyHandler();

  // Evaluate lambda and augment exception messages with the methodName.
  template <typename TLambda>
  static auto RunInMethodContext(char const *methodName, TLambda lambda) {
    try {
      return lambda();
    } catch (facebook::jsi::JSError const &) {
      throw; // do not augment the JSError exceptions.
    } catch (std::exception const &ex) {
      ChakraThrow((std::string{"Exception in "} + methodName + ": " + ex.what()).c_str());
    } catch (...) {
      ChakraThrow((std::string{"Exception in "} + methodName + ": <unknown>").c_str());
    }
  }

  // Evaluate lambda and convert all exceptions to Chakra engine exceptions.
  template <typename TLambda>
  JsValueRef HandleCallbackExceptions(TLambda lambda) noexcept {
    try {
      try {
        return lambda();
      } catch (facebook::jsi::JSError const &jsError) {
        // This block may throw exceptions
        SetException(ToJsValueRef(jsError.value()));
      }
    } catch (std::exception const &ex) {
      SetException(ex.what());
    } catch (...) {
      SetException(L"Unexpected error");
    }

    return m_undefinedValue;
  }

  void PromiseContinuation(JsValueRef value) noexcept;
  void PromiseRejectionTracker(JsValueRef promise, JsValueRef reason, bool handled);

  virtual void setupNativePromiseContinuation() noexcept;

  // Memory tracker helpers
  void setupMemoryTracker() noexcept;

  // In-proc debugging helpers
  virtual void startDebuggingIfNeeded() = 0;
  virtual void stopDebuggingIfNeeded() = 0;

  // Version related helpers
  static void initRuntimeVersion() noexcept;
  static uint64_t getRuntimeVersion() {
    return s_runtimeVersion;
  }

  // Miscellaneous
  virtual std::unique_ptr<const facebook::jsi::Buffer> generatePreparedScript(
      const std::string &sourceURL,
      const facebook::jsi::Buffer &sourceBuffer) noexcept = 0;
  virtual facebook::jsi::Value evaluateJavaScriptSimple(
      const facebook::jsi::Buffer &buffer,
      const std::string &sourceURL) = 0;
  virtual bool evaluateSerializedScript(
      const facebook::jsi::Buffer &scriptBuffer,
      const facebook::jsi::Buffer &serializedScriptBuffer,
      const std::string &sourceURL,
      JsValueRef *result) = 0;

  enum class PropertyAttibutes {
    None = 0,
    ReadOnly = 1 << 1,
    DontEnum = 1 << 2,
    DontDelete = 1 << 3,
    Frozen = ReadOnly | DontDelete,
    DontEnumAndFrozen = DontEnum | Frozen,
  };

  friend constexpr PropertyAttibutes operator&(PropertyAttibutes left, PropertyAttibutes right) {
    return (PropertyAttibutes)((int)left & (int)right);
  }

  friend constexpr bool operator!(PropertyAttibutes attrs) {
    return attrs == PropertyAttibutes::None;
  }

  JsValueRef CreatePropertyDescriptor(JsValueRef value, PropertyAttibutes attrs);

  // The number of arguments that we keep on stack.
  // We use heap if we have more argument.
  constexpr static size_t MaxStackArgCount = 8;

  // JsValueArgs helps to optimize passing arguments to Chakra function.
  // If number of arguments is below or equal to MaxStackArgCount,
  // then they are kept on call stack, otherwise arguments are allocated on heap.
  struct JsValueArgs final {
    JsValueArgs(ChakraRuntime &rt, facebook::jsi::Value const &firstArg, Span<facebook::jsi::Value const> args);
    ~JsValueArgs();
    operator ChakraApi::Span<JsValueRef>();

   private:
    size_t const m_count{};
    std::array<JsValueRef, MaxStackArgCount> m_stackArgs{{JS_INVALID_REFERENCE}};
    std::unique_ptr<JsValueRef[]> const m_heapArgs;
  };

  // This type represents a view to Value based on JsValueRef.
  // It avoids extra memory allocation by using an in-place storage.
  // It uses ChakraPointerValueView that does nothing in the invalidate() method.
  struct JsiValueView final {
    JsiValueView(JsValueRef jsValue);
    ~JsiValueView() noexcept;
    operator facebook::jsi::Value const &() const noexcept;

    using StoreType = std::aligned_storage_t<sizeof(ChakraPointerValueView)>;
    static facebook::jsi::Value InitValue(JsValueRef jsValue, StoreType *store);

   private:
    StoreType m_pointerStore{};
    facebook::jsi::Value const m_value{};
  };

  // This class helps to use stack storage for passing arguments that must be temporary converted from
  // JsValueRef to facebook::jsi::Value.
  struct JsiValueViewArgs final {
    JsiValueViewArgs(JsValueRef *args, size_t argCount) noexcept;
    facebook::jsi::Value const *Data() const noexcept;
    size_t Size() const noexcept;

   private:
    size_t const m_size{};
    std::array<JsiValueView::StoreType, MaxStackArgCount> m_stackPointerStore{};
    std::array<facebook::jsi::Value, MaxStackArgCount> m_stackArgs{};
    std::unique_ptr<JsiValueView::StoreType[]> const m_heapPointerStore{};
    std::unique_ptr<facebook::jsi::Value[]> const m_heapArgs{};
  };

  // PropNameIDView helps to use the stack storage for temporary conversion from
  // JsPropertyIdRef to facebook::jsi::PropNameID.
  struct PropNameIDView final {
    PropNameIDView(JsPropertyIdRef propertyId) noexcept;
    ~PropNameIDView() noexcept;
    operator facebook::jsi::PropNameID const &() const noexcept;

    using StoreType = std::aligned_storage_t<sizeof(ChakraPointerValueView)>;

   private:
    StoreType m_pointerStore{};
    facebook::jsi::PropNameID const m_propertyId;
  };

 private:
  // Property ID cache to improve execution speed
  struct PropertyId final {
    JsRefHolder Object;
    JsRefHolder Proxy;
    JsRefHolder Symbol;
    JsRefHolder byteLength;
    JsRefHolder configurable;
    JsRefHolder enumerable;
    JsRefHolder get;
    JsRefHolder getOwnPropertyDescriptor;
    JsRefHolder hostFunctionSymbol;
    JsRefHolder hostObjectSymbol;
    JsRefHolder length;
    JsRefHolder message;
    JsRefHolder ownKeys;
    JsRefHolder propertyIsEnumerable;
    JsRefHolder prototype;
    JsRefHolder set;
    JsRefHolder toString;
    JsRefHolder value;
    JsRefHolder writable;
  } m_propertyId;

  JsRefHolder m_undefinedValue;
  JsRefHolder m_proxyConstructor;
  JsRefHolder m_hostObjectProxyHandler;

  static std::once_flag s_runtimeVersionInitFlag;
  static uint64_t s_runtimeVersion;

  // Arguments shared by the specializations
  ChakraRuntimeArgs m_args;

  JsRefHolder m_context;
  JsRefHolder m_prevContext;

  // Set the Chakra API exception thrower on this thread
  ExceptionThrowerHolder m_exceptionThrower{this};

  // These buffers back the external array buffers that we handover to
  // ChakraCore.
  std::vector<std::shared_ptr<const facebook::jsi::Buffer>> m_pinnedPreparedScripts;

  bool m_pendingJSError{false};
};

} // namespace Microsoft::JSI
