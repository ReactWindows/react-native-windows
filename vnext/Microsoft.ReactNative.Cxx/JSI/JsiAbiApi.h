// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_JSIABIAPI
#define MICROSOFT_REACTNATIVE_JSIABIAPI

#include <map>
#include <mutex>
#include "Crash.h"
#include "jsi/jsi.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

static constexpr size_t MaxCallArgCount = 32;

// Forward declare the facebook::jsi::Runtime implementation on top of ABI-safe JsiRuntime.
struct JsiAbiRuntime;

// An ABI-safe wrapper for facebook::jsi::Buffer.
struct JsiByteBufferWrapper : implements<JsiByteBufferWrapper, IJsiByteBuffer> {
  JsiByteBufferWrapper(JsiRuntime const &runtime, std::shared_ptr<facebook::jsi::Buffer const> const &buffer) noexcept;
  ~JsiByteBufferWrapper() noexcept;
  uint32_t Size();
  void GetData(JsiByteArrayUser const &useBytes);

 private:
  JsiRuntime m_runtime;
  std::shared_ptr<facebook::jsi::Buffer const> m_buffer;
};

// A wrapper for ABI-safe JsiPreparedJavaScript.
struct JsiPreparedJavaScriptWrapper : facebook::jsi::PreparedJavaScript {
  JsiPreparedJavaScriptWrapper(JsiPreparedJavaScript const &preparedScript) noexcept;
  ~JsiPreparedJavaScriptWrapper() noexcept;
  JsiPreparedJavaScript const &Get() const noexcept;

 private:
  JsiPreparedJavaScript m_preparedScript;
};

// An ABI-safe wrapper for facebook::jsi::HostObject.
struct JsiHostObjectWrapper : implements<JsiHostObjectWrapper, IJsiHostObject> {
  JsiHostObjectWrapper(std::shared_ptr<facebook::jsi::HostObject> &&hostObject) noexcept;

  JsiValueRef GetProperty(JsiRuntime const &runtime, JsiPropertyIdRef const &name);
  void SetProperty(JsiRuntime const &runtime, JsiPropertyIdRef const &name, JsiValueRef const &value);
  Windows::Foundation::Collections::IVector<JsiPropertyIdRef> GetPropertyIds(JsiRuntime const &runtime);

  std::shared_ptr<facebook::jsi::HostObject> const &HostObjectSharedPtr() noexcept;

 private:
  std::shared_ptr<facebook::jsi::HostObject> m_hostObject;
};

// The function object that wraps up the facebook::jsi::HostFunctionType
struct JsiHostFunctionWrapper {
  // We only support new and move constructors.
  JsiHostFunctionWrapper(facebook::jsi::HostFunctionType &&hostFunction) noexcept;

  JsiValueRef operator()(JsiRuntime const &runtime, JsiValueRef const &thisArg, array_view<JsiValueRef const> args);

  static facebook::jsi::HostFunctionType &GetHostFunction(JsiHostFunction const &hostFunction) noexcept;

 private:
  facebook::jsi::HostFunctionType m_hostFunction;
  JsiObjectRef m_functionData{};
};

// JSI runtime implementation as a wrapper for the ABI-safe JsiRuntime.
struct JsiAbiRuntime : facebook::jsi::Runtime {
  JsiAbiRuntime(JsiRuntime const &runtime) noexcept;
  ~JsiAbiRuntime() noexcept;

  // Get JsiAbiRuntime from JsiRuntime in current thread.
  static JsiAbiRuntime *GetFromJsiRuntime(JsiRuntime const &runtime) noexcept;

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
  facebook::jsi::Instrumentation &instrumentation() override;

 protected:
  PointerValue *cloneSymbol(const PointerValue *pv) override;
  PointerValue *cloneString(const PointerValue *pv) override;
  PointerValue *cloneObject(const PointerValue *pv) override;
  PointerValue *clonePropNameID(const PointerValue *pv) override;

  facebook::jsi::PropNameID createPropNameIDFromAscii(const char *str, size_t length) override;
  facebook::jsi::PropNameID createPropNameIDFromUtf8(const uint8_t *utf8, size_t length) override;
  facebook::jsi::PropNameID createPropNameIDFromString(const facebook::jsi::String &str) override;
  std::string utf8(const facebook::jsi::PropNameID &propertyNameId) override;
  bool compare(const facebook::jsi::PropNameID &left, const facebook::jsi::PropNameID &right) override;

