// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "BaseScriptStoreImpl.h"
#include "MemoryMappedBuffer.h"

#include <RuntimeOptions.h>

// C++/WinRT
#include <winrt/base.h>

// Standard Library
#include <fstream>

namespace facebook {
namespace react {

namespace {

class ByteArrayBuffer : public facebook::jsi::Buffer {
 public:
  size_t size() const override {
    return size_;
  }

  const uint8_t *data() const override {
    return byteArray_.get();
  }

  uint8_t *data() {
    return byteArray_.get();
  }

  ByteArrayBuffer(size_t size) : size_(size), byteArray_(std::make_unique<uint8_t[]>(size)) {}

  ByteArrayBuffer(ByteArrayBuffer &&) = default;
  ByteArrayBuffer &operator=(ByteArrayBuffer &&) = default;

 private:
  ByteArrayBuffer(const ByteArrayBuffer &) = delete;
  ByteArrayBuffer &operator=(const ByteArrayBuffer &) = delete;

  size_t size_;
  std::unique_ptr<uint8_t[]> byteArray_;
};

class BufferViewBuffer : public facebook::jsi::Buffer {
 public:
  size_t size() const override {
    return size_;
  }

  const uint8_t *data() const override {
    return buffer_->data() + offset_;
  }

  uint8_t *data() {
    return const_cast<uint8_t *>(buffer_->data()) + offset_;
  }

  BufferViewBuffer(std::unique_ptr<const facebook::jsi::Buffer> buffer, size_t offset, size_t size)
      : buffer_(std::move(buffer)), offset_(offset), size_(size) {
    if (size_ > buffer_->size() - offset)
      std::terminate();
  }

  BufferViewBuffer(BufferViewBuffer &&) = default;
  BufferViewBuffer &operator=(BufferViewBuffer &&) = default;

 private:
  BufferViewBuffer(const BufferViewBuffer &) = delete;
  BufferViewBuffer &operator=(const BufferViewBuffer &) = delete;

