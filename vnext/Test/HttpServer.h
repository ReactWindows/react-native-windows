#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http.hpp>

#include <thread>

namespace Microsoft::React::Test {

#pragma region Utility functions

boost::beast::multi_buffer CreateStringResponseBody(std::string &&content);

#pragma endregion Utility functions

struct HttpCallbacks {
  std::function<void __cdecl ()> OnResponseSent;
  std::function<boost::beast::http::response<boost::beast::http::dynamic_body> __cdecl (
      const boost::beast::http::request<boost::beast::http::string_body> &)>
      OnGet;
};

///
// Represents a client session in within an owning server instance.
// Generates and submits the appropriate HTTP response.
///
class HttpSession : public std::enable_shared_from_this<HttpSession> {
  boost::asio::ip::tcp::socket &m_socket;
  boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
  boost::beast::flat_buffer m_buffer;
  boost::beast::http::request<boost::beast::http::string_body> m_request;
  std::shared_ptr<boost::beast::http::response<boost::beast::http::dynamic_body>> m_response; // Generic response
  HttpCallbacks &m_callbacks;

  void Read();
  void Respond();
  void Close();

  void OnRead(boost::system::error_code ec, std::size_t transferred);
  void OnWrite(boost::system::error_code ec, std::size_t transferred, bool close);

 public:
  HttpSession(boost::asio::ip::tcp::socket &socket, HttpCallbacks &callbacks);

  ~HttpSession();

  void Start();
};

///
// Represents an HTTP server endpoint (IP:PORT).
// Accepts client connections and dispatches sessions for each incoming
// connection.
///
class HttpServer : public std::enable_shared_from_this<HttpServer> {
  std::thread m_contextThread;
  boost::asio::io_context m_context;
  boost::asio::ip::tcp::acceptor m_acceptor;
  boost::asio::ip::tcp::socket m_socket;
  HttpCallbacks m_callbacks;
  std::vector<std::shared_ptr<HttpSession>> m_sessions;

  void OnAccept(boost::system::error_code ec);

 public:
  ///
  // address - Valid IP address string (i.e. "127.0.0.1).
  // port    - TCP port number (i.e. 80).
  ///
  HttpServer(std::string &&address, std::uint16_t port);

  ~HttpServer();

  void Accept();
  void Start();
  void Stop();

  ///
  // Callback to invoke after a successful response is sent.
  ///
  void SetOnResponseSent(std::function<void __cdecl ()> &&handler) noexcept;

  ///
  // Function that creates an HTTP response to send to the client on GET
  // requests.
  ///
  void SetOnGet(std::function<boost::beast::http::response<boost::beast::http::dynamic_body> __cdecl (
                    const boost::beast::http::request<boost::beast::http::string_body> &)> &&onGet) noexcept;
};

} // namespace Microsoft::React::Test