  std::string symbolToString(const facebook::jsi::Symbol &symbol) override;

  facebook::jsi::String createStringFromAscii(const char *str, size_t length) override;
  facebook::jsi::String createStringFromUtf8(const uint8_t *utf8, size_t length) override;
  std::string utf8(const facebook::jsi::String &str) override;

  facebook::jsi::Value createValueFromJsonUtf8(const uint8_t *json, size_t length) override;

  facebook::jsi::Object createObject() override;
  facebook::jsi::Object createObject(std::shared_ptr<facebook::jsi::HostObject> ho) override;
  std::shared_ptr<facebook::jsi::HostObject> getHostObject(const facebook::jsi::Object &obj) override;
  facebook::jsi::HostFunctionType &getHostFunction(const facebook::jsi::Function &func) override;
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
  facebook::jsi::Array getPropertyNames(const facebook::jsi::Object &obj) override;

  facebook::jsi::WeakObject createWeakObject(const facebook::jsi::Object &obj) override;
  facebook::jsi::Value lockWeakObject(facebook::jsi::WeakObject &weakObj) override;

  facebook::jsi::Array createArray(size_t length) override;
  size_t size(const facebook::jsi::Array &arr) override;
  size_t size(const facebook::jsi::ArrayBuffer &arrayBuffer) override;
  uint8_t *data(const facebook::jsi::ArrayBuffer &arrayBuffer) override;
  facebook::jsi::Value getValueAtIndex(const facebook::jsi::Array &arr, size_t i) override;
  void setValueAtIndexImpl(facebook::jsi::Array &arr, size_t i, const facebook::jsi::Value &value) override;

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

  ScopeState *pushScope() override;
  void popScope(ScopeState *scope) override;

  bool strictEquals(const facebook::jsi::Symbol &a, const facebook::jsi::Symbol &b) const override;
  bool strictEquals(const facebook::jsi::String &a, const facebook::jsi::String &b) const override;
  bool strictEquals(const facebook::jsi::Object &a, const facebook::jsi::Object &b) const override;
  bool instanceOf(const facebook::jsi::Object &o, const facebook::jsi::Function &f) override;

  void RethrowJsiError() const;

  void SetJsiError(facebook::jsi::JSError const &jsError) noexcept;
  void SetJsiError(std::exception const &nativeException) noexcept;

 private: // Convert JSI to ABI-safe JSI values
  static JsiSymbolRef const &AsJsiSymbolRef(PointerValue const *pv) noexcept;
  static JsiStringRef const &AsJsiStringRef(PointerValue const *pv) noexcept;
  static JsiObjectRef const &AsJsiObjectRef(PointerValue const *pv) noexcept;
  static JsiPropertyIdRef const &AsJsiPropertyIdRef(PointerValue const *pv) noexcept;

  static JsiSymbolRef const &AsJsiSymbolRef(facebook::jsi::Symbol const &symbol) noexcept;
  static JsiStringRef const &AsJsiStringRef(facebook::jsi::String const &str) noexcept;
  static JsiObjectRef const &AsJsiObjectRef(facebook::jsi::Object const &obj) noexcept;
  static JsiPropertyIdRef const &AsJsiPropertyIdRef(facebook::jsi::PropNameID const &propertyId) noexcept;
  static JsiWeakObjectRef const &AsJsiWeakObjectRef(facebook::jsi::WeakObject const &weakObject) noexcept;
  static JsiValueRef AsJsiValueRef(facebook::jsi::Value const &value) noexcept;

  static JsiPropertyIdRef DetachJsiPropertyIdRef(facebook::jsi::PropNameID &&propertyId) noexcept;
  static JsiValueRef DetachJsiValueRef(facebook::jsi::Value &&value) noexcept;

 private: // Convert ABI-safe JSI to JSI values
  PointerValue *MakeSymbolValue(JsiSymbolRef &&symbol) const noexcept;
  PointerValue *MakeStringValue(JsiStringRef &&str) const noexcept;
  PointerValue *MakeObjectValue(JsiObjectRef &&obj) const noexcept;
  PointerValue *MakePropNameIDValue(JsiPropertyIdRef &&propertyId) const noexcept;

