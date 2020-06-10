#include "ViewUserFolList.h"


http::response<http::string_body> ViewUserFolList::post() {
  return defaultPlug();
}

http::response<http::string_body> ViewUserFolList::put() {
  return defaultPlug();
}

http::response<http::string_body> ViewUserFolList::delete_() {
  return defaultPlug();
}

http::response<http::string_body> ViewUserFolList::get() {
  boost::beast::http::response<http::string_body> res;
  res.set(http::field::content_type, "application/json");
  nlohmann::json respBody;
  std::unique_ptr<sql::PreparedStatement> userStmt(conn->prepareStatement(
      "select login, name, surname, idol_id from MeetYou.user u INNER JOIN "
      "MeetYou.userfollowers f ON ( f.idol_id = u.id)  where f.user_id = ? ;"));
  userStmt->setInt(1, userId);
  std::unique_ptr<sql::ResultSet> followers(userStmt->executeQuery());
  while (followers->next()) {
    respBody["followers"] += {
        {"login",   followers->getString(1)},
        {"name",    followers->getString(2)},
        {"surname", followers->getString(3)},
        {"user_id", followers->getString(4)},
    };
  }
  if (!respBody.contains("followers"))
    respBody["followers"] = std::vector<std::string>(0);
  res.result(200);
  res.body() = respBody.dump();
  res.set(http::field::content_length, respBody.dump().size());
  return res;
}

ViewUserFolList::ViewUserFolList(const http::request<http::string_body> &_req,
                                 const std::shared_ptr<sql::Connection> &_conn, int _userId)
    : View(_req, _conn,
           _userId) {}

