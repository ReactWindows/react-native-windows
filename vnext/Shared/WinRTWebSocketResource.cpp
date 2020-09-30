// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "WinRTWebSocketResource.h"

#include <Unicode.h>
#include <Utilities.h>
#include <Utils/CppWinrtLessExceptions.h>

// Windows API
#include <RestrictedErrorInfo.h>
#include <roerrorapi.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Security.Cryptography.h>

using Microsoft::Common::Unicode::Utf16ToUtf8;
using Microsoft::Common::Unicode::Utf8ToUtf16;
using Microsoft::Common::Utilities::CheckedReinterpretCast;

using std::function;
using std::lock_guard;
using std::mutex;
using std::size_t;
using std::string;
using std::vector;

using winrt::fire_and_forget;
using winrt::hresult;
using winrt::hresult_error;
using winrt::resume_background;
using winrt::resume_on_signal;
using winrt::Windows::Foundation::IAsyncAction;
using winrt::Windows::Foundation::Uri;
using winrt::Windows::Networking::Sockets::IMessageWebSocket;
using winrt::Windows::Networking::Sockets::IMessageWebSocketMessageReceivedEventArgs;
using winrt::Windows::Networking::Sockets::IWebSocket;
using winrt::Windows::Networking::Sockets::MessageWebSocket;
using winrt::Windows::Networking::Sockets::SocketMessageType;
using winrt::Windows::Networking::Sockets::WebSocketClosedEventArgs;
using winrt::Windows::Security::Cryptography::CryptographicBuffer;
using winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult;
using winrt::Windows::Storage::Streams::DataWriter;
using winrt::Windows::Storage::Streams::DataWriterStoreOperation;
using winrt::Windows::Storage::Streams::IDataReader;
using winrt::Windows::Storage::Streams::IDataWriter;
using winrt::Windows::Storage::Streams::UnicodeEncoding;

namespace {
///
/// Implements an awaiter for Mso::DispatchQueue
///
auto resume_in_queue(const Mso::DispatchQueue &queue) noexcept {
  struct awaitable {
    awaitable(const Mso::DispatchQueue &queue) noexcept : m_queue{queue} {}

    bool await_ready() const noexcept {
      return false;
    }

    void await_resume() const noexcept {}

    void await_suspend(std::experimental::coroutine_handle<> resume) noexcept {
      m_callback = [context = resume.address()]() noexcept {
        std::experimental::coroutine_handle<>::from_address(context)();
      };
      m_queue.Post(std::move(m_callback));
    }

   private:
    Mso::DispatchQueue m_queue;
    Mso::VoidFunctor m_callback;
  };

  return awaitable{queue};
}
} // namespace

