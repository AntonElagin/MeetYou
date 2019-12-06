#ifndef TECHPROJECT_WEBSOCKETSESSION_H
#define TECHPROJECT_WEBSOCKETSESSION_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "Fail.h"
#include "IWebsocketSession.h"

namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace websocket = boost::beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

// Echoes back all received WebSocket messages
class WebsocketSession : public std::enable_shared_from_this<WebsocketSession> {
  websocket::stream<beast::tcp_stream> ws_;
  beast::flat_buffer buffer_;

 public:
  // Take ownership of the socket
  explicit WebsocketSession(tcp::socket &&socket);

  // Start the asynchronous accept operation
  template <class Body, class Allocator>
  void do_accept(http::request<Body, http::basic_fields<Allocator>> req) {
    // Set suggested timeout settings for the websocket
    ws_.set_option(
        websocket::stream_base::timeout::suggested(beast::role_type::server));

    // Set a decorator to change the Server of the handshake
    ws_.set_option(
        websocket::stream_base::decorator([](websocket::response_type &res) {
          res.set(http::field::server,
                  std::string(BOOST_BEAST_VERSION_STRING) + " advanced-server");
        }));

    // Accept the websocket handshake
    ws_.async_accept(req, beast::bind_front_handler(&WebsocketSession::onAccept,
                                                    shared_from_this()));
  }

 private:
  void onAccept(beast::error_code ec);

  void doRead();

  void onRead(beast::error_code ec, std::size_t bytes_transferred);

  void onWrite(beast::error_code ec, std::size_t bytes_transferred);
};

//------------------------------------------------------------------------------

#endif  // TECHPROJECT_WEBSOCKETSESSION_H
