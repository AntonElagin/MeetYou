//
// Created by anton on 08.11.2019.
//

#ifndef TECHPROJECT_RUNNER_H
#define TECHPROJECT_RUNNER_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "listener.h"

namespace beast = boost::beast;                 // from <boost/beast.hpp>
namespace http = boost::beast::http;                   // from <boost/beast/http.hpp>
namespace websocket = boost::beast::websocket;         // from <boost/beast/websocket.hpp>
namespace net = boost::asio;                    // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>



class runner {
    boost::asio::ip::address const address;
    unsigned short const port;
    int const threads;

public:
    void run();
    runner(const net::ip::address& address, const unsigned int& port,const  int& threads);
};


#endif //TECHPROJECT_RUNNER_H