  facebook::jsi::Symbol MakeSymbol(JsiSymbolRef &&symbol) const noexcept;
  facebook::jsi::String MakeString(JsiStringRef &&str) const noexcept;
  facebook::jsi::Object MakeObject(JsiObjectRef &&obj) const noexcept;
  facebook::jsi::PropNameID MakePropNameID(JsiPropertyIdRef &&propertyId) const noexcept;
  facebook::jsi::Array MakeArray(JsiObjectRef &&arr) noexcept;
  facebook::jsi::WeakObject MakeWeakObject(JsiWeakObjectRef &&weakObject) const noexcept;
  facebook::jsi::Function MakeFunction(JsiObjectRef &&func) noexcept;
  facebook::jsi::Value MakeValue(JsiValueRef &&value) const noexcept;

  // Allow access to the helper function
  friend struct JsiByteBufferWrapper;
  friend struct JsiHostObjectWrapper;
  friend struct JsiHostFunctionWrapper;
  friend struct AbiJSError;
  friend struct AbiJSINativeException;

 private: // PointerValue structures
  struct DataPointerValue : PointerValue {
    DataPointerValue(winrt::weak_ref<JsiRuntime> &&weakRuntime, uint64_t data) noexcept;
    DataPointerValue(uint64_t data) noexcept;
    void invalidate() override;

    uint64_t m_data;
    winrt::weak_ref<JsiRuntime> m_weakRuntime;
  };

  struct SymbolPointerValue : DataPointerValue {
    SymbolPointerValue(winrt::weak_ref<JsiRuntime> &&weakRuntime, JsiSymbolRef &&symbol) noexcept;
    void invalidate() override;
    static JsiSymbolRef const &GetData(PointerValue const *pv) noexcept;
    static JsiSymbolRef Detach(PointerValue const *pv) noexcept;
  };

  struct StringPointerValue : DataPointerValue {
    StringPointerValue(winrt::weak_ref<JsiRuntime> &&weakRuntime, JsiStringRef &&str) noexcept;
    void invalidate() override;
    static JsiStringRef const &GetData(PointerValue const *pv) noexcept;
    static JsiStringRef Detach(PointerValue const *pv) noexcept;
  };

  struct ObjectPointerValue : DataPointerValue {
    ObjectPointerValue(winrt::weak_ref<JsiRuntime> &&weakRuntime, JsiObjectRef &&obj) noexcept;
    void invalidate() override;
    static JsiObjectRef const &GetData(PointerValue const *pv) noexcept;
    static JsiObjectRef Detach(PointerValue const *pv) noexcept;
  };

  struct PropNameIDPointerValue : DataPointerValue {
    PropNameIDPointerValue(winrt::weak_ref<JsiRuntime> &&weakRuntime, JsiPropertyIdRef &&propertyId) noexcept;
    void invalidate() override;
    static JsiPropertyIdRef const &GetData(PointerValue const *pv) noexcept;
    static JsiPropertyIdRef Detach(PointerValue const *pv) noexcept;
  };

  // This type is to represent a reference to Value based on JsiValueData.
  // It avoids extra memory allocation by using an in-place storage.
  // It does not release the underlying pointer on invalidate() call
  // by proving null as runtime pointer.
  struct ValueRef {
    ValueRef(JsiValueRef const &data) noexcept;
    ~ValueRef() noexcept;
    operator facebook::jsi::Value const &() const noexcept;

    using StoreType = std::aligned_storage_t<sizeof(DataPointerValue)>;
    static void InitValueRef(JsiValueRef const &data, facebook::jsi::Value *value, StoreType *store) noexcept;

   private:
    StoreType m_pointerStore{};
    facebook::jsi::Value m_value{};
  };

  struct ValueRefArray {
    ValueRefArray(array_view<JsiValueRef const> args) noexcept;
    facebook::jsi::Value const *Data() const noexcept;
    size_t Size() const noexcept;

   private:
    std::array<ValueRef::StoreType, MaxCallArgCount> m_pointerStoreArray{};
    std::array<facebook::jsi::Value, MaxCallArgCount> m_valueArray{};
    size_t m_size{};
  };

  struct PropNameIDRef {
    PropNameIDRef(JsiPropertyIdRef const &data) noexcept;
    ~PropNameIDRef() noexcept;
    operator facebook::jsi::PropNameID const &() const noexcept;

    using StoreType = std::aligned_storage_t<sizeof(DataPointerValue)>;

   private:
    StoreType m_pointerStore{};
    facebook::jsi::PropNameID m_propertyId;
  };

 private:
  JsiRuntime m_runtime;
  bool m_pendingJSError{false};
};

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_JSIABIAPI
