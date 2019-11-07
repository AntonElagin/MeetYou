//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: Advanced server
//
//------------------------------------------------------------------------------

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/strand.hpp>
#include <boost/make_unique.hpp>
#include <boost/optional.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "http_session.h"

namespace beast = boost::beast;                 // from <boost/beast.hpp>
namespace http = beast::http;                   // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;         // from <boost/beast/websocket.hpp>
namespace net = boost::asio;                    // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>

// Return a reasonable mime type based on the extension of a file.
beast::string_view
mime_type(beast::string_view path)
{
  using beast::iequals;
  auto const ext = [&path]
  {
    auto const pos = path.rfind(".");
    if(pos == beast::string_view::npos)
      return beast::string_view{};
    return path.substr(pos);
  }();
  if(iequals(ext, ".htm"))  return "text/html";
  if(iequals(ext, ".html")) return "text/html";
  if(iequals(ext, ".php"))  return "text/html";
  if(iequals(ext, ".css"))  return "text/css";
  if(iequals(ext, ".txt"))  return "text/plain";
  if(iequals(ext, ".js"))   return "application/javascript";
  if(iequals(ext, ".json")) return "application/json";
  if(iequals(ext, ".xml"))  return "application/xml";
  if(iequals(ext, ".swf"))  return "application/x-shockwave-flash";
  if(iequals(ext, ".flv"))  return "video/x-flv";
  if(iequals(ext, ".png"))  return "image/png";
  if(iequals(ext, ".jpe"))  return "image/jpeg";
  if(iequals(ext, ".jpeg")) return "image/jpeg";
  if(iequals(ext, ".jpg"))  return "image/jpeg";
  if(iequals(ext, ".gif"))  return "image/gif";
  if(iequals(ext, ".bmp"))  return "image/bmp";
  if(iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
  if(iequals(ext, ".tiff")) return "image/tiff";
  if(iequals(ext, ".tif"))  return "image/tiff";
  if(iequals(ext, ".svg"))  return "image/svg+xml";
  if(iequals(ext, ".svgz")) return "image/svg+xml";
  return "application/text";
}

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
std::string
path_cat(
    beast::string_view base,
    beast::string_view path)
{
  if(base.empty())
    return std::string(path);
  std::string result(base);
#ifdef BOOST_MSVC
  char constexpr path_separator = '\\';
    if(result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
    for(auto& c : result)
        if(c == '/')
            c = path_separator;
#else
  char constexpr path_separator = '/';
  if(result.back() == path_separator)
    result.resize(result.size() - 1);
  result.append(path.data(), path.size());
#endif
  return result;
}

// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
template<
    class Body, class Allocator,
    class Send>
void
handle_request(
    beast::string_view doc_root,
    http::request<Body, http::basic_fields<Allocator>>&& req,
    Send&& send)
{
  // Returns a bad request response
  auto const bad_request =
      [&req](beast::string_view why)
      {
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;
      };

  // Returns a not found response
  auto const not_found =
      [&req](beast::string_view target)
      {
        http::response<http::string_body> res{http::status::not_found, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + std::string(target) + "' was not found.";
        res.prepare_payload();
        return res;
      };

  // Returns a server error response
  auto const server_error =
      [&req](beast::string_view what)
      {
        http::response<http::string_body> res{http::status::internal_server_error, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occurred: '" + std::string(what) + "'";
        res.prepare_payload();
        return res;
      };

  // Make sure we can handle the method
  if( req.method() != http::verb::get &&
      req.method() != http::verb::head)
    return send(bad_request("Unknown HTTP-method"));

  // Request path must be absolute and not contain "..".
  if( req.target().empty() ||
      req.target()[0] != '/' ||
      req.target().find("..") != beast::string_view::npos)
    return send(bad_request("Illegal request-target"));

  // Build the path to the requested file
  std::string path = path_cat(doc_root, req.target());
  if(req.target().back() == '/')
    path.append("index.html");

  // Attempt to open the file
  beast::error_code ec;
  http::file_body::value_type body;
  body.open(path.c_str(), beast::file_mode::scan, ec);

  // Handle the case where the file doesn't exist
  if(ec == beast::errc::no_such_file_or_directory)
    return send(not_found(req.target()));

  // Handle an unknown error
  if(ec)
    return send(server_error(ec.message()));

  // Cache the size since we need it after the move
  auto const size = body.size();

  // Respond to HEAD request
  if(req.method() == http::verb::head)
  {
    http::response<http::empty_body> res{http::status::ok, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, mime_type(path));
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return send(std::move(res));
  }

  // Respond to GET request
  http::response<http::file_body> res{
      std::piecewise_construct,
      std::make_tuple(std::move(body)),
      std::make_tuple(http::status::ok, req.version())};
  res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
  res.set(http::field::content_type, mime_type(path));
  res.content_length(size);
  res.keep_alive(req.keep_alive());
  return send(std::move(res));
}

//------------------------------------------------------------------------------

// Report a failure
void
fail(beast::error_code ec, char const* what)
{
  std::cerr << what << ": " << ec.message() << "\n";
}

// Echoes back all received WebSocket messages
class websocket_session : public std::enable_shared_from_this<websocket_session>
{
  websocket::stream<beast::tcp_stream> ws_;
  beast::flat_buffer buffer_;

public:
  // Take ownership of the socket
  explicit
  websocket_session(tcp::socket&& socket)
      : ws_(std::move(socket))
  {
  }

  // Start the asynchronous accept operation
  template<class Body, class Allocator>
  void
  do_accept(http::request<Body, http::basic_fields<Allocator>> req)
  {
    // Set suggested timeout settings for the websocket
    ws_.set_option(
        websocket::stream_base::timeout::suggested(
            beast::role_type::server));

    // Set a decorator to change the Server of the handshake
    ws_.set_option(websocket::stream_base::decorator(
        [](websocket::response_type& res)
        {
          res.set(http::field::server,
                  std::string(BOOST_BEAST_VERSION_STRING) +
                  " advanced-server");
        }));

    // Accept the websocket handshake
    ws_.async_accept(
        req,
        beast::bind_front_handler(
            &websocket_session::on_accept,
            shared_from_this()));
  }

private:
  void
  on_accept(beast::error_code ec)
  {
    if(ec)
      return fail(ec, "accept");

    // Read a message
    do_read();
  }

  void
  do_read()
  {
    // Read a message into our buffer
    ws_.async_read(
        buffer_,
        beast::bind_front_handler(
            &websocket_session::on_read,
            shared_from_this()));
  }

  void
  on_read(
      beast::error_code ec,
      std::size_t bytes_transferred)
  {
    boost::ignore_unused(bytes_transferred);

    // This indicates that the websocket_session was closed
    if(ec == websocket::error::closed)
      return;

    if(ec)
      fail(ec, "read");

    // Echo the message
    ws_.text(ws_.got_text());
    ws_.async_write(
        buffer_.data(),
        beast::bind_front_handler(
            &websocket_session::on_write,
            shared_from_this()));
  }

  void
  on_write(
      beast::error_code ec,
      std::size_t bytes_transferred)
  {
    boost::ignore_unused(bytes_transferred);

    if(ec)
      return fail(ec, "write");

    // Clear the buffer
    buffer_.consume(buffer_.size());

    // Do another read
    do_read();
  }
};

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------


int main(int argc, char* argv[])
{
  // Check command line arguments.
  if (argc != 5)
  {
    std::cerr <<
              "Usage: advanced-server <address> <port> <doc_root> <threads>\n" <<
              "Example:\n" <<
              "    advanced-server 0.0.0.0 8080 . 1\n";
    return EXIT_FAILURE;
  }
  auto const address = net::ip::make_address(argv[1]);
  auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
  //auto const doc_root = std::make_shared<std::string>(argv[3]);
  auto const threads = std::max<int>(1, std::atoi(argv[4]));

  // The io_context is required for all I/O
  net::io_context ioc{threads};

  // Create and launch a listening port
  std::make_shared<listener>(
      ioc,
      tcp::endpoint{address, port},
      doc_root)->run();

  // Capture SIGINT and SIGTERM to perform a clean shutdown
  net::signal_set signals(ioc, SIGINT, SIGTERM);
  signals.async_wait(
      [&](beast::error_code const&, int)
      {
        // Stop the `io_context`. This will cause `run()`
        // to return immediately, eventually destroying the
        // `io_context` and all of the sockets in it.
        ioc.stop();
      });

  // Run the I/O service on the requested number of threads
  std::vector<std::thread> v;
  v.reserve(threads - 1);
  for(auto i = threads - 1; i > 0; --i)
    v.emplace_back(
        [&ioc]
        {
          ioc.run();
        });
  ioc.run();

  // (If we get here, it means we got a SIGINT or SIGTERM)

  // Block until all the threads exit
  for(auto& t : v)
    t.join();

  return EXIT_SUCCESS;
}