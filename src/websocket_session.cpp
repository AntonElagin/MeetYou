#include "../include/websocket_session.h"
#include <boost/property_tree/ptree.hpp>

websocket_session::websocket_session(tcp::socket &&socket)
    : ws_(std::move(socket)) {}

// template<class Body, class Allocator>
// void websocket_session::do_accept(http::request<Body,
// http::basic_fields<Allocator>> req) {
//    // Set suggested timeout settings for the websocket
//    ws_.set_option(
//            websocket::stream_base::timeout::suggested(
//                    beast::role_type::server));
//
//    // Set a decorator to change the Server of the handshake
//    ws_.set_option(websocket::stream_base::decorator(
//            [](websocket::response_type& res)
//            {
//                res.set(http::field::server,
//                        std::string(BOOST_BEAST_VERSION_STRING) +
//                        " advanced-server");
//            }));
//
//    // Accept the websocket handshake
//    ws_.async_accept(
//            req,
//            beast::bind_front_handler(
//                    &websocket_session::on_accept,
//                    shared_from_this()));
//}

void websocket_session::on_accept(beast::error_code ec) {
  if (ec) return fail(ec, "accept");

  // Read a message
  do_read();
}

void websocket_session::do_read() {
  // Read a message into our buffer
  ws_.async_read(buffer_, beast::bind_front_handler(&websocket_session::on_read,
                                                    shared_from_this()));
}

void websocket_session::on_read(beast::error_code ec,
                                std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  // This indicates that the websocket_session was closed
  if (ec == websocket::error::closed) return;

  if (ec) fail(ec, "read");

  // Echo the message
  ws_.text(ws_.got_text());
  ws_.async_write(buffer_.data(),
                  beast::bind_front_handler(&websocket_session::on_write,
                                            shared_from_this()));
}

void websocket_session::on_write(beast::error_code ec,
                                 std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec) return fail(ec, "write");

  // Clear the buffer
  buffer_.consume(buffer_.size());

  // Do another read
  do_read();
}
