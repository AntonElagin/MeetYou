//
// Created by anton on 08.11.2019.
//

#ifndef TECHPROJECT_RUNNER_H
#define TECHPROJECT_RUNNER_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "listener.h"

namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace websocket =
    boost::beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;


  template<class T>  // TODO : Нужно  для тестов
  class runner {
    boost::asio::ip::address const address;
    unsigned short const port;
    int const threads;

  public:
    void run();

    runner(net::ip::address address, const unsigned int &port,
           const int &threads);
  };


#endif  // TECHPROJECT_RUNNER_H