namespace Microsoft::React {

WinRTWebSocketResource::WinRTWebSocketResource(
    IMessageWebSocket &&socket,
    IDataWriter &&writer,
    Uri &&uri,
    vector<ChainValidationResult> &&certExeptions) noexcept
    : m_uri{std::move(uri)}, m_socket{std::move(socket)}, m_writer{std::move(writer)} {
  m_socket.MessageReceived({this, &WinRTWebSocketResource::OnMessageReceived});

  for (auto certException : certExeptions) {
    m_socket.Control().IgnorableServerCertificateErrors().Append(certException);
  }
}

WinRTWebSocketResource::WinRTWebSocketResource(
    IMessageWebSocket &&socket,
    Uri &&uri,
    vector<ChainValidationResult> certExeptions) noexcept
    : WinRTWebSocketResource(
          std::move(socket),
          DataWriter{socket.OutputStream()},
          std::move(uri),
          std::move(certExeptions)) {}

WinRTWebSocketResource::WinRTWebSocketResource(
    const string &urlString,
    vector<ChainValidationResult> &&certExeptions) noexcept
    : WinRTWebSocketResource(MessageWebSocket{}, Uri{Utf8ToUtf16(urlString)}, std::move(certExeptions)) {}

WinRTWebSocketResource::~WinRTWebSocketResource() noexcept /*override*/
{
  Close(CloseCode::GoingAway, "Disposed");

  m_closePerformed.Wait();
}

#pragma region Private members

IAsyncAction WinRTWebSocketResource::PerformConnect() noexcept {
  auto self = shared_from_this();

  co_await resume_background();

  try {
    auto async = self->m_socket.ConnectAsync(self->m_uri);

    co_await lessthrow_await_adapter<IAsyncAction>{async};

    if (SUCCEEDED(async.ErrorCode())) {
      self->m_readyState = ReadyState::Open;
      if (self->m_connectHandler) {
        self->m_connectHandler();
      }
    } else {
      if (self->m_errorHandler) {
        self->m_errorHandler({GetRestrictedErrorMessage(), ErrorType::Connection});
      }
    }

  } catch (hresult_error const &e) {
    if (self->m_errorHandler) {
      self->m_errorHandler({Utf16ToUtf8(e.message()), ErrorType::Connection});
    }
  }

  SetEvent(self->m_connectPerformed.get());
  self->m_connectPerformedPromise.set_value();
  self->m_connectRequested = false;
}

fire_and_forget WinRTWebSocketResource::PerformPing() noexcept {
  auto self = shared_from_this();

  co_await resume_background();

  co_await resume_on_signal(self->m_connectPerformed.get());

  if (self->m_readyState != ReadyState::Open) {
    self = nullptr;
    co_return;
  }

  try {
    self->m_socket.Control().MessageType(SocketMessageType::Utf8);

    string s{};
    winrt::array_view<const uint8_t> arr(
        CheckedReinterpretCast<const uint8_t *>(s.c_str()),
        CheckedReinterpretCast<const uint8_t *>(s.c_str()) + s.length());
    self->m_writer.WriteBytes(arr);

    auto async = self->m_writer.StoreAsync();

    co_await lessthrow_await_adapter<DataWriterStoreOperation>{async};

    if (SUCCEEDED(async.ErrorCode())) {
      if (self->m_pingHandler) {
        self->m_pingHandler();
      }
    } else {
      if (self->m_errorHandler) {
        self->m_errorHandler({GetRestrictedErrorMessage(), ErrorType::Ping});
      }
    }
  } catch (hresult_error const &e) {
    if (self->m_errorHandler) {
      self->m_errorHandler({Utf16ToUtf8(e.message()), ErrorType::Ping});
    }
  }
}

fire_and_forget WinRTWebSocketResource::PerformWrite(string &&message, bool isBinary) noexcept {
  auto self = shared_from_this();
  {
    auto guard = lock_guard<mutex>{m_writeQueueMutex};
    m_writeQueue.emplace(std::move(message), isBinary);
  }

  co_await resume_background();

  co_await resume_on_signal(self->m_connectPerformed.get()); // Ensure connection attempt has finished

  co_await resume_in_queue(self->m_dispatchQueue); // Ensure writes happen sequentially

  if (self->m_readyState != ReadyState::Open) {
    self = nullptr;
    co_return;
  }

  try {
    size_t length;
    string messageLocal;
    bool isBinaryLocal;
    {
      auto guard = lock_guard<mutex>{m_writeQueueMutex};
      std::tie(messageLocal, isBinaryLocal) = m_writeQueue.front();
      m_writeQueue.pop();
    }

    if (isBinaryLocal) {
      self->m_socket.Control().MessageType(SocketMessageType::Binary);

      auto buffer = CryptographicBuffer::DecodeFromBase64String(Utf8ToUtf16(messageLocal));
      length = buffer.Length();
      self->m_writer.WriteBuffer(buffer);
    } else {
      self->m_socket.Control().MessageType(SocketMessageType::Utf8);

      // TODO: Use char_t instead of uint8_t?
      length = messageLocal.size();
      winrt::array_view<const uint8_t> view(
          CheckedReinterpretCast<const uint8_t *>(messageLocal.c_str()),
          CheckedReinterpretCast<const uint8_t *>(messageLocal.c_str()) + messageLocal.length());
      self->m_writer.WriteBytes(view);
    }

    auto async = self->m_writer.StoreAsync();

    co_await lessthrow_await_adapter<DataWriterStoreOperation>{async};

    if (SUCCEEDED(async.ErrorCode())) {
      if (self->m_writeHandler) {
        self->m_writeHandler(length);
      }
    } else {
      if (self->m_errorHandler) {
        self->m_errorHandler({GetRestrictedErrorMessage(), ErrorType::Send});
      }
    }
  } catch (std::exception const &e) {
    // Utf8ToUtf16 may throw
    if (self->m_errorHandler) {
      self->m_errorHandler({e.what(), ErrorType::Ping});
    }
  } catch (hresult_error const &e) {
    // TODO: Remove after fixing unit tests exceptions.
    if (self->m_errorHandler) {
      self->m_errorHandler({Utf16ToUtf8(e.message()), ErrorType::Ping});
    }
  }
}

fire_and_forget WinRTWebSocketResource::PerformClose() noexcept {
  co_await resume_background();

  co_await resume_on_signal(m_connectPerformed.get());

  try {
    m_socket.Close(static_cast<uint16_t>(m_closeCode), Utf8ToUtf16(m_closeReason));

    if (m_closeHandler) {
      m_closeHandler(m_closeCode, m_closeReason);
    }
  } catch (winrt::hresult_invalid_argument const &) {
    if (m_errorHandler) {
      m_errorHandler({GetRestrictedErrorMessage(), ErrorType::Close});
    }
  } catch (hresult_error const &e) {
    if (m_errorHandler) {
      m_errorHandler({Utf16ToUtf8(e.message()), ErrorType::Close});
    }
  }

  m_readyState = ReadyState::Closed;
  m_closePerformed.Set();
}

void WinRTWebSocketResource::OnMessageReceived(
    IWebSocket const &sender,
    IMessageWebSocketMessageReceivedEventArgs const &args) {
  try {
    string response;
    IDataReader reader = args.GetDataReader();
    auto len = reader.UnconsumedBufferLength();
    if (args.MessageType() == SocketMessageType::Utf8) {
      reader.UnicodeEncoding(UnicodeEncoding::Utf8);
      vector<uint8_t> data(len);
      reader.ReadBytes(data);

      response = string(CheckedReinterpretCast<char *>(data.data()), data.size());
    } else {
      auto buffer = reader.ReadBuffer(len);
      winrt::hstring data = CryptographicBuffer::EncodeToBase64String(buffer);

      response = Utf16ToUtf8(std::wstring_view(data));
    }

    if (m_readHandler) {
      m_readHandler(response.length(), response, args.MessageType() == SocketMessageType::Binary);
    }
  } catch (hresult_error const &e) {
    if (m_errorHandler) {
      m_errorHandler({Utf16ToUtf8(e.message()), ErrorType::Receive});
    }
  }
}

void WinRTWebSocketResource::Synchronize() noexcept {
  // Ensure sequence of other operations
  if (m_connectRequested) {
    m_connectPerformedPromise.get_future().wait();
  }
}

string WinRTWebSocketResource::GetRestrictedErrorMessage() noexcept {
  string result;
  winrt::com_ptr<IRestrictedErrorInfo> errorInfo;
  if (SUCCEEDED(GetRestrictedErrorInfo(errorInfo.put()))) {
    BSTR description;
    HRESULT error;
    BSTR restrictedDesc;
    BSTR capabilitySid;
    if (SUCCEEDED(errorInfo->GetErrorDetails(&description, &error, &restrictedDesc, &capabilitySid))) {
      try {
        result = Utf16ToUtf8(description) + ": " + Utf16ToUtf8(restrictedDesc);
      } catch (const std::exception &e) {
        result = e.what();
      }
    } else {
      result = "Unknown error";
    }
    // BSTR's allocated memory must be explicitly released.
    // See https://docs.microsoft.com/en-us/cpp/atl-mfc-shared/allocating-and-releasing-memory-for-a-bstr
    SysFreeString(description);
    SysFreeString(restrictedDesc);
    SysFreeString(capabilitySid);
  } else {
    result = "Unknown error";
  }

  return std::move(result);
}

#pragma endregion Private members

#pragma region IWebSocketResource

void WinRTWebSocketResource::Connect(const Protocols &protocols, const Options &options) noexcept {
  m_readyState = ReadyState::Connecting;

  for (const auto &header : options) {
    m_socket.SetRequestHeader(header.first, Utf8ToUtf16(header.second));
  }

  winrt::Windows::Foundation::Collections::IVector<winrt::hstring> supportedProtocols =
      m_socket.Control().SupportedProtocols();
  for (const auto &protocol : protocols) {
    supportedProtocols.Append(Utf8ToUtf16(protocol));
  }

  m_connectRequested = true;
  PerformConnect();
}

void WinRTWebSocketResource::Ping() noexcept {
  PerformPing();
}

void WinRTWebSocketResource::Send(string &&message) noexcept {
  PerformWrite(std::move(message), false);
}

void WinRTWebSocketResource::SendBinary(string &&base64String) noexcept {
  PerformWrite(std::move(base64String), true);
}

void WinRTWebSocketResource::Close(CloseCode code, const string &reason) noexcept {
  if (m_readyState == ReadyState::Closing || m_readyState == ReadyState::Closed)
    return;

  Synchronize();

  m_readyState = ReadyState::Closing;
  m_closeCode = code;
  m_closeReason = reason;
  PerformClose();
}

IWebSocketResource::ReadyState WinRTWebSocketResource::GetReadyState() const noexcept {
  return m_readyState;
}

void WinRTWebSocketResource::SetOnConnect(function<void()> &&handler) noexcept {
  m_connectHandler = std::move(handler);
}

void WinRTWebSocketResource::SetOnPing(function<void()> &&handler) noexcept {
  m_pingHandler = std::move(handler);
}

void WinRTWebSocketResource::SetOnSend(function<void(size_t)> &&handler) noexcept {
  m_writeHandler = std::move(handler);
}

void WinRTWebSocketResource::SetOnMessage(function<void(size_t, const string &, bool isBinary)> &&handler) noexcept {
  m_readHandler = std::move(handler);
}

void WinRTWebSocketResource::SetOnClose(function<void(CloseCode, const string &)> &&handler) noexcept {
  m_closeHandler = std::move(handler);
}

void WinRTWebSocketResource::SetOnError(function<void(Error &&)> &&handler) noexcept {
  m_errorHandler = std::move(handler);
}

#pragma endregion IWebSocketResource

} // namespace Microsoft::React
