#include "ViewUserHobby.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

http::response<http::string_body> ViewUserHobby::get() {
  http::response<http::string_body> res;
  res.set(http::field::content_type, "json/application");
  res.keep_alive(req.keep_alive());

  nlohmann::json reqBody;
  try {
    reqBody = nlohmann::json::parse(req.body());
  } catch (...) {
    return templateReturn(400, "Invalid JSON");
  }

  if (!(reqBody.contains("user_id") && reqBody["user_id"].is_number_unsigned()))
    return templateReturn(400, "Invalid params or params count");

  int user = reqBody["user_id"];

  std::unique_ptr<sql::PreparedStatement> hobbyStmt(
      conn->prepareStatement("Select hobby from userhobby where user_id = ?"));
  hobbyStmt->setInt(1, user);
  std::unique_ptr<sql::ResultSet> hobbyRes(hobbyStmt->executeQuery());
  nlohmann::json resBody;
  while (hobbyRes->next()) {
    resBody["hobbies"] += hobbyRes->getString(1);
  }
  if (!resBody.contains("hobbies"))
    resBody["hobbies"] = std::vector<std::string>(0);
  resBody["user_id"] = user;
  std::string body = resBody.dump();
  res.result(200);
  res.body() = body;
  res.set(http::field::content_length, body.length());
  return res;
}

bool ViewUserHobby::isHobby(const std::string &value) {
  return value.length() > 2 && value.length() < 100;
}

http::response<http::string_body> ViewUserHobby::post() {
  http::response<http::string_body> res;
  res.set(http::field::content_type, "json/application");
  res.keep_alive(req.keep_alive());

  nlohmann::json reqBody;
  try {
    reqBody = nlohmann::json::parse(req.body());
  } catch (...) {
    return templateReturn(400, "Invalid JSON");
  }
  bool a = reqBody.contains("hobby");
  bool b = reqBody["hobby"].is_array();
  bool c = reqBody["hobby"].is_string();

  if (!((reqBody.contains("hobby")) &&
        (reqBody["hobby"].is_array() || reqBody["hobby"].is_string())))
    return templateReturn(400, "Invalid params or params count");

  std::unique_ptr<sql::PreparedStatement> hobbyStmt(conn->prepareStatement(
      "INSERT into userhobby(user_id, hobby) Values(?,?);"));
  std::unique_ptr<sql::PreparedStatement> validateStmt(conn->prepareStatement(
      "Select * from userhobby where user_id = ? and hobby = ?;"));
  if (reqBody["hobby"].is_string()) {
    std::string hobby = reqBody["hobby"];
    validateStmt->setInt(1, userId);
    validateStmt->setString(2, hobby);
    std::unique_ptr<sql::ResultSet> valid(validateStmt->executeQuery());
    if (valid->isLast()) {
      hobbyStmt->setString(2, hobby);
      hobbyStmt->setInt(1, userId);
      hobbyStmt->executeQuery();
    }
  } else {
    std::vector<std::string> list = reqBody["hobby"];
    for (const auto &iter : list) {
      validateStmt->setInt(1, userId);
      validateStmt->setString(2, iter);
      std::unique_ptr<sql::ResultSet> valid(validateStmt->executeQuery());
      if (valid->isLast()) {
        hobbyStmt->setString(2, iter);
        hobbyStmt->setInt(1, userId);
        hobbyStmt->executeQuery();
      }
    }
  }
  return templateReturn(200, "OK");
}

http::response<http::string_body> ViewUserHobby::delete_() {
  http::response<http::string_body> res;
  res.set(http::field::content_type, "json/application");
  res.keep_alive(req.keep_alive());

  nlohmann::json reqBody;
  try {
    reqBody = nlohmann::json::parse(req.body());
  } catch (...) {
    return templateReturn(400, "Invalid JSON");
  }

  if (!(reqBody.contains("hobby") &&
        (reqBody["hobby"].is_array() || reqBody["hobby"].is_string())))
    return templateReturn(400, "Invalid params or params count");

  std::unique_ptr<sql::PreparedStatement> hobbyStmt(conn->prepareStatement(
      "DELETE FROM userhobby WHERE user_id = ? and hobby = ?;"));
  if (reqBody["hobby"].is_string()) {
    std::string hobby = reqBody["hobby"];
    hobbyStmt->setString(2, hobby);
    hobbyStmt->setInt(1, userId);
    hobbyStmt->executeQuery();
  } else {
    std::vector<std::string> list = reqBody["hobby"];
    for (const auto &iter : list) {
      hobbyStmt->setString(2, iter);
      hobbyStmt->setInt(1, userId);
      hobbyStmt->executeQuery();
    }
  }
  return templateReturn(200, "OK");
}

http::response<http::string_body> ViewUserHobby::put() { return defaultPlug(); }

ViewUserHobby::ViewUserHobby(const http::request<http::string_body> &_req,
                             const std::shared_ptr<sql::Connection> &_conn,
                             int _userId)
    : View(_req, _conn, _userId) {}
