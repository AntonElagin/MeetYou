#ifndef TECHPROJECT_VIEW_INTERFACE_H
#define TECHPROJECT_VIEW_INTERFACE_H

#include <boost/beast.hpp>

namespace http = boost::beast::http;

template<class Body, class Allocator>
class ViewInterface {
  http::request<Body, http::basic_fields<Allocator>> req;
public:

  virtual http::response<http::string_body> get() = 0;

  virtual http::response<http::string_body> post() = 0;

  virtual http::response<http::string_body> delete_() = 0;

  virtual http::response<http::string_body> put() = 0;

  virtual ~ViewInterface()() = default;
};


#endif  //TECHPROJECT_VIEW_INTERFACE_H
