//
// Created by anton on 08.11.2019.
//

#ifndef TECHPROJECT_LISTENER_H
#define TECHPROJECT_LISTENER_H

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include "http_session.h"
//#include "fail.h"
namespace beast = boost::beast;                 // from <boost/beast.hpp>
namespace http = boost::beast::http;                   // from <boost/beast/http.hpp>
namespace websocket = boost::beast::websocket;         // from <boost/beast/websocket.hpp>
namespace net = boost::asio;                    // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>



//------------------------------------------------------------------------------

// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener>
{
    net::io_context& ioc;
    tcp::acceptor acceptor;
public:
    listener(net::io_context& _ioc, tcp::endpoint _endpoint);
    // Start accepting incoming connections
    void run();
private:
    void do_accept();
    void on_accept(beast::error_code ec, tcp::socket socket);
};

//------------------------------------------------------------------------------



#endif //TECHPROJECT_LISTENER_H
