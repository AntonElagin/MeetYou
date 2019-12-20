#include "ViewOther.h"

ViewOther::ViewOther(const http::request<http::string_body> &_req,
                     const std::shared_ptr<sql::Connection> &_conn, int _userId)
    : View(_req, _conn, _userId) {}

http::response<http::string_body> ViewOther::get() { return badRequestPlug(); }

http::response<http::string_body> ViewOther::post() { return badRequestPlug(); }

http::response<http::string_body> ViewOther::delete_() {
  return badRequestPlug();
}

http::response<http::string_body> ViewOther::put() { return badRequestPlug(); }

http::response<http::string_body> ViewOther::badRequestPlug() {
  http::response<http::string_body> res;
  res.result(404);
  res.set(http::field::content_type, "json/application");
  res.keep_alive(req.keep_alive());
  nlohmann::json body;
  body["status"] = 404;
  body["message"] = "Invalid way";
  std::string s = body.dump();
  res.body() = s;
  res.set(http::field::content_length, s.length());
  return res;
}
