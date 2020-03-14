// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// clang-format off
#include "WinRTWebSocketResource.h"

#include <winrt/Windows.Security.Cryptography.h>
#include <Unicode.h>
#include <Utilities.h>

// Standard Library
#include <future>

using Microsoft::Common::Unicode::Utf8ToUtf16;
using Microsoft::Common::Unicode::Utf16ToUtf8;
using Microsoft::Common::Utilities::CheckedReinterpretCast;

using std::function;
using std::size_t;
using std::string;
using std::vector;

using winrt::fire_and_forget;
using winrt::hresult;
using winrt::hresult_error;
using winrt::Windows::Foundation::Uri;
using winrt::Windows::Networking::Sockets::IWebSocket;
using winrt::Windows::Networking::Sockets::MessageWebSocket;
using winrt::Windows::Networking::Sockets::MessageWebSocketMessageReceivedEventArgs;
using winrt::Windows::Networking::Sockets::SocketMessageType;
using winrt::Windows::Networking::Sockets::WebSocketClosedEventArgs;
using winrt::Windows::Security::Cryptography::CryptographicBuffer;
using winrt::Windows::Storage::Streams::DataReader;
using winrt::Windows::Storage::Streams::DataWriter;
using winrt::Windows::Storage::Streams::UnicodeEncoding;

namespace Microsoft::React
{
WinRTWebSocketResource::WinRTWebSocketResource(Uri&& uri)
  : m_uri{ std::move(uri) }
  , m_socket{}
  , m_writer{ m_socket.OutputStream() }
{
  m_socket.MessageReceived({ this, &WinRTWebSocketResource::OnMessageReceived });
  m_socket.Closed({ this, &WinRTWebSocketResource::OnClosed });
}

WinRTWebSocketResource::WinRTWebSocketResource(const string& urlString)
  : WinRTWebSocketResource(Uri{ Utf8ToUtf16(urlString) })
{
}

WinRTWebSocketResource::~WinRTWebSocketResource() /*override*/
{
  //TODO: Clean up
}

#pragma region Private members

fire_and_forget WinRTWebSocketResource::PerformConnect()
{
  hresult hr = S_OK;
  try
  {
    co_await winrt::resume_background();

    co_await m_socket.ConnectAsync(m_uri);

    m_connectHandler();
  }
  catch (hresult_error const& e)
  {
    hr = e.code();
  }

  if (!SUCCEEDED(hr))
  {
    hresult_error e{ hr };
    //OutputDebugString("WebSocket.connect failed (0x%8X) %ls\n", e);
    m_errorHandler({ Utf16ToUtf8(e.message()), ErrorType::Connection });
  }
}

void WinRTWebSocketResource::OnMessageReceived(IWebSocket const& sender, MessageWebSocketMessageReceivedEventArgs const& args)
{
  try
  {
    string response;
    DataReader reader = args.GetDataReader();
    auto len = reader.UnconsumedBufferLength();
    if (args.MessageType() == SocketMessageType::Utf8)
    {
      reader.UnicodeEncoding(UnicodeEncoding::Utf8);
      vector<uint8_t> data(len);
      reader.ReadBytes(data);

      response = string(CheckedReinterpretCast<char*>(data.data()), data.size());
    }
    else
    {
      auto buffer = reader.ReadBuffer(len);
      winrt::hstring data = CryptographicBuffer::EncodeToBase64String(buffer);

      response = Utf16ToUtf8(std::wstring_view(data));
    }

    if (m_readHandler)
      m_readHandler(response.length(), response);//TODO: move?
  }
  catch (hresult_error const& e)
  {
    //TODO: Re-enable (fix "OutputDebugStringW': function does not take 2 arguments")
    //OutputDebugString("Read failed (0x%8X) %ls\n", e);
    m_errorHandler({ Utf16ToUtf8(e.message()), ErrorType::Receive });
  }
}

void WinRTWebSocketResource::OnClosed(IWebSocket const& sender, WebSocketClosedEventArgs const& args)
{
  if (m_closeHandler)
  {
    //TODO: Parameterize (pass via member variables?)
    m_closeHandler(CloseCode::Normal, "Closing");
  }
}

#pragma endregion Private members

#pragma region IWebSocketResource

void WinRTWebSocketResource::Connect(const Protocols& protocols, const Options& options)
{
  for (auto& header : options)
  {
    m_socket.SetRequestHeader(header.first, Utf8ToUtf16(header.second));
  }

  winrt::Windows::Foundation::Collections::IVector<winrt::hstring> supportedProtocols =
    m_socket.Control().SupportedProtocols();
  for (auto& protocol : protocols)
  {
    supportedProtocols.Append(Utf8ToUtf16(protocol));
  }

  PerformConnect();
}

void WinRTWebSocketResource::Ping()
{

}

void WinRTWebSocketResource::Send(const string& message)
{

}

void WinRTWebSocketResource::SendBinary(const string& base64String)
{

}

void WinRTWebSocketResource::Close(CloseCode code, const string& reason)
{
  m_socket.Close(static_cast<uint16_t>(code), Utf8ToUtf16(reason) );
}

IWebSocketResource::ReadyState WinRTWebSocketResource::GetReadyState() const
{
  return ReadyState::Closed;
}

void WinRTWebSocketResource::SetOnConnect(function<void()>&& handler)
{
  m_connectHandler = handler;
}

void WinRTWebSocketResource::SetOnPing(function<void()>&& handler)
{

}

void WinRTWebSocketResource::SetOnSend(function<void(size_t)>&& handler)
{

}

void WinRTWebSocketResource::SetOnMessage(function<void(size_t, const string&)>&& handler)
{
  m_readHandler = handler;
}

void WinRTWebSocketResource::SetOnClose(function<void(CloseCode, const string&)>&& handler)
{
  m_closeHandler = handler;
}

void WinRTWebSocketResource::SetOnError(function<void(Error&&)>&& handler)
{
  m_errorHandler = handler;
}

#pragma endregion IWebSocketResource

}// namespace Microsoft::React
