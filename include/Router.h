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
#include <utility>
#include "AuthMiddleware.h"
#include "View.h"
#include "ViewRegistration.h"

namespace http = boost::beast::http;

class Router {
  http::request<http::string_body> req;
  std::shared_ptr<sql::Connection> conn;
  sql::Driver* driver;
  int userId;
  // Таблица для проверки необходимости авторизации при get запросе
  std::unordered_map<std::string, bool> authGetMap;

  std::unique_ptr<View> getView(const std::string& path);

  http::response<http::string_body> authException();

  http::response<http::string_body> methodException();

 public:
  template <class Send>
  void startRouting(Send&& send);

  explicit Router(http::request<http::string_body> req);
};

template <class Send>
void Router::startRouting(Send&& send) {
  std::regex reg{"/[^?]+"};
  std::smatch iterator;

  std::unique_ptr<View> controller;
  AuthMiddleware authMiddleware(conn, req);
  bool authFlag = authMiddleware.isAuth();
  userId = authMiddleware.getUserId();
  std::string target = req.target().to_string();
  if (std::regex_search(target, iterator, reg)) {
    std::string path(iterator.str());
    if (req.method_string() == "get") {
      if (authGetMap[path]) {
        if (!authFlag) return send(authException());
      }
      controller = getView(path);
      return send(std::move(controller->get()));
    } else if (req.method_string() == "post" && authFlag) {
      controller = getView(path);
      return send(std::move(controller->post()));
    } else if (req.method_string() == "put" && authFlag) {
      controller = getView(path);
      return send(std::move(controller->put()));
    } else if (req.method_string() == "delete" && authFlag) {
      controller = getView(path);
      return send(std::move(controller->delete_()));
    }
    if (!authFlag) {
      return send(std::move(authException()));
    }
    return send(std::move(methodException()));
  }
}

#endif  // TECHPROJECT_ROUTER_H