  std::unique_ptr<const facebook::jsi::Buffer> buffer_;
  size_t offset_;
  size_t size_;
};

constexpr const char *PERSIST_MAGIC = "RNWPREP";
constexpr const char *PERSIST_EOF = "EOF";

int constexpr length__(const char *str) {
  return *str ? 1 + length__(str + 1) : 0;
}

// TODO : Enforce any packing policy ?
struct PreparedScriptPrefix {
  // TODO :: constexpr initialize the array.
  char magic[length__(PERSIST_MAGIC)];
  jsi::ScriptVersion_t scriptVersion;
  jsi::JSRuntimeVersion_t runtimeVersion;
  uint64_t sizeInBytes;
};

struct PreparedScriptSuffix {
  char eof[length__(PERSIST_EOF)];
};

} // namespace

jsi::VersionedBuffer BaseScriptStoreImpl::getVersionedScript(const std::string &url) noexcept {
  std::ifstream file(url, std::ios::binary | std::ios::ate);

  if (!file) {
    return {nullptr, 0};
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  auto buffer = std::make_unique<ByteArrayBuffer>(static_cast<size_t>(size));
  if (!file.read(reinterpret_cast<char *>(buffer->data()), size)) {
    return {nullptr, 0};
  }

  file.close();

  return {std::move(buffer), versionProvider_ ? versionProvider_->getVersion(url) : static_cast<uint64_t>(size)};
}

jsi::ScriptVersion_t BaseScriptStoreImpl::getScriptVersion(const std::string &url) noexcept {
  if (versionProvider_) {
    return versionProvider_->getVersion(url);
  } else {
    std::ifstream file(url, std::ios::binary | std::ios::ate);

    if (!file) {
      return 0;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    return static_cast<uint64_t>(size);
  }
}

std::unique_ptr<const jsi::Buffer> LocalFileSimpleBufferStore::getBuffer(const std::string &bufferId) noexcept {
  // 1. Store path must be set
  // 2. It must be a directory that exists. TODO :: Figure out a cross platform
  // way to ensure this.
  // 3. Directory must end with the path delimiter. TODO :: Verify this.
  if (storeDirectory_.empty()) {
    std::terminate();
  }

  if (Microsoft::React::GetRuntimeOptionBool("JSI.MemoryMappedScriptStore")) {
    try {
      return Microsoft::JSI::MakeMemoryMappedBuffer(winrt::to_hstring(storeDirectory_ + bufferId).c_str());
    } catch (const facebook::jsi::JSINativeException &) {
      return nullptr;
    }
  } else {
    // Treat buffer id as the relative path fragment.
    std::ifstream file(storeDirectory_ + bufferId, std::ios::binary | std::ios::ate);

    if (!file) {
      return nullptr;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    auto buffer = std::make_unique<ByteArrayBuffer>(static_cast<size_t>(size));
    if (!file.read(reinterpret_cast<char *>(buffer->data()), size)) {
      return nullptr;
    }

    return buffer;
  }
}

bool LocalFileSimpleBufferStore::persistBuffer(
    const std::string &relativeUrl,
    std::unique_ptr<const jsi::Buffer> buffer) noexcept {
  // Assumptions on storeDirectory_ same as in getRawBuffer
  if (storeDirectory_.empty())
    std::terminate();

  std::ofstream file;
  file.open(storeDirectory_ + relativeUrl, std::ios::binary | std::ios::trunc);
  if (!file)
    return false;

  file.write(reinterpret_cast<const char *>(buffer->data()), buffer->size());
  file.close();

  return true;
}

std::string BasePreparedScriptStoreImpl::getPreparedScriptFileName(
    const jsi::ScriptSignature &scriptSignature,
    const jsi::JSRuntimeSignature &runtimeSignature,
    const char *prepareTag) {
  // Essentially, we are trying to construct,
  // prep_<source_url>_<runtime_id>_<preparation_tag>.cache

  std::string prparedScriptFileName("prep_");

  const std::string &scriptUrl = scriptSignature.url;

  // As a crude heuristic we choose the last 64 characters of the source url.
  constexpr int MAXLENGTH = 64;
  prparedScriptFileName.append(
      scriptUrl.begin() + ((scriptUrl.size() < MAXLENGTH) ? 0 : (scriptUrl.size() - MAXLENGTH)), scriptUrl.end());

  // Make a valid file name.
  std::replace(prparedScriptFileName.begin(), prparedScriptFileName.end(), '\\', '_');
  std::replace(prparedScriptFileName.begin(), prparedScriptFileName.end(), '/', '_');
  std::replace(prparedScriptFileName.begin(), prparedScriptFileName.end(), ':', '_');
  std::replace(prparedScriptFileName.begin(), prparedScriptFileName.end(), '.', '_');

  if (runtimeSignature.runtimeName.empty()) {
    std::terminate();
  }

  prparedScriptFileName.append("_");
  prparedScriptFileName.append(runtimeSignature.runtimeName);

  if (prepareTag) {
    prparedScriptFileName.append("_");
    prparedScriptFileName.append(prepareTag);
  }

  // TODO :: Need to constuct a hash. ref:
  // https://en.wikipedia.org/wiki/Base64#Filenames

  // extension
  prparedScriptFileName.append(".cache");

  return prparedScriptFileName;
}

std::shared_ptr<const jsi::Buffer> BasePreparedScriptStoreImpl::tryGetPreparedScript(
    const jsi::ScriptSignature &scriptSignature,
    const jsi::JSRuntimeSignature &runtimeSignature,
    const char *prepareTag) noexcept {
  std::string preparedScriptFilePath = getPreparedScriptFileName(scriptSignature, runtimeSignature, prepareTag);

  auto buffer = bufferStore_->getBuffer(preparedScriptFilePath);

  if (!buffer) {
    return nullptr;
  }

  const PreparedScriptPrefix *prefix = reinterpret_cast<const PreparedScriptPrefix *>(buffer->data());

  if (strncmp(prefix->magic, PERSIST_MAGIC, sizeof(prefix->magic)) != 0) {
    // magic value doesn't match!! The store is very likely corrupted or belongs
    // to old version.
    return nullptr;
  }

  if (prefix->scriptVersion != scriptSignature.version) {
    // script version don't match!! Need to regenerate cache.
    return nullptr;
  }

  if (prefix->runtimeVersion != runtimeSignature.version) {
    // Runtime changed after the cache generation.
    return nullptr;
  }

  if (prefix->sizeInBytes != buffer->size() - sizeof(PreparedScriptPrefix) - sizeof(PreparedScriptSuffix)) {
    // Size is not as expected. Store is possibly corrupted .. It is safer to
    // bail out.
    return nullptr;
  }

  const PreparedScriptSuffix *suffix = reinterpret_cast<const PreparedScriptSuffix *>(
      buffer->data() + sizeof(PreparedScriptPrefix) + prefix->sizeInBytes);
  if (strncmp(suffix->eof, PERSIST_EOF, sizeof(suffix->eof)) != 0) {
    // magic value doesn't match!! The store is very likely corrupted or belongs
    // to old version.
    return nullptr;
  }

  return std::make_shared<BufferViewBuffer>(
      std::move(buffer), sizeof(PreparedScriptPrefix), static_cast<size_t>(prefix->sizeInBytes));
}

void BasePreparedScriptStoreImpl::persistPreparedScript(
    std::shared_ptr<const jsi::Buffer> preparedScript,
    const jsi::ScriptSignature &scriptMetadata,
    const jsi::JSRuntimeSignature &runtimeMetadata,
    const char *prepareTag) noexcept {
  // TODO :: Unfortunately, The current abstraction is forcing us to make a
  // copy. Need to re-evaluate.
  auto newBuffer = std::make_unique<ByteArrayBuffer>(
      sizeof(PreparedScriptPrefix) + preparedScript->size() + sizeof(PreparedScriptSuffix));

  PreparedScriptPrefix *prefix = reinterpret_cast<PreparedScriptPrefix *>(newBuffer->data());
  memcpy_s(prefix->magic, sizeof(prefix->magic), PERSIST_MAGIC, sizeof(prefix->magic));
  prefix->scriptVersion = scriptMetadata.version;
  prefix->runtimeVersion = runtimeMetadata.version;
  prefix->sizeInBytes = preparedScript->size();

  memcpy_s(
      newBuffer->data() + sizeof(PreparedScriptPrefix),
      newBuffer->size() - sizeof(PreparedScriptPrefix),
      preparedScript->data(),
      preparedScript->size());

  PreparedScriptSuffix *suffix = reinterpret_cast<PreparedScriptSuffix *>(
      newBuffer->data() + sizeof(PreparedScriptPrefix) + preparedScript->size());
  memcpy_s(suffix->eof, sizeof(suffix->eof), PERSIST_EOF, sizeof(suffix->eof));

  std::string preparedScriptFilePath = getPreparedScriptFileName(scriptMetadata, runtimeMetadata, prepareTag);

  bufferStore_->persistBuffer(preparedScriptFilePath, std::move(newBuffer));
}

} // namespace react
} // namespace facebook
