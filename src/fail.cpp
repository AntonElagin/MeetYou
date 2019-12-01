//
// Created by anton on 08.11.2019.
//

#include "../include/fail.h"

void fail(boost::beast::error_code ec, char const* what) {
  std::cerr << what << ": " << ec.message() << "\n";
}