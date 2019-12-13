#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include "ViewUser.h"

ViewUser::ViewUser(const http::request<http::string_body> &_req, const std::shared_ptr<sql::Connection> &_conn,
                   int _userId) : View(_req, _conn, _userId) {

}

http::response<http::string_body> ViewUser::put() {
  return boost::beast::http::response<http::string_body>();
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
  if (js.contains("user_id") || js.contains("login")) {
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
  if (js.contains("user_id") && js.contains("name")
      && js.contains("surname") && js.contains("sex")
      && js.contains("location") && js.contains("birthday")) {
    int id = js["user_id"];
    if (id == userId) {
      std::string name = js["name"],
          surname = js["surname"],
//          TODO : Проверка валидности поля (Тольно : male , female)
          sex = js["sex"],
          location = js["location"],
//          TODO : Проверка валидности birthday
          birthday = js["birthday"];
      std::unique_ptr<sql::PreparedStatement> userStmt(conn->prepareStatement(
          "Update user set name = ?, surname = ?, sex = ?, birthday = ?, location = ? where id = ?"));
      userStmt->setString(1, name);
      userStmt->setString(2, surname);
      userStmt->setString(3, sex);
      userStmt->setString(4, birthday);
      userStmt->setString(5, location);
      userStmt->setInt(6, id);
      nlohmann::json respBody;
      if (!userStmt->execute()) {
        return templateReturn(200, "OK");
      }
      throw "server error";
    }

    return templateReturn(403, "Access denied");
  }
}

http::response<http::string_body> ViewUser::delete_() {
  return defaultPlug();
}