#include "ViewUserFollow.h"


ViewUserFollow::ViewUserFollow(const http::request<http::string_body> &_req, const std::shared_ptr<sql::Connection> &_conn,
                       int _userId) : View(_req, _conn, _userId) {

}

http::response<http::string_body> ViewUserFollow::get() {
  boost::beast::http::response<http::string_body> res;
  res.set(http::field::content_type, "application/json");
  nlohmann::json respBody;
  std::unique_ptr<sql::PreparedStatement> userStmt(conn->prepareStatement(
      "select login, name, surname, u.id from MeetYou.user u INNER JOIN MeetYou.userfollowers f ON ( f.user_id = ?)  ;"));
  userStmt->setInt(1, userId);
  std::unique_ptr<sql::ResultSet> followers(userStmt->executeQuery());
  while (followers->next()) {
    respBody["followers"] += {{"login",followers->getString(1)},
                              {"name",followers->getString(2)},
                              {"surname",followers->getString(3)},
                              {"user_id",followers->getString(4)},};
  }
  res.result(200);
  res.body() = respBody.dump();
  res.set(http::field::content_length, respBody.dump().size());
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
      std::unique_ptr<sql::PreparedStatement> isFollowerStmt(conn->prepareStatement("Select * from userfollowers where idol_id = ? and user_id = ?"));
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