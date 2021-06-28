// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Modules/WebSocketModule.h>

#include <Utils.h>
#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>
#include "Unicode.h"

// Standard Libriary
#include <iomanip>

using namespace facebook::xplat;
using namespace folly;

using Microsoft::Common::Unicode::Utf16ToUtf8;
using Microsoft::Common::Unicode::Utf8ToUtf16;

using std::shared_ptr;
using std::string;
using std::weak_ptr;

namespace {
constexpr char moduleName[] = "WebSocketModule";
} // anonymous namespace

namespace Microsoft::React {

WebSocketModule::WebSocketModule()
    : m_resourceFactory{[](string &&url) { return IWebSocketResource::Make(std::move(url)); }} {}

void WebSocketModule::SetResourceFactory(
    std::function<shared_ptr<IWebSocketResource>(const string &)> &&resourceFactory) {
  m_resourceFactory = std::move(resourceFactory);
}

string WebSocketModule::getName() {
  return moduleName;
}

std::map<string, dynamic> WebSocketModule::getConstants() {
  return {};
}

// clang-format off
std::vector<facebook::xplat::module::CxxModule::Method> WebSocketModule::getMethods()
{
  return
  {
    Method(
      "connect",
      [this](dynamic args) // const string& url, dynamic protocols, dynamic options, int64_t id
      {
        IWebSocketResource::Protocols protocols;
        dynamic protocolsDynamic = jsArgAsDynamic(args, 1);
        if (!protocolsDynamic.empty())
        {
          for (const auto& protocol : protocolsDynamic)
          {
            protocols.push_back(protocol.getString());
          }
        }

        IWebSocketResource::Options options;
        dynamic optionsDynamic = jsArgAsDynamic(args, 2);
        if (!optionsDynamic.empty() && optionsDynamic.count("headers") != 0)
        {
          const auto& headersDynamic = optionsDynamic["headers"];
          for (const auto& header : headersDynamic.items())
          {
            options.emplace(Utf8ToUtf16(header.first.getString()), header.second.getString());
          }
        }

        weak_ptr weakWs = this->GetOrCreateWebSocket(jsArgAsInt(args, 3), jsArgAsString(args, 0));
        if (auto sharedWs = weakWs.lock())
        {
          sharedWs->Connect(protocols, options);
        }
      }),
    Method(
      "close",
      [this](dynamic args) // [int64_t code, string reason,] int64_t id
      {
        // See react-native\Libraries\WebSocket\WebSocket.js:_close
        if (args.size() == 3) // WebSocketModule.close(statusCode, closeReason, this._socketId);
        {
          weak_ptr weakWs = this->GetOrCreateWebSocket(jsArgAsInt(args, 2));
          if (auto sharedWs = weakWs.lock())
          {
            sharedWs->Close(static_cast<IWebSocketResource::CloseCode>(jsArgAsInt(args, 0)), jsArgAsString(args, 1));
          }
        }
        else if (args.size() == 1) // WebSocketModule.close(this._socketId);
        {
          weak_ptr weakWs = this->GetOrCreateWebSocket(jsArgAsInt(args, 0));
          if (auto sharedWs = weakWs.lock())
          {
            sharedWs->Close(IWebSocketResource::CloseCode::Normal, {});
          }
        }
        else
        {
          auto errorObj = dynamic::object("id", -1)("message", "Incorrect number of parameters");
          this->SendEvent("websocketFailed", std::move(errorObj));
        }
      }),
    Method(
      "send",
      [this](dynamic args) // const string& message, int64_t id
      {
        weak_ptr weakWs = this->GetOrCreateWebSocket(jsArgAsInt(args, 1));
        if (auto sharedWs = weakWs.lock())
        {
          sharedWs->Send(jsArgAsString(args, 0));
        }
      }),
    Method(
      "sendBinary",
      [this](dynamic args) // const string& base64String, int64_t id
      {
        weak_ptr weakWs = this->GetOrCreateWebSocket(jsArgAsInt(args, 1));
        if (auto sharedWs = weakWs.lock())
        {
          sharedWs->SendBinary(jsArgAsString(args, 0));
        }
      }),
    Method(
      "ping",
      [this](dynamic args) // int64_t id
      {
        weak_ptr weakWs = this->GetOrCreateWebSocket(jsArgAsInt(args, 0));
        if (auto sharedWs = weakWs.lock())
        {
          sharedWs->Ping();
        }
      })
  };
} // getMethods
// clang-format on

#pragma region private members

void WebSocketModule::SendEvent(string &&eventName, dynamic &&args) {
  auto weakInstance = this->getInstance();
  if (auto instance = weakInstance.lock()) {
    instance->callJSFunction("RCTDeviceEventEmitter", "emit", dynamic::array(std::move(eventName), std::move(args)));
  }
}

// clang-format off
shared_ptr<IWebSocketResource> WebSocketModule::GetOrCreateWebSocket(int64_t id, string&& url)
{
  auto itr = m_webSockets.find(id);
  if (itr == m_webSockets.end())
  {
    shared_ptr<IWebSocketResource> ws;
    try
    {
      ws = m_resourceFactory(std::move(url));
    }
    catch (const winrt::hresult_error& e)
    {
      std::stringstream ss;
      ss << "[" << std::hex << std::showbase << std::setw(8) << static_cast<uint32_t>(e.code()) << "] " <<
        winrt::to_string(e.message());

      SendEvent("webSocketFailed", dynamic::object("id", id)("message", std::move(ss.str())));

      return nullptr;
    }
    catch (const std::exception& e)
    {
      SendEvent("webSocketFailed", dynamic::object("id", id)("message", e.what()));

      return nullptr;
    }
    catch (...)
    {
      SendEvent("webSocketFailed", dynamic::object("id", id)("message", "Unidentified error creating IWebSocketResource"));

      return nullptr;
    }

    auto weakInstance = this->getInstance();
    ws->SetOnError([this, id, weakInstance](const IWebSocketResource::Error& err)
    {
      auto strongInstance = weakInstance.lock();
      if (!strongInstance)
        return;

      auto errorObj = dynamic::object("id", id)("message", err.Message);
      this->SendEvent("websocketFailed", std::move(errorObj));
    });
    ws->SetOnConnect([this, id, weakInstance]()
    {
      auto strongInstance = weakInstance.lock();
      if (!strongInstance)
        return;

      auto args = dynamic::object("id", id);
      this->SendEvent("websocketOpen", std::move(args));
    });
    ws->SetOnMessage([this, id, weakInstance](size_t length, const string& message, bool isBinary)
    {
      auto strongInstance = weakInstance.lock();
      if (!strongInstance)
        return;

      auto args = dynamic::object("id", id)("data", message)("type", isBinary ? "binary" : "text");
      this->SendEvent("websocketMessage", std::move(args));
    });
    ws->SetOnClose([this, id, weakInstance](IWebSocketResource::CloseCode code, const string& reason)
    {
      auto strongInstance = weakInstance.lock();
      if (!strongInstance)
        return;

      auto args = dynamic::object("id", id)("code", static_cast<uint16_t>(code))("reason", reason);
      this->SendEvent("websocketClosed", std::move(args));
    });

    m_webSockets.emplace(id, ws);
    return ws;
  }

  return itr->second;
}
// clang-format on

#pragma endregion private members

/*extern*/ std::unique_ptr<facebook::xplat::module::CxxModule> CreateWebSocketModule() noexcept {
  return make_unique<WebSocketModule>();
}

} // namespace Microsoft::React
