#include "View.h"

#include <utility>

View::View(http::request<http::string_body> _req,
           std::shared_ptr<sql::Connection> _conn, int _userId)
    : req(std::move(_req)), conn(std::move(_conn)), userId(_userId) {}

http::response<http::string_body> View::defaultPlug() {
  http::response<http::string_body> res;
  res.result(405);
  res.set(http::field::content_type, "json/application");
  res.keep_alive(req.keep_alive());
  nlohmann::json body;
  body["status"] = 405;
  body["message"] = "Invalid method";
  std::string s = body.dump();
  res.body() = s;
  res.set(http::field::content_length, s.length());
  return res;
}
