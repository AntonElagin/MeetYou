#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "../include/runner.h"
#include <mysql/mysql.h>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <mysql_error.h>



int main(int argc, char* argv[]) {



  if (argc != 4) {
    std::cerr << "Somethin went wrong\n"
              << "Example:\n"
              << "     127.0.0.1 8080 10\n";
    return EXIT_FAILURE;
  }
  boost::asio::ip::address address = net::ip::make_address(argv[1]);
  auto port = static_cast<unsigned short>(std::atoi(argv[2]));
  int threads = std::max<int>(1, std::atoi(argv[3]));

  runner  serv(address, port, threads);
  serv.run();


  return EXIT_SUCCESS;
}