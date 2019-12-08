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
#include "IRunner.h"
#include "Listener.h"

namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace websocket = boost::beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class Runner : public IRunner {
  boost::asio::ip::address const address;
  unsigned short const port;
  int const threads;

 public:
  Runner(net::ip::address address, const unsigned int &port,
         const int &threads);

  void run() override;
};

#endif  // TECHPROJECT_RUNNER_H
