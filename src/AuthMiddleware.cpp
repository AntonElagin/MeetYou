#include "AuthMiddleware.h"

#include <utility>
#include <boost/beast/http/field.hpp>
#include <boost/beast/http/impl/field.ipp>

namespace http = boost::beast::http;
bool AuthMiddleware::isAuth() {
  try {
    auto b = req.at(http::field::cookie);
    std::string c = b.to_string(), token;
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
        // Добавил проверку ip, с которого пришел запрос
        if (result->getString("ip") != md5(ip)) return false;
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
    boost::beast::http::request<boost::beast::http::string_body> _req,
    const std::string &_ip)
    : conn(std::move(_conn)), req(std::move(_req)), authUser(-1), ip(_ip) {}

int AuthMiddleware::getUserId() { return authUser; }
