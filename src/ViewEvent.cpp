#include "ViewEvent.h"

ViewEvent::ViewEvent(const http::request<http::string_body> &_req, const std::shared_ptr<sql::Connection> &_conn,
                     int _userId) : View(_req, _conn, _userId) {
}

http::response<http::string_body> ViewEvent::get() {
  boost::beast::http::response<http::string_body>res;
  res.set(http::field::content_type, "application/json");
  nlohmann::json js;
  try {
    js = nlohmann::json::parse(req.body());
  } catch (nlohmann::json::parse_error &e) {
    return templateReturn(400, "JSON error");
  }
  nlohmann::json respBody;
  if (js.contains("name")) {
    std::string name = js["name"];
    std::string type, description, date, admin;
      try {
        // Поиск event
        std::unique_ptr<sql::PreparedStatement> eventStmt(conn->prepareStatement(
            "SELECT id, type, description, date, admin FROM event WHERE name = ?;"));
        eventStmt->setString(1, name);
        std::unique_ptr<sql::ResultSet> event(eventStmt->executeQuery());
        while (event->next()) {
          respBody["event_id"] = event->getInt(1);
          respBody["type"] = event->getString(2);
          respBody["description"] = event->getString(3);
          respBody["date"] = event->getString(4);
          respBody["admin"] = event->getString(5);
        }
        std::unique_ptr<sql::PreparedStatement> userStmt(conn->prepareStatement(
            "select login ,name, surname, u.id from MeetYou.user u\n"
            "INNER JOIN MeetYou.followers f ON ( f.event_id = ? AND u.id = f.user_id) ;"));
        userStmt->setInt(1, respBody["event_id"]);
        std::unique_ptr<sql::ResultSet> followers(userStmt->executeQuery());
        while (followers->next()) {
          respBody["followers"] += {{"login",followers->getString(1)},
                                    {"name",followers->getString(2)},
                                    {"surname",followers->getString(3)},
                                    {"user_id",followers->getString(4)},};
        }
        res.result(200);
        std::string body = respBody.dump();
        res.body() = body;
        res.set(http::field::content_length, body.length());
        return res;
      }
      catch (sql::SQLException & e) {
//        TODO : Возвращаем 500 или 400(если название дублируется)
        return templateReturn(500, e.what());
      }
    }
  return templateReturn(400, "Invalid params or params count");
}

http::response<http::string_body> ViewEvent::post() {
  boost::beast::http::response<http::string_body>res;
  res.set(http::field::content_type, "application/json");
  nlohmann::json js;
  try {
    js = nlohmann::json::parse(req.body());
  } catch (nlohmann::json::parse_error &e) {
    return templateReturn(400, "JSON error");
  }
  if (js.contains("name") && js.contains("type") && js.contains("description") && js.contains("date")) {
    std::string name = js["name"],
        type = js["type"],
        description = js["description"],
        date = js["date"];

    if (isDescription(description) && isName(name)) {
      try {
        // Создание пользователя
        std::unique_ptr<sql::PreparedStatement> userStmt(conn->prepareStatement(
            "INSERT  INTO event(name, type, description, date, admin) VALUES (?,?,?,?,?);"));
        userStmt->setString(1, name);
        userStmt->setString(2, type);
        userStmt->setString(3, description);
        userStmt->setString(4, date);
        userStmt->setInt(5, userId);
        if (userStmt->execute()) throw "server error";
        return templateReturn(200, "OK");
      }
      catch (sql::SQLException & e) {
//        TODO : ВОзвращаем 500 или 400(если название дублируется)
        return templateReturn(500, e.what());
      }
    }
    return templateReturn(400, "Invalid data");
  }
  return templateReturn(400, "Invalid params or params count");
}

http::response<http::string_body> ViewEvent::delete_() {
  return boost::beast::http::response<http::string_body>();
}

http::response<http::string_body> ViewEvent::put() {
  boost::beast::http::response<http::string_body> res;
  res.set(http::field::content_type, "application/json");
  nlohmann::json js;
  try {
    js = nlohmann::json::parse(req.body());
  } catch (nlohmann::json::parse_error &e) {
    return templateReturn(400, "JSON error");
  }
  nlohmann::json respBody;
  if (js.contains("name") && js.contains("type") && js.contains("description") && js.contains("date")) {
    std::string name = js["name"],
        type = js["type"],
        description = js["description"],
        date = js["date"];


    if (isDescription(description) && isName(name)) {
      try {
        // Создание пользователя
        std::unique_ptr<sql::PreparedStatement> userStmt(conn->prepareStatement(
            "UPDATE event SET name = ?, type = ?, description = ?, date = ? WHERE name = ? and admin = ?;"));
        userStmt->setString(1, name);
        userStmt->setString(2, type);
        userStmt->setString(3, description);
        userStmt->setString(4, date);
        userStmt->setInt(6, userId);
        userStmt->setString(5, name);
        if (userStmt->execute()) throw "server error";
        return templateReturn(200, "OK");
      }
      catch (sql::SQLException &e) {
        return templateReturn(500, e.what());
      }
    }
    return templateReturn(400, "Invalid some data");
  }
  return templateReturn(400, "Invalid params or params count");
}

bool ViewEvent::isName(const std::string &value) {
  return value.length() > 6 && value.length() < 45;
}

bool ViewEvent::isDescription(const std::string &value) {
//  std:tm time
  return value.length() < 150;
}
