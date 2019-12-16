#ifndef TECHPROJECT_ROUTER_H
#define TECHPROJECT_ROUTER_H

#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <boost/beast.hpp>
#include <regex>
#include <set>
#include <unordered_map>
#include <utility>
#include "AuthMiddleware.h"
#include "View.h"
#include "ViewOther.h"
#include "ViewRegistration.h"
#include "ViewUser.h"
#include "ViewEvent.h"
#include "ViewEventFollow.h"
#include "ViewUserFollow.h"

namespace http = boost::beast::http;

class Router {
  http::request<http::string_body> req;
  std::shared_ptr<sql::Connection> conn;
  sql::Driver* driver;
  std::string ip;
  int userId;
  // Таблица для проверки необходимости авторизации при get запросе
  std::unordered_map<std::string, bool> authGetMap;

  std::unique_ptr<View> getView(const std::string& path);

  http::response<http::string_body> authException();

  http::response<http::string_body> methodException();

 public:
  template <class Send>
  void startRouting(Send &&send);

  explicit Router(http::request<http::string_body> req, const std::string &ip);
};

template <class Send>
void Router::startRouting(Send &&send) {
try {
  std::regex reg{"/[^?]*"};
  std::smatch iterator;

  std::unique_ptr<View> controller;
  AuthMiddleware authMiddleware(conn, req, ip);
  bool authFlag = authMiddleware.isAuth();
  userId = authMiddleware.getUserId();
  std::string target = req.target().to_string();
  if (std::regex_search(target, iterator, reg)) {
    std::string path(iterator.str());
    std::cout << req.method_string() << std::endl;
    if (req.method_string() == "GET") {
      if (authGetMap.find(path) != authGetMap.end())
        if (authGetMap[path]) {
          if (!authFlag) return send(authException());
        }
      controller = getView(path);
      return send(std::move(controller->get()));
    } else if (req.method_string() == "POST" &&
               (authFlag || path == "/auth" || path == "/auth" ||
                authGetMap.find(path) == authGetMap.end())) {
      controller = getView(path);
      return send(std::move(controller->post()));
    } else if (req.method_string() == "PUT" &&
               (authFlag || authGetMap.find(path) == authGetMap.end())) {
      controller = getView(path);
      return send(std::move(controller->put()));
    } else if (req.method_string() == "DELETE" &&
               (authFlag || authGetMap.find(path) == authGetMap.end())) {
      controller = getView(path);
      return send(std::move(controller->delete_()));
    }
    if (!authFlag && authGetMap.find(path) != authGetMap.end()) {
      return send(std::move(authException()));
    }
    return send(std::move(methodException()));
  }
} catch(...) {
  http::response<http::string_body> res;
  res.set(http::field::content_type, "application/json");
  res.result(500);
  nlohmann::json respBody;
  respBody["status"] = 500;
  respBody["message"] = "server error (routing)";
  res.body() = respBody.dump();
  res.set(http::field::content_length, respBody.dump().length());
  send(std::move(res));
  }
}

#endif  // TECHPROJECT_ROUTER_H
