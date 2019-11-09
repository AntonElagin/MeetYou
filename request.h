#ifndef TECHPROJECT_REQUEST_H
#define TECHPROJECT_REQUEST_H

#include <boost/beast.hpp>

namespace http = boost::beast::http;

template<class Body, class Allocator>
class request {
  http::request<Body, http::basic_fields<Allocator>> req;
public:

  virtual http::response<http::string_body> get() = 0;
  virtual http::response<http::string_body> post() = 0;
  virtual http::response<http::string_body> delete_() = 0;
  virtual http::response<http::string_body> put() = 0;
  virtual ~request() = default;
};

template<class Body, class Allocator>
class auth_request: public request<Body,Allocator> {
unsigned int user_id = 0;
public:
  explicit auth_request(http::request<Body, http::basic_fields<Allocator>> req);
  unsigned int get_user_id();
  bool is_auth();
  virtual http::response<http::string_body> get();
  virtual http::response<http::string_body> post();
  virtual http::response<http::string_body> delete_();
  virtual http::response<http::string_body> put();

private:
  bool is_valid_toke();
  void update_token();
};




#endif  // TECHPROJECT_REQUEST_H
