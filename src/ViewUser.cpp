#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/gregorian/parsers.hpp>
#include "ViewUser.h"

ViewUser::ViewUser(const http::request<http::string_body> &_req, const std::shared_ptr<sql::Connection> &_conn,
                   int _userId) : View(_req, _conn, _userId) {

}

http::response<http::string_body> ViewUser::put() {
  return defaultPlug();
}

http::response<http::string_body> ViewUser::get() {
  boost::beast::http::response<http::string_body> res;
  res.set(http::field::content_type, "application/json");
  nlohmann::json js;
  try {
    js = nlohmann::json::parse(req.body());
  } catch (nlohmann::json::parse_error &e) {
    return templateReturn(400, "JSON error");
  }

  int userId = -1;
  if ((js.contains("user_id") && js["user_id"].is_number_integer()) || js.contains("login")) {
    std::string login;
    int id = -1;
    if (js.contains("user_id"))
      id = js["user_id"];
    else
      login = js["login"];
    std::unique_ptr<sql::PreparedStatement> userStmt(conn->prepareStatement(
        "SELECT id, name, surname, sex, birthday, location, login FROM user where login = ? OR id = ?;"));
    userStmt->setString(1, login);
    userStmt->setInt(2, id);
    std::unique_ptr<sql::ResultSet> user(userStmt->executeQuery());
    nlohmann::json respBody;
    while (user->next()) {
      respBody["user_id"] = user->getInt(1);
      respBody["name"] = user->getString(2);
      respBody["surname"] = user->getString(3);
      respBody["sex"] = user->getString(4);
      respBody["birthday"] = user->getString(5);
      respBody["location"] = user->getString(6);
      respBody["login"] = user->getString(7);
      std::string respString(respBody.dump());
      res.result(200);
      res.body() = respString;
      res.set(http::field::content_length, respString.length());
      return res;
    }
    return templateReturn(204, "No user or data");
  }
  return templateReturn(400, "Invalid data");
}

http::response<http::string_body> ViewUser::post() {
  boost::beast::http::response<http::string_body> res;
  res.set(http::field::content_type, "application/json");
  nlohmann::json js;
  try {
    js = nlohmann::json::parse(req.body());
  } catch (nlohmann::json::parse_error &e) {
    return templateReturn(400, "JSON error");
  }
  if (js.contains("name")
      && js.contains("surname") && js.contains("sex")
      && js.contains("location") && js.contains("birthday")) {
    std::string name = js["name"],
        surname = js["surname"],
        sex = js["sex"],
        location = js["location"],
        birthday = js["birthday"];
    int validateCode = validate(name, surname, sex, location, birthday);
    if (!validateCode) {
      std::unique_ptr<sql::PreparedStatement> userStmt(conn->prepareStatement(
          "Update user set name = ?, surname = ?, sex = ?, birthday = ?, location = ? where id = ?"));
      userStmt->setString(1, name);
      userStmt->setString(2, surname);
      userStmt->setString(3, sex);
      userStmt->setString(4, birthday);
      userStmt->setString(5, location);
      userStmt->setInt(6, userId);
      nlohmann::json respBody;
      if (!userStmt->execute()) {
        return templateReturn(200, "OK");
      }
      throw "server error";
    }
    switch (validateCode) {
      case 1:
        return templateReturn(400, "Invalid name");
      case 2:
        return templateReturn(400, "Invalid surname");
      case 3:
        return templateReturn(400, "Invalid sex");
      case 4:
        return templateReturn(400, "Invalid location");
      case 5:
        return templateReturn(400, "Invalid birthday");
    }
  }
  return templateReturn(400, "Invalid params or params count");
}

http::response<http::string_body> ViewUser::delete_() {
  return defaultPlug();
}

int ViewUser::validate(const std::string &name, const std::string &surname, const std::string &sex,
                       const std::string &location, const std::string &birthday) {
  if (!(name.length() > 2 && name.length() < 45))
    return 1;
  if (!(surname.length() > 2 && surname.length() < 45))
    return 2;
  if (sex != "male" && sex != "female")
    return 3;
  if (!(location.length() > 5 && location.length() < 45))
    return 4;
  try {
    using namespace boost::gregorian;
    date birth(from_string(birthday));
    date now(day_clock::local_day());
    years year(16);
    if (birth.is_not_a_date() || (birth + year) > now)
      return 5;
  } catch (...) {
    return 5;
  }
  return 0;
}
