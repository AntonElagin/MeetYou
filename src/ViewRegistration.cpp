#include "ViewRegistration.h"
#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include "Md5.h"

bool ViewRegistration::isPassword(const std::string &value) {
  return value.size() > 6 && value.size() < 20;
}

bool ViewRegistration::isLogin(const std::string &value) {
  return value.size() > 6 && value.size() < 45;
}

bool ViewRegistration::isEmail(const std::string &value) {
  std::regex reg{
      R"(^([a-z0-9_-]+\.)*[a-z0-9_-]+@[a-z0-9_-]+(\.[a-z0-9_-]+)*\.[a-z]{2,6}$)"};
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
  nlohmann::json js = nlohmann::json::parse(req.body());
  if (js.contains("password") && js.contains("login") && js.contains("email")) {
    pas = js.at("password");
    log = js.at("login");
    eml = js.at("email");
    if (isPassword(pas) && isEmail(eml) && isLogin(log)) {
      try {
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

        srand(time(0));
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

      } catch (sql::SQLException &e) {
        std::regex reg{"Duplicate entry '[^']+' for key '[^_]+_UNIQUE'"};
        if (!std::regex_search(e.what(), reg)) {
          res.result(500);
          std::string error =
              "{\n\"error\" : \"500\",\n \"message\" : \"server error\"\n}";
          res.body() = error;
          res.set(http::field::content_length, error.length());
          return res;
        }

        std::cout << "# ERR: " << e.what() << std::endl;
        std::smatch iterator;
        std::regex invalidParam{"'[^_]+_UNIQUE'"};
        std::string a = e.what();
        std::regex_search(a, iterator, invalidParam);
        nlohmann::json body;

        body["login"] = log;
        body["email"] = eml;
        body["error"] =
            "Invalid " + iterator.str().erase(iterator.str().length() - 8,
                                              iterator.str().length() - 1);
        res.body() = body.dump();
        res.set(http::field::content_length, body.dump().size());
        return res;
      } catch (...) {
        std::cout << "Pizdech!" << std::endl;
      }
    }
    nlohmann::json exBody;
    res.result(400);
    exBody["error"] = 400;
    //  exBody["message"] = "JSON error";
    if (!isLogin(log)) {
      exBody["login_message"] = "Invalid";
    }
    if (!isEmail(eml)) {
      exBody["email_message"] = "Invalid";
    }
    if (!isPassword(pas)) {
      exBody["password_message"] = "Invalid";
    }
    std::string exBodyStr = exBody.dump();
    res.set(http::field::content_length, exBodyStr.size());
    res.body() = exBodyStr;
    res.prepare_payload();
    return res;
  }
  nlohmann::json exBody;
  res.result(400);
  exBody["error"] = 400;
  exBody["message"] = "JSON error";
  std::string exBodyStr = exBody.dump();
  res.set(http::field::content_length, exBodyStr.size());
  res.body() = exBodyStr;
  res.prepare_payload();
  return res;
}

http::response<http::string_body> ViewRegistration::get() {
  boost::beast::http::response<http::string_body> res;
  nlohmann::json js = nlohmann::json::parse(req.body());
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
      res.body() = body.dump();
      res.set(http::field::content_length, body.dump().size());
      return res;
    }
    nlohmann::json exBody;
    res.result(400);
    exBody["error"] = 400;
    exBody["message"] = "Invalid user data";
    std::string exBodyStr = exBody.dump();
    res.set(http::field::content_length, exBodyStr.size());
    res.body() = exBodyStr;
    res.prepare_payload();
    return res;
  }
  nlohmann::json exBody;
  res.result(400);
  exBody["error"] = 400;
  exBody["message"] = "JSON error";
  std::string exBodyStr = exBody.dump();
  res.set(http::field::content_length, exBodyStr.size());
  res.body() = exBodyStr;
  res.prepare_payload();
  return res;
}

http::response<http::string_body> ViewRegistration::put() {
  return defaultPlug();
}

ViewRegistration::ViewRegistration(
    const http::request<http::string_body> &_req,
    const std::shared_ptr<sql::Connection> &_conn, int _userId)
    : View(_req, _conn, _userId) {}

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
    str += std::to_string((char)rand() % 256);
  }
  return str;
}

void ViewRegistration::createToken(const std::string &token,
                                   const int &userId) {
  std::unique_ptr<sql::PreparedStatement> tokenStmt(conn->prepareStatement(
      "INSERT  INTO token(user_id, token) VALUES (?,?);"));
  tokenStmt->setInt(1, userId);
  tokenStmt->setString(2, token);
  if (tokenStmt->execute()) throw "server error";
}
