#include "ViewEventFollow.h"

ViewEventFollow::ViewEventFollow(const http::request<http::string_body> &_req,
                                 const std::shared_ptr<sql::Connection> &_conn,
                                 int _userId)
    : View(_req, _conn, _userId) {}

http::response<http::string_body> ViewEventFollow::get() {
  boost::beast::http::response<http::string_body> res;
  res.set(http::field::content_type, "application/json");
  nlohmann::json respBody;
  std::unique_ptr<sql::PreparedStatement> userStmt(conn->prepareStatement(
      "select name, type, date, u.id from MeetYou.event u INNER JOIN "
      "MeetYou.followers f ON (f.event_id = u.id) where user_id = ? ;"));
  userStmt->setInt(1, userId);
  std::unique_ptr<sql::ResultSet> followers(userStmt->executeQuery());
  while (followers->next()) {
    respBody["followers"] += {
        {"name", followers->getString(1)},
        {"type", followers->getString(2)},
        {"date", followers->getString(3)},
        {"event_id", followers->getString(4)},
    };
  }
  if (!respBody.contains("followers"))
    respBody["followers"] = std::vector<std::string>();
  res.result(200);
  res.body() = respBody.dump();
  res.set(http::field::content_length, respBody.dump().size());
  return res;
}

http::response<http::string_body> ViewEventFollow::post() {
  nlohmann::json js;
  try {
    js = nlohmann::json::parse(req.body());
  } catch (nlohmann::json::parse_error &e) {
    return templateReturn(400, "JSON error");
  }
  if (js.contains("event_id")) {
    if (js["event_id"].is_number_unsigned()) {
      std::unique_ptr<sql::PreparedStatement> isFollowerStmt(
          conn->prepareStatement(
              "Select * from followers where event_id = ? and user_id = ?"));
      isFollowerStmt->setInt(1, js["event_id"]);
      isFollowerStmt->setInt(2, userId);
      std::unique_ptr<sql::ResultSet> user(isFollowerStmt->executeQuery());
      while (user->next()) {
        return templateReturn(200, "OK");
      }
      std::unique_ptr<sql::PreparedStatement> followStmt(conn->prepareStatement(
          "INSERT IGNORE INTO followers (event_id,user_id) VALUES (?, ?);"));
      followStmt->setInt(1, js["event_id"]);
      followStmt->setInt(2, userId);
      followStmt->execute();
      return templateReturn(200, "OK");
    }
    return templateReturn(400, "Invalid event_id");
  }
  return templateReturn(400, "Invalid data");
  ;
}

http::response<http::string_body> ViewEventFollow::delete_() {
  nlohmann::json js;
  try {
    js = nlohmann::json::parse(req.body());
  } catch (nlohmann::json::parse_error &e) {
    return templateReturn(400, "JSON error");
  }
  if (js.contains("event_id")) {
    if (js["event_id"].is_number_unsigned()) {
      std::unique_ptr<sql::PreparedStatement> followStmt(conn->prepareStatement(
          "DELETE FROM followers WHERE  event_id = ? AND user_id = ?;"));
      followStmt->setInt(1, js["event_id"]);
      followStmt->setInt(2, userId);
      followStmt->execute();
      return templateReturn(200, "OK");
    }
    return templateReturn(400, "Invalid event_id");
  }
  return templateReturn(400, "Invalid data");
}

http::response<http::string_body> ViewEventFollow::put() {
  return defaultPlug();
}
