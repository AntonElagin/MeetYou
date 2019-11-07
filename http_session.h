
#ifndef TECHPROJECT_HTTP_SESSION_H
#define TECHPROJECT_HTTP_SESSION_H

#include "queue.h"

namespace beast = boost::beast;


// Handles an HTTP server connection
class http_session : public std::enable_shared_from_this<http_session>
{



  beast::tcp_stream stream_;
  beast::flat_buffer buffer_;
  std::shared_ptr<std::string const> doc_root_;
  queue queue_;

  // The parser is stored in an optional container so we can
  // construct it from scratch it at the beginning of each new message.
  boost::optional<http::request_parser<http::string_body>> parser_;

public:
  // Take ownership of the socket
  http_session(
      tcp::socket&& socket,
      std::shared_ptr<std::string const> const& doc_root)
      : stream_(std::move(socket))
      , doc_root_(doc_root)
      , queue_(*this)
  {
  }

  // Start the session
  void
  run()
  {
    do_read();
  }

private:
  void
  do_read()
  {
    // Construct a new parser for each message
    parser_.emplace();

    // Apply a reasonable limit to the allowed size
    // of the body in bytes to prevent abuse.
    parser_->body_limit(10000);

    // Set the timeout.
    stream_.expires_after(std::chrono::seconds(30));

    // Read a request using the parser-oriented interface
    http::async_read(
        stream_,
        buffer_,
        *parser_,
        beast::bind_front_handler(
            &http_session::on_read,
            shared_from_this()));
  }

  void
  on_read(beast::error_code ec, std::size_t bytes_transferred)
  {
    boost::ignore_unused(bytes_transferred);

    // This means they closed the connection
    if(ec == http::error::end_of_stream)
      return do_close();

    if(ec)
      return fail(ec, "read");

    // See if it is a WebSocket Upgrade
    if(websocket::is_upgrade(parser_->get()))
    {
      // Create a websocket session, transferring ownership
      // of both the socket and the HTTP request.
      std::make_shared<websocket_session>(
          stream_.release_socket())->do_accept(parser_->release());
      return;
    }

    // Send the response
    handle_request(*doc_root_, parser_->release(), queue_);

    // If we aren't at the queue limit, try to pipeline another request
    if(! queue_.is_full())
      do_read();
  }

  void
  on_write(bool close, beast::error_code ec, std::size_t bytes_transferred)
  {
    boost::ignore_unused(bytes_transferred);

    if(ec)
      return fail(ec, "write");

    if(close)
    {
      // This means we should close the connection, usually because
      // the response indicated the "Connection: close" semantic.
      return do_close();
    }

    // Inform the queue that a write completed
    if(queue_.on_write())
    {
      // Read another request
      do_read();
    }
  }

  void
  do_close()
  {
    // Send a TCP shutdown
    beast::error_code ec;
    stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

    // At this point the connection is closed gracefully
  }
};


#endif //TECHPROJECT_HTTP_SESSION_H
