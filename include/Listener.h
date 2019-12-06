#ifndef TECHPROJECT_LISTENER_H
#define TECHPROJECT_LISTENER_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "HttpSession.h"
#include "IListener.h"
//#include "fail.h"
namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace websocket = boost::beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

//------------------------------------------------------------------------------

// Принимает входящие соединения и запускает сеансы
class Listener : public std::enable_shared_from_this<Listener>, IListener {
  net::io_context &ioc;
  tcp::acceptor acceptor;

 public:
  Listener(net::io_context &_ioc, tcp::endpoint _endpoint);

  void run() override;

 private:
  void doAccept();

  void onAccept(beast::error_code ec, tcp::socket socket);
};

//------------------------------------------------------------------------------

#endif  // TECHPROJECT_LISTENER_H
