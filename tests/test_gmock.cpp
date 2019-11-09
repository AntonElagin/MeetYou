//#include "gmock/http_session.h"
#include "gmock/listener.h"
#include "../runner.h"
#include <boost/asio.hpp>
#include "gmock/http_session.h"


TEST(http_session, do_read) {

  boost::asio::io_service service;
  boost::asio::ip::tcp::socket socket(service);2
  EXPECT_CALL(mock_http_session, run())
      .Times(1);

  mock_http_session http(socket);
  EXPECT_TRUE(http.run());
}


TEST(mock_listener, do_read) {
  EXPECT_CALL(mock_listener, run())
  .Times(1);
  boost::asio::ip::address address = boost::asio::ip::make_address("127.0.0.1");
  auto port = static_cast<unsigned short>(std::atoi("8800"));
  int threads = std::max<int>(1, std::atoi("1"));

  runner<mock_listener> serv(address, port, threads);
  serv.run();
}