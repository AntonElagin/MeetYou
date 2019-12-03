#ifndef TECHPROJECT_VIEW_H
#define TECHPROJECT_VIEW_H

#include <cppconn/connection.h>
#include <boost/beast.hpp>
#include <utility>

namespace http = boost::beast::http;

// template <class Body, class Allocator>
class View {
 public:
  std::shared_ptr<sql::Connection> conn;
  http::request<http::string_body> req;

  View(http::request<http::string_body> _req,
       std::shared_ptr<sql::Connection> _conn);

  virtual http::response<http::string_body> get() = 0;
  virtual http::response<http::string_body> post() = 0;
  virtual http::response<http::string_body> delete_() = 0;
  virtual http::response<http::string_body> put() = 0;
  virtual ~View() = default;
};

#endif  // TECHPROJECT_VIEW_H
