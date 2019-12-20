#ifndef BOOST_BEAST_EXAMPLE_WEBSOCKET_CHAT_MULTI_WEBSOCKET_SESSION_HPP
#define BOOST_BEAST_EXAMPLE_WEBSOCKET_CHAT_MULTI_WEBSOCKET_SESSION_HPP

#include "shared_state.hpp"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include "View.h"
#include <cppconn/driver.h>
#include <sstream>
#include "User.h"
namespace beast = boost::beast;                 // from <boost/beast.hpp>
namespace http = beast::http;                   // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;         // from <boost/beast/websocket.hpp>
namespace net = boost::asio;                    // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>

class shared_state;

/** Represents an active WebSocket connection to the server
*/
class websocket_session : public boost::enable_shared_from_this<websocket_session> {
    beast::flat_buffer buffer_;
    std::shared_ptr<sql::Connection> conn;
    websocket::stream<beast::tcp_stream> ws_;
    boost::shared_ptr<shared_state> state_;
    std::vector<boost::shared_ptr<std::string const>> queue_;
    int chatid;
    User user;

    void fail(beast::error_code ec, char const *what);

    void on_accept(beast::error_code ec);

    void on_read(beast::error_code ec, std::size_t bytes_transferred);

    void on_write(beast::error_code ec, std::size_t bytes_transferred);

public:
    websocket_session(tcp::socket &&socket, boost::shared_ptr<shared_state> const &state,
                      std::shared_ptr<sql::Connection> conn, int chatid,User );

    ~websocket_session();

    template<class Body, class Allocator>
    void run(http::request<Body, http::basic_fields<Allocator>> req);

    // Send a message
    void send(boost::shared_ptr<std::string const> const &ss);


    void save_message();

private:
    void on_send(boost::shared_ptr<std::string const> const &ss);
};

template<class Body, class Allocator>
void websocket_session::run(http::request<Body, http::basic_fields<Allocator>> req) {
    // Set suggested timeout settings for the websocket
    ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));
    // Accept the websocket handshake
    ws_.async_accept(req, beast::bind_front_handler(&websocket_session::on_accept, shared_from_this()));
}


#endif