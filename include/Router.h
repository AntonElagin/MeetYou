//
// Created by anton on 03.12.2019.
//

#ifndef TECHPROJECT_ROUTER_H
#define TECHPROJECT_ROUTER_H

#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <boost/beast.hpp>
#include <regex>
#include <unordered_map>
#include "AuthMiddleware.h"
#include "View.h"
#include "ViewRegistration.h"

namespace http = boost::beast::http;

template <class Send>
class Router {
  http::request<http::string_body> req;
  Send send;
  std::shared_ptr<sql::Connection> conn;

  // Таблица для проверки необходимости авторизации при get запросе
  std::unordered_map<std::string, bool> authGetMap;
  template <class View>
  void viewRoute(const std::string& path);

 public:
  void startRouting();
  std::unique_ptr<View> getView(const std::string& path);
  Router(const http::request<http::string_body>& req, Send&& send);

  http::response<http::string_body> authExeption();

  http::response<http::string_body> methodExeption();
};

template <class Send>
Router<Send>::Router(const http::request<http::string_body>& req, Send&& send)
    : req(req), send(send) {
  authGetMap = {{"/auth", false}, {"/user", false}, {"/event", true}};
}

template <class Send>
void Router<Send>::startRouting() {
  std::regex reg{"/[^?]+"};
  std::smatch iterator;

  std::unique_ptr<View> controller;
  std::unique_ptr<sql::Driver> driver(get_driver_instance());
  conn = driver->connect("tcp://127.0.0.1:3306", "root", "12A02El99");
  conn->setSchema("MeetYou");

  AuthMiddleware authMiddleware(conn, req);
  bool authFlag = authMiddleware.isAuth();
  int userId = authMiddleware.getUserId();

  if (std::regex_search(req.target().to_string(), iterator, reg)) {
    std::string path(iterator.str());
    if (req.method_string() == "get") {
      controller = viewRoute(path);
      return send(controller.get());
    } else if (req.method_string() == "post" && authFlag) {
      controller = viewRoute(path);
    } else if (req.method_string() == "put" && authFlag) {
      controller = viewRoute(path);
    } else if (req.method_string() == "delete" && authFlag) {
      controller = viewRoute(path);
    }
    if (!authFlag) {
      return send(authExeption());
    }
    return send(methodExeption());
  }
}

template <class Send>
template <class View>
void Router<Send>::viewRoute(const std::string& path) {}

template <class Send>
std::unique_ptr<View> Router<Send>::getView(const std::string& path) {
  if (path == "/auth" || path == "/")
    return std::unique_ptr<ViewRegistration>();
  else if (path == "/user")
    return std::unique_ptr<View>();
  else if (path == "/event")
    return std::unique_ptr<View>();
  else if (path == "/chat")
    return std::unique_ptr<View>();
  else if (path == "/hobby")
    return std::unique_ptr<View>();
  else if (path == "/other")
    return std::unique_ptr<View>();
  else
    return std::unique_ptr<View>();
}

template <class Send>
http::response<http::string_body> Router<Send>::authExeption() {
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

template <class Send>
http::response<http::string_body> Router<Send>::methodExeption() {
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

#endif  // TECHPROJECT_ROUTER_H
