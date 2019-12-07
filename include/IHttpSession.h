//
// Created by anton on 16.11.2019.
//

#ifndef TECHPROJECT_IHTTPSESSION_H
#define TECHPROJECT_IHTTPSESSION_H

#include <boost/beast.hpp>

namespace beast = boost::beast;

class IHttpSession {
  virtual void run() = 0;
  virtual void onWrite(bool close, beast::error_code ec,
                       std::size_t bytes_transferred) = 0;
};

#endif  // TECHPROJECT_IHTTPSESSION_H
