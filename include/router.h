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
#include "auth_middleware.h"
#include "view.h"
#include "view_registration.h"

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
  conn = driver->connect("tcp://127.0.0.1:3306", "root", "12A02El99"));
  conn->setSchema("MeetYou");

  if (req.find("Cookie")) {
    AuthMiddleware authMiddleware(conn, req["Cookie"]);
    authMiddleware.isAuth();
    int userId = authMiddleware.getUserId();
    if (std::regex_search(req.target(), iterator, reg)) {
      std::string path(iterator.str());
      if (req.method_string() == "get") {
        if (authGetMap.find(path) != authGetMap.end()) {
          if (authGetMap[path]) {
          }
        }
        viewRoute(path);
      } else if (req.method_string() == "post") {
        viewRoute(path);
      } else if (req.method_string() == "put") {
        viewRoute(path);
      } else if (req.method_string() == "delete") {
        viewRoute(path);
      } else {
      }
    }
  }
}

template <class Send>
template <class View>
void Router<Send>::viewRoute(const std::string& path) {
  try {
    if (authGetMap.find(path)) {
      AuthMiddleware authMiddleware(conn, req.at("Cookie"));
    }
  } catch (...) {
  }
}

template <class Send>
std::unique_ptr<View> Router<Send>::getView(const std::string& path) {
  if (path == "/auth")
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

#endif  // TECHPROJECT_ROUTER_H
