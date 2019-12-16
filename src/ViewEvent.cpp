#include "ViewEvent.h"
#include "boost/date_time/gregorian/gregorian.hpp"


ViewEvent::ViewEvent(const http::request<http::string_body> &_req, const std::shared_ptr<sql::Connection> &_conn,
                     int _userId) : View(_req, _conn, _userId) {
}

http::response<http::string_body> ViewEvent::get() {
  boost::beast::http::response<http::string_body> res;
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
    // Поиск event
    std::unique_ptr<sql::PreparedStatement> eventStmt(conn->prepareStatement(
        "SELECT id, type, description, date, admin FROM event WHERE name = ?;"));
    eventStmt->setString(1, name);
    std::unique_ptr<sql::ResultSet> event(eventStmt->executeQuery());
    bool flag = true;
    while (event->next()) {
      flag = false;
      respBody["event_id"] = event->getInt(1);
      respBody["name"] = name;
      respBody["type"] = event->getString(2);
      respBody["description"] = event->getString(3);
      respBody["date"] = event->getString(4);
      respBody["admin"] = event->getString(5);
    }
    if (flag)
      return templateReturn(404,"event does not found");
    std::unique_ptr<sql::PreparedStatement> userStmt(conn->prepareStatement(
        "select login ,name, surname, u.id from MeetYou.user u\n"
        "INNER JOIN MeetYou.followers f ON ( f.event_id = ? AND u.id = f.user_id) ;"));
    userStmt->setInt(1, respBody["event_id"]);
    std::unique_ptr<sql::ResultSet> followers(userStmt->executeQuery());
    while (followers->next()) {
        respBody["followers"] += {{"login",   followers->getString(1)},
                                  {"name",    followers->getString(2)},
                                  {"surname", followers->getString(3)},
                                  {"user_id", followers->getString(4)},};

    }
    res.result(200);
    std::string body = respBody.dump();
    res.body() = body;
    res.set(http::field::content_length, body.length());
    return res;
  }
  return templateReturn(400, "Invalid params or params count");
}

http::response<http::string_body> ViewEvent::post() {
  boost::beast::http::response<http::string_body> res;
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
    int validCode = validate(name, type, description, date);
    if (!validCode) {
      std::unique_ptr<sql::PreparedStatement> validateStmt(conn->prepareStatement(
          "SELECT * FROM event WHERE name = ?;"));
      validateStmt->setString(1, name);
      std::unique_ptr<sql::ResultSet> result(validateStmt->executeQuery());
      if (result->next())
        return templateReturn(400, "Duplicate name");
      // Создание ивента
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
    switch (validCode) {
      case 1:
        return templateReturn(400, "Invalid name");
      case 2:
        return templateReturn(400, "Invalid type");
      case 3:
        return templateReturn(400, "Invalid description");
      case 4:
        return templateReturn(400, "Invalid date");
    }
  }
  return templateReturn(400, "Invalid params or params count");
}

http::response<http::string_body> ViewEvent::delete_() {
  return defaultPlug();
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
  if ((js.contains("name") || (js.contains("event_id") && js["event_id"].is_number_integer())) && js.contains("type") &&
      js.contains("description") && js.contains("date")) {
    int event_id = -1;
    std::string name;

    if (js.contains("name"))
      name = js["name"];
    else
      event_id = js["event_id"];

    std::string type = js["type"],
        description = js["description"],
        date = js["date"];

    int validCode = validate((name.length()!=0)?name:"username2", type, description, date);
    if (!validCode) {
      std::unique_ptr<sql::PreparedStatement> userStmt(conn->prepareStatement(
          "UPDATE event SET type = ?, description = ?, date = ? WHERE id = ? OR name = ? and admin = ?;"));
//        userStmt->setString(1, name);
      userStmt->setString(1, type);
      userStmt->setString(2, description);
      userStmt->setString(3, date);
      userStmt->setInt(6, userId);
      userStmt->setString(5, name);
      userStmt->setInt(4, event_id);
      if (userStmt->execute()) throw "server error";
      return templateReturn(200, "OK");
    }
    switch (validCode) {
      case 1:
        return templateReturn(400, "Invalid name");
      case 2:
        return templateReturn(400, "Invalid type");
      case 3:
        return templateReturn(400, "Invalid description");
      case 4:
        return templateReturn(400, "Invalid date");
    }
  }
  return templateReturn(400, "Invalid params or params count");
}


int ViewEvent::validate(const std::string &name, const std::string &type, const std::string &description,
                        const std::string &_date) {
  if (!(name.length() > 5 && name.length() < 50))
    return 1;
  if (!(type.length() > 5 && type.length() < 50))
    return 2;
  if (!(description.length() > 5 && description.length() < 200))
    return 3;
  try {
    using namespace boost::gregorian;
    date dt(from_string(_date));
    date now(day_clock::local_day());
    if (now > dt)
      return 4;
  } catch (...) {
    return 4;
  }
  return 0;
}
