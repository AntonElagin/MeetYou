#include "../include/view_registration.h"

http::response<http::string_body> ViewRegistration::delete_() {
  boost::beast::http::response<http::string_body> res;

  return res;
}

http::response<http::string_body> ViewRegistration::post() {
  boost::beast::http::response<http::string_body> res;
  //  req.bo
  return res;
}

http::response<http::string_body> ViewRegistration::get() {
  boost::beast::http::response<http::string_body> res;
  nlohmann::json js(this->req.body());
  std::cout << this->req.target() << std::endl;
  return res;
}

http::response<http::string_body> ViewRegistration::put() {
  boost::beast::http::response<http::string_body> res;
  return res;
}

// template <class Body, class Allocator>
// ViewRegistration<Body, Allocator>::ViewRegistration(
//    http::request<Body, http::basic_fields<Allocator>>& _req,
//    const std::shared_ptr<sql::Connection>& _conn)
//    : View<Body, Allocator>(_req, _conn) {}

// template <class Body, class Allocator>
ViewRegistration::ViewRegistration(
    const http::request<http::string_body>& _req,
    const std::shared_ptr<sql::Connection>& _conn)
    : View(_req, _conn) {}
