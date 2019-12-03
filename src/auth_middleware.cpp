#include "../include/auth_middleware.h"


bool AuthMiddleware::is_Auth() {


  std::string c = cookies.to_string(), token;
  std::smatch iterator;
  std::regex rex( "access_token=[^;]+" );

  if (std::regex_search(c, iterator, rex)) {
    token = iterator.str().erase(0,13);
    std::unique_ptr<sql::PreparedStatement> st(conn->prepareStatement("SELECT * FROM token WHERE token =?"));
    st->setString(1, token);
    std::cout << std::endl << "~~~~~~~" << std::endl;
    std::cout << iterator.str() << "\n" << token << std::endl;

    std::shared_ptr<sql::ResultSet> result(st->executeQuery());
    while(result->next()) {
      authUser = result->getInt('user_id');
      return true;
    }
  }
  return false;
}

AuthMiddleware::AuthMiddleware(std::shared_ptr<sql::Statement>&  _stmt, boost::beast::http::basic_string_body<char, std::char_traits<char>>& _req)
    : cookie(_req), stmt(_stmt), authUser(-1)
{}

AuthMiddleware::AuthMiddleware(std::shared_ptr<sql::Statement>&  _stmt, boost::beast::http::request<boost::beast::http::dynamic_body>& _req)
    : req(_req), stmt(_stmt), authUser(-1)
{}

AuthMiddleware::AuthMiddleware(std::shared_ptr<sql::Statement>&  _stmt, boost::string_view& _req)
    : cookies(_req), stmt(_stmt), authUser(-1)
{}

AuthMiddleware::AuthMiddleware(std::shared_ptr<sql::Connection> _conn, boost::string_view& _req) :
    conn(_conn), cookies(_req), authUser(-1)
{
}
