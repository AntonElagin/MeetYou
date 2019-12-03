#ifndef TECHPROJECT_AUTH_MIDDLEWARE_H
#define TECHPROJECT_AUTH_MIDDLEWARE_H

#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/statement.h>
#include <boost/beast.hpp>
#include <regex>
#include <utility>
#include "auth_middleware_interface.h"
#include "md5.h"

//#include <char_traits>
class AuthMiddleware : public AuthMiddlewareInterface {
 public:
  AuthMiddleware(
      std::shared_ptr<sql::Statement>& _stmt,
      boost::beast::http::basic_string_body<char, std::char_traits<char>>&
          _req);
  AuthMiddleware(
      std::shared_ptr<sql::Statement>& _stmt,
      boost::beast::http::request<boost::beast::http::dynamic_body>& _req);
  AuthMiddleware(std::shared_ptr<sql::Statement>& _stmt,
                 boost::string_view& _req);
  AuthMiddleware(std::shared_ptr<sql::Connection> _conn,
                 boost::string_view& _req);
  bool is_Auth() override;

 private:
  int authUser;
  boost::string_view cookies;
  std::shared_ptr<sql::Connection> conn;
  boost::beast::http::string_body cookie;
  boost::beast::http::request<boost::beast::http::dynamic_body> req;
  std::shared_ptr<sql::Statement> stmt;

  int get_user_id();
};

#endif  // TECHPROJECT_AUTH_MIDDLEWARE_H