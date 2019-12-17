#include "ViewEventHobby.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>


http::response<http::string_body> ViewEventHobby::get() {
  http::response<http::string_body> res;
  res.set(http::field::content_type, "json/application");
  res.keep_alive(req.keep_alive());

  nlohmann::json reqBody;
  try {
    reqBody = nlohmann::json::parse(req.body());
  } catch (...) {
    return templateReturn(400, "Invalid JSON");
  }

  if (!(reqBody.contains("event_id") && reqBody["event_id"].is_number_unsigned()))
    return templateReturn(400, "Invalid params or params count");

  int event = reqBody["event_id"];

  std::unique_ptr<sql::PreparedStatement> hobbyStmt(
      conn->prepareStatement("Select hobby from eventhobby where event_id = ?"));
  hobbyStmt->setInt(1, event);
  std::unique_ptr<sql::ResultSet> hobbyRes(hobbyStmt->executeQuery());
  nlohmann::json resBody;
  while (hobbyRes->next()) {
    resBody["hobbies"] += hobbyRes->getString(1);
  }
  if (!resBody.contains("hobbies"))
    resBody["hobbies"] = nullptr;
  resBody["event_id"] = event;
  std::string body = resBody.dump();
  res.result(200);
  res.body() = body;
  res.set(http::field::content_length, body.length());
  return res;
}

bool ViewEventHobby::isHobby(const std::string &value) {
  return value.length() > 2 && value.length() < 100;
}

http::response<http::string_body> ViewEventHobby::post() {
  http::response<http::string_body> res;
  res.set(http::field::content_type, "json/application");
  res.keep_alive(req.keep_alive());

  nlohmann::json reqBody;
  try {
    reqBody = nlohmann::json::parse(req.body());
  } catch (...) {
    return templateReturn(400, "Invalid JSON");
  }

  if (!(reqBody.contains("hobby") && (reqBody["hobby"].is_array() || reqBody["hobby"].is_string())) &&
      reqBody.contains("event_id") && reqBody["event_id"].is_number_unsigned())
    return templateReturn(400, "Invalid params or params count");

  int event = reqBody["event_id"];
  std::unique_ptr<sql::PreparedStatement> eventStmt(
      conn->prepareStatement("Select * from event where admin = ? and id = ?"));
  eventStmt->setInt(2, event);
  eventStmt->setInt(1, userId);
  std::unique_ptr<sql::ResultSet> eventRes(eventStmt->executeQuery());

  if (!eventRes->next())
    return templateReturn(401, "Access denied");

  std::unique_ptr<sql::PreparedStatement> hobbyStmt(
      conn->prepareStatement("INSERT into eventhobby(event_id,hobby) Values(?,?);"));
  std::unique_ptr<sql::PreparedStatement> validateStmt(
      conn->prepareStatement("Select * from eventhobby where event_id = ? and hobby = ?;"));
  if (reqBody["hobby"].is_string()) {
    std::string hobby = reqBody["hobby"];
    validateStmt->setInt(1, event);
    validateStmt->setString(2, hobby);
    std::unique_ptr<sql::ResultSet> valid(validateStmt->executeQuery());
    if (valid->isLast()) {
      hobbyStmt->setString(2, hobby);
      hobbyStmt->setInt(1, event);
      hobbyStmt->executeQuery();
    }
  } else {
    std::vector<std::string> list = reqBody["hobby"];
    for (const auto &iter: list) {
      validateStmt->setInt(1, event);
      validateStmt->setString(2, iter);
      std::unique_ptr<sql::ResultSet> valid(validateStmt->executeQuery());
      if (valid->isLast()) {
        hobbyStmt->setString(2, iter);
        hobbyStmt->setInt(1, event);
        hobbyStmt->executeQuery();
      }
    }
  }
  return templateReturn(200, "OK");
}

http::response<http::string_body> ViewEventHobby::delete_() {
  http::response<http::string_body> res;
  res.set(http::field::content_type, "json/application");
  res.keep_alive(req.keep_alive());

  nlohmann::json reqBody;
  try {
    reqBody = nlohmann::json::parse(req.body());
  } catch (...) {
    return templateReturn(400, "Invalid JSON");
  }


  if (!(reqBody.contains("hobby") && (reqBody["hobby"].is_array() || reqBody["hobby"].is_string())) &&
      reqBody.contains("event_id") && reqBody["event_id"].is_number_unsigned())
    return templateReturn(400, "Invalid params or params count");

  int event = reqBody["event_id"];
  std::unique_ptr<sql::PreparedStatement> eventStmt(
      conn->prepareStatement("Select * from event where admin = ? and id = ?"));
  eventStmt->setInt(2, event);
  eventStmt->setInt(1, userId);
  std::unique_ptr<sql::ResultSet> eventRes(eventStmt->executeQuery());

  if (!eventRes->next())
    return templateReturn(401, "Access denied");

  std::unique_ptr<sql::PreparedStatement> hobbyStmt(
      conn->prepareStatement("DELETE FROM eventhobby WHERE event_id = ? and hobby = ?;"));
  if (reqBody["hobby"].is_string()) {
    std::string hobby = reqBody["hobby"];
    hobbyStmt->setString(2, hobby);
    hobbyStmt->setInt(1, event);
    hobbyStmt->executeQuery();
  } else {
    std::vector<std::string> list = reqBody["hobby"];
    for (const auto &iter: list) {

      hobbyStmt->setString(2, iter);
      hobbyStmt->setInt(1, event);
      hobbyStmt->executeQuery();
    }
  }
  return templateReturn(200, "OK");
}


http::response<http::string_body> ViewEventHobby::put() {
  return defaultPlug();
}

ViewEventHobby::ViewEventHobby(const http::request<http::string_body> &_req,
                               const std::shared_ptr<sql::Connection> &_conn,
                               int _userId) : View(_req, _conn, _userId) {

}






