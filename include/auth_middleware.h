#ifndef TECHPROJECT_AUTH_MIDDLEWARE_H
#define TECHPROJECT_AUTH_MIDDLEWARE_H

#include "auth_middleware_interface.h"
#include <cppconn/statement.h>
#include <boost/beast.hpp>
#include <utility>

class AuthMiddleware : public AuthMiddlewareInterface {
public:

  AuthMiddleware(const boost::beast::http::request<boost::beast::http::dynamic_body> &req);

  bool is_auth() ;

private:
  boost::beast::http::request<boost::beast::http::dynamic_body> req;
  std::shared_ptr<sql::Statement> stmt;


  AuthMiddleware(std::shared_ptr<sql::Statement> _stmt, boost::beast::http::request<boost::beast::http::dynamic_body>& _req);
  unsigned int get_user_id();

  bool is_valid_toke();

  void update_token();
};

bool AuthMiddleware::is_auth() {
//  req.get();

}

AuthMiddleware::AuthMiddleware(std::shared_ptr<sql::Statement> _stmt,
                               boost::beast::http::request<boost::beast::http::dynamic_body> &_req)
                               : req(_req), stmt(std::move(_stmt))
{}

AuthMiddleware::AuthMiddleware(const boost::beast::http::request<boost::beast::http::dynamic_body> &req) : req(req) {}


#endif //TECHPROJECT_AUTH_MIDDLEWARE_H