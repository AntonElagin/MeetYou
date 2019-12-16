#include "ViewRegistration.h"
#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

#include <utility>
#include "Md5.h"

bool ViewRegistration::isPassword(const std::string &value) {
  return value.size() > 6 && value.size() < 20;
}

bool ViewRegistration::isLogin(const std::string &value) {
  return value.size() > 6 && value.size() < 45;
}

bool ViewRegistration::isEmail(const std::string &value) {
//  TODO : Проверить регулярку(Возможно отрабатывает не все email)
  std::regex reg{
      R"(^([A-Za-z0-9_-]+\.)*[a-z0-9_-]+@[a-z0-9_-]+(\.[a-z0-9_-]+)*\.[a-z]{2,6}$)"};
  return (value.size() > 6) && (value.size() < 45) &&
         std::regex_search(value, reg);
}

http::response<http::string_body> ViewRegistration::delete_() {
  return defaultPlug();
}

http::response<http::string_body> ViewRegistration::post() {
  boost::beast::http::response<http::string_body> res;
  //  res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
  res.set(http::field::content_type, "json/application");
  res.keep_alive(req.keep_alive());
  std::string pas, log, eml;
  //  Парсинг body
  nlohmann::json js;
  try {
    js = nlohmann::json::parse(req.body());
  } catch (nlohmann::json::parse_error &e) {
    return templateReturn(400, "JSON error");
  }

  if (js.contains("password") && js.contains("login") && js.contains("email")) {
    pas = js.at("password");
    log = js.at("login");
    eml = js.at("email");
    if (isPassword(pas) && isEmail(eml) && isLogin(log)) {
        switch (isDuplicate(log, eml)) {
          case 1:
            return templateReturn(400, "Duplicate login");
          case 2:
            return templateReturn(400, "Duplicate email");
          case 3:
            return templateReturn(400, "Duplicate login and email");
          case 0:

            // Создание пользователя
            std::unique_ptr<sql::PreparedStatement> userStmt(conn->prepareStatement(
                "INSERT  INTO user(email, login, password) VALUES (?,?,?);"));
            userStmt->setString(3, md5(pas));
            userStmt->setString(2, log);
            userStmt->setString(1, eml);
            if (userStmt->execute()) throw "server error";

            //      Получаем  Id нового пользователя
            std::unique_ptr<sql::PreparedStatement> userIdStmt(
                conn->prepareStatement("Select id From user where login = ?;"));
            userIdStmt->setString(1, log);
            std::unique_ptr<sql::ResultSet> user(userIdStmt->executeQuery());
            int user_id = -1;
            while (user->next()) {
              std::cout << user->getInt(1);
              user_id = user->getInt(1);
            }

            pas += randomString(20);
            pas = md5(pas);

            //        Создаем токен сессии для нового пользователя
            createToken(pas, user_id);
            res.set(http::field::set_cookie, "access_token=" + pas + ";path=/");

            nlohmann::json body;
            body["status"] = 200;
            body["message"] = "OK";
            body["access_token"] = md5(pas);
            res.body() = body.dump();
            res.set(http::field::content_length, body.dump().size());
            return res;
        }
      }

    if (!isLogin(log)) {
      return templateReturn(400, "Invalid login");
    }
    if (!isEmail(eml)) {
      return templateReturn(400, "Invalid email");
    }
    if (!isPassword(pas)) {
      return templateReturn(400, "Invalid password");
    }
  }
  return templateReturn(400, "Invalid params or params count");;
}

http::response<http::string_body> ViewRegistration::get() {
  boost::beast::http::response<http::string_body> res;
  nlohmann::json js;
  try {
    js = nlohmann::json::parse(req.body());
  } catch (nlohmann::json::parse_error &e) {
    return templateReturn(400, "JSON error");
  }
  int userId = -1;
  if (js.contains("password") && js.contains("login")) {
    std::string password = js["password"];
    std::string login = js["login"];
    userId = returnUser(password, login);
    if (userId != -1) {
      std::string token(md5(randomString(40)));

      createToken(token, userId);
      res.set(http::field::set_cookie, "access_token=" + token + ";path=/");

      nlohmann::json body;
      body["status"] = 200;
      body["message"] = "OK";
      body["access_token"] = token;
      res.result(200);
      res.body() = body.dump();
      res.set(http::field::content_length, body.dump().size());
      return res;
    }
    return templateReturn(400, "Invalid user data");
  }
  return templateReturn(400, "JSON error");
}

http::response<http::string_body> ViewRegistration::put() {
  return defaultPlug();
}

ViewRegistration::ViewRegistration(const http::request<http::string_body> &_req,
                                   const std::shared_ptr<sql::Connection> &_conn,
                                   int _userId, std::string _ip)
    : View(_req, _conn, _userId), ip(std::move(_ip)) {}

int ViewRegistration::returnUser(const std::string &password,
                                 const std::string &value) {
  if (isEmail(value)) {
    std::unique_ptr<sql::PreparedStatement> userStmt(conn->prepareStatement(
        "SELECT id FROM user where email = ? AND password = ?;"));
    userStmt->setString(2, md5(password));
    userStmt->setString(1, value);

    std::unique_ptr<sql::ResultSet> user(userStmt->executeQuery());
    while (user->next()) {
      return user->getInt(1);
    }

  } else {
    std::unique_ptr<sql::PreparedStatement> userStmt(conn->prepareStatement(
        "SELECT id FROM user where login = ? AND password = ?;"));
    userStmt->setString(2, md5(password));
    userStmt->setString(1, value);

    std::unique_ptr<sql::ResultSet> user(userStmt->executeQuery());
    while (user->next()) {
      return user->getInt(1);
    }
  }
  return -1;
}

std::string ViewRegistration::randomString(int size) {
  std::string str;
  srand(time(0));
  for (int i = 0; i < size; ++i) {
    str += std::to_string((char) rand() % 256);
  }
  return str;
}

void ViewRegistration::createToken(const std::string &token,
                                   const int &userId) {
  try {
    std::unique_ptr<sql::PreparedStatement> tokenStmt(conn->prepareStatement(
        "INSERT  INTO token(user_id, token, ip) VALUES (?, ?, ?);"));
    tokenStmt->setInt(1, userId);
    tokenStmt->setString(2, token);
    tokenStmt->setString(3, md5(ip));
    if (tokenStmt->execute()) throw "server error";
  } catch (sql::SQLException &e) {
    std::cout << e.what() << std::endl;
  }

}

int ViewRegistration::isDuplicate(const std::string &login, const std::string &email) {
  int res = 0;
  std::unique_ptr<sql::PreparedStatement> loginDuplicateStmt(
      conn->prepareStatement("Select * From user where login = ?;"));
  loginDuplicateStmt->setString(1, login);
  std::unique_ptr<sql::ResultSet> result1(loginDuplicateStmt->executeQuery());
  if (result1->next())
    res += 1;
  std::unique_ptr<sql::PreparedStatement> emailDuplicateStmt(
      conn->prepareStatement("Select * From user where email = ?;"));
  emailDuplicateStmt->setString(1, email);
  std::unique_ptr<sql::ResultSet> result2(emailDuplicateStmt->executeQuery());
  if (result2->next())
    res += 2;
  return res;
}
