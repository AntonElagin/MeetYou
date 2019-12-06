#ifndef TECHPROJECT_AUTHMIDDLEWARE_H
#define TECHPROJECT_AUTHMIDDLEWARE_H

#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/statement.h>
#include <boost/beast.hpp>
#include <regex>
#include <utility>
#include "IAuthMiddleware.h"
#include "Md5.h"

class AuthMiddleware : public IAuthMiddleware {
 public:
  AuthMiddleware(
      std::shared_ptr<sql::Connection> _conn,
      const boost::beast::http::request<boost::beast::http::string_body> &_req);
  bool isAuth() override;
  int getUserId();

 private:
  int authUser;
  boost::beast::http::request<boost::beast::http::string_body> req;
  std::shared_ptr<sql::Connection> conn;
};

#endif  // TECHPROJECT_AUTHMIDDLEWARE_H