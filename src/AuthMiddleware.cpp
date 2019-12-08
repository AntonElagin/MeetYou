#include "AuthMiddleware.h"

#include <utility>

bool AuthMiddleware::isAuth() {
  try {
    std::string c = req.at("Cookie").to_string(), token;
    std::smatch iterator;
    std::regex rex("access_token=[^;]+");

    if (std::regex_search(c, iterator, rex)) {
      token = iterator.str().erase(0, 13);
      std::unique_ptr<sql::PreparedStatement> st(
          conn->prepareStatement("SELECT * FROM token WHERE token =?"));
      st->setString(1, token);
      std::cout << std::endl << "~~~~~~~" << std::endl;
      std::cout << iterator.str() << "\n" << token << std::endl;

      std::shared_ptr<sql::ResultSet> result(st->executeQuery());
      while (result->next()) {
        authUser = result->getInt("user_id");
        return true;
      }
    }
  } catch (std::out_of_range) {
  } catch (sql::SQLException &e) {
    std::cout << e.what() << std::endl;
  }
  return false;
}

AuthMiddleware::AuthMiddleware(
    std::shared_ptr<sql::Connection> _conn,
    const boost::beast::http::request<boost::beast::http::string_body> &_req)
    : conn(std::move(_conn)), req(_req), authUser(-1) {}

int AuthMiddleware::getUserId() { return authUser; }
