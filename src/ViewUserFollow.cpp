#include "ViewUserFollow.h"

ViewUserFollow::ViewUserFollow(const http::request<http::string_body> &_req,
                               const std::shared_ptr<sql::Connection> &_conn,
                               int _userId)
    : View(_req, _conn, _userId) {}

http::response<http::string_body> ViewUserFollow::get() {
  http::response<http::string_body> res;
  res.set(http::field::content_type, "application/json");
  res.result(200);
  nlohmann::json reqBody;
  try {
    reqBody = nlohmann::json::parse(req.body());
  } catch (nlohmann::json::parse_error &e) {
    return templateReturn(400, "JSON error");
  }

  if (!(reqBody.contains("idol_id") && reqBody["idol_id"].is_number_unsigned()))
    return templateReturn(400, "Invalid params or params count");

  std::unique_ptr<sql::PreparedStatement> isFollowerStmt(
      conn->prepareStatement(
          "Select * from userfollowers where idol_id = ? and user_id = ?"));
  isFollowerStmt->setInt(1, reqBody["idol_id"]);
  isFollowerStmt->setInt(2, userId);
  std::unique_ptr<sql::ResultSet> user(isFollowerStmt->executeQuery());
  nlohmann::json respBody;
  std::string str;
  respBody["follow"] = user->next();
  str = respBody.dump();
  res.set(http::field::content_length, str.length());
  res.body() = str;
  return res;
}


http::response<http::string_body> ViewUserFollow::post() {
  nlohmann::json js;
  try {
    js = nlohmann::json::parse(req.body());
  } catch (nlohmann::json::parse_error &e) {
    return templateReturn(400, "JSON error");
  }

  if (js.contains("idol_id")) {
    if (js["idol_id"].is_number_unsigned()) {
      std::unique_ptr<sql::PreparedStatement> isFollowerStmt(
          conn->prepareStatement(
              "Select * from userfollowers where idol_id = ? and user_id = ?"));
      isFollowerStmt->setInt(1, js["idol_id"]);
      isFollowerStmt->setInt(2, userId);
      std::unique_ptr<sql::ResultSet> user(isFollowerStmt->executeQuery());
      while (user->next()) {
        return templateReturn(200, "OK");
      }
      std::unique_ptr<sql::PreparedStatement> followStmt(conn->prepareStatement(
          "INSERT IGNORE INTO userfollowers (idol_id,user_id) VALUES (?, ?);"));
      followStmt->setInt(1, js["idol_id"]);
      followStmt->setInt(2, userId);
      followStmt->execute();

      return templateReturn(200, "OK");
    }

    return templateReturn(400, "Invalid event_id");
  }
  return templateReturn(400, "Invalid data");
}

http::response<http::string_body> ViewUserFollow::delete_() {
  nlohmann::json js;
  try {
    js = nlohmann::json::parse(req.body());
  } catch (nlohmann::json::parse_error &e) {
    return templateReturn(400, "JSON error");
  }
  if (js.contains("idol_id")) {
    if (js["idol_id"].is_number_unsigned()) {
      std::unique_ptr<sql::PreparedStatement> followStmt(conn->prepareStatement(
          "DELETE FROM userfollowers WHERE idol_id = ? AND user_id = ?;"));
      followStmt->setInt(1, js["idol_id"]);
      followStmt->setInt(2, userId);
      followStmt->execute();
      return templateReturn(200, "OK");
    }

    return templateReturn(400, "Invalid event_id");
  }

  return templateReturn(400, "Invalid data");
}

http::response<http::string_body> ViewUserFollow::put() {
  return defaultPlug();
}