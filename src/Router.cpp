#include "Router.h"

Router::Router(http::request<http::string_body> req)
    : req(std::move(req)), userId(-1) {
  authGetMap = {{"/auth", false}, {"/user", false}, {"/event", true}};
  driver = get_driver_instance();
  std::shared_ptr<sql::Connection> con(
      driver->connect("tcp://127.0.0.1:3306", "root", "12A02El99"));
  con->setSchema("MeetYou");
  conn = std::move(con);
}

// template <class Send>
// void Router::startRouting(Send&& send) {
//  std::regex reg{"/[^?]+"};
//  std::smatch iterator;
//
//  std::unique_ptr<View> controller;
//  AuthMiddleware authMiddleware(conn, req);
//  bool authFlag = authMiddleware.isAuth();
//  userId = authMiddleware.getUserId();
//  std::string target = req.target().to_string();
//  if (std::regex_search(target, iterator, reg)) {
//    std::string path(iterator.str());
//    if (req.method_string() == "get") {
//      if (authGetMap[path]) {
//        if (!authFlag) return send(authException());
//      }
//      controller = getView(path);
//      return send(std::move(controller->get()));
//    } else if (req.method_string() == "post" && authFlag) {
//      controller = getView(path);
//      return send(std::move(controller->post()));
//    } else if (req.method_string() == "put" && authFlag) {
//      controller = getView(path);
//      return send(std::move(controller->put()));
//    } else if (req.method_string() == "delete" && authFlag) {
//      controller = getView(path);
//      return send(std::move(controller->delete_()));
//    }
//    if (!authFlag) {
//      return send(std::move(authException()));
//    }
//    return send(std::move(methodException()));
//  }
//}

std::unique_ptr<View> Router::getView(const std::string& path) {
  if (path == "/auth" || path == "/")
    return std::unique_ptr<View>(new ViewRegistration(req, conn, userId));
  else if (path == "/user")
    return std::unique_ptr<View>(new ViewRegistration(req, conn, userId));
  else if (path == "/event")
    return std::unique_ptr<View>(new ViewRegistration(req, conn, userId));
  else if (path == "/chat")
    return std::unique_ptr<View>(new ViewRegistration(req, conn, userId));
  else if (path == "/hobby")
    return std::unique_ptr<View>(new ViewRegistration(req, conn, userId));
  else if (path == "/other")
    return std::unique_ptr<View>(new ViewRegistration(req, conn, userId));
  else
    return std::unique_ptr<View>(new ViewRegistration(req, conn, userId));
}

http::response<http::string_body> Router::authException() {
  http::response<http::string_body> res;
  res.result(401);
  res.set(http::field::content_type, "json/application");
  res.keep_alive(req.keep_alive());
  nlohmann::json body;
  body["status"] = 401;
  body["message"] = "Please auth!";
  body["redirect_url"] = "/auth";
  std::string s = body.dump();
  res.body() = s;
  res.set(http::field::content_length, s.length());
  return res;
}

http::response<http::string_body> Router::methodException() {
  http::response<http::string_body> res;
  res.result(405);
  res.set(http::field::content_type, "json/application");
  res.keep_alive(req.keep_alive());
  nlohmann::json body;
  body["status"] = 401;
  body["message"] = "Invalid method";
  body["methods"] += "get";
  body["methods"] += "post";
  body["methods"] += "delete";
  body["methods"] += "put";
  std::string s = body.dump();
  res.body() = s;
  res.set(http::field::content_length, s.length());
  return res;
}
