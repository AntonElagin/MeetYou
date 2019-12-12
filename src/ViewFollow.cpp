#include "ViewFollow.h"

ViewFollow::ViewFollow(const http::request<http::string_body> &_req, const std::shared_ptr<sql::Connection> &_conn,
                       int _userId) : View(_req, _conn, _userId) {

}

http::response<http::string_body> ViewFollow::get() {
  return defaultPlug();
}

http::response<http::string_body> ViewFollow::post() {
  boost::beast::http::response<http::string_body> res;
  //  TODO : Падает на невалидном JSON
  nlohmann::json respBody;
  nlohmann::json js = nlohmann::json::parse(req.body());
  if (js.contains("event_id")) {
    if (js["event_id"].is_number_unsigned()) {
      std::unique_ptr<sql::PreparedStatement> followStmt(conn->prepareStatement(
          "INSERT OR IGNORE INTO followers (event_id,user_id) VALUES (?, ?);"));
      followStmt->setInt(1, js["event_id"]);
      followStmt->setInt(2, userId);
      followStmt->execute();
      respBody["status"] = 200;
      respBody["message"] = "OK";
      res.result(200);
      res.body() = respBody.dump();
      res.set(http::field::content_length, respBody.dump().size());
      return res;
    }
    res.result(400);
    respBody["status"] = 400;
    respBody["message"] = "Invalid event_id";
    std::string exBodyStr = respBody.dump();
    res.set(http::field::content_length, exBodyStr.size());
    res.body() = exBodyStr;
    res.prepare_payload();
    return res;
  }
  res.result(400);
  respBody["status"] = 400;
  respBody["message"] = "Invalid json";
  std::string exBodyStr = respBody.dump();
  res.set(http::field::content_length, exBodyStr.size());
  res.body() = exBodyStr;
  res.prepare_payload();
  return res;
}

http::response<http::string_body> ViewFollow::delete_() {
  boost::beast::http::response<http::string_body> res;
  //  TODO : Падает на невалидном JSON
  nlohmann::json respBody;
  nlohmann::json js = nlohmann::json::parse(req.body());
  if (js.contains("event_id")) {
    if (js["event_id"].is_number_unsigned()) {
      std::unique_ptr<sql::PreparedStatement> followStmt(conn->prepareStatement(
          "DELETE FROM followers WHERE  event_id = ? AND user_id = ?);"));
      followStmt->setInt(1, js["event_id"]);
      followStmt->setInt(2, userId);
      followStmt->execute();
      respBody["status"] = 200;
      respBody["message"] = "OK";
      res.result(200);
      res.body() = respBody.dump();
      res.set(http::field::content_length, respBody.dump().size());
      return res;
    }
    res.result(400);
    respBody["status"] = 400;
    respBody["message"] = "Invalid event_id";
    std::string exBodyStr = respBody.dump();
    res.set(http::field::content_length, exBodyStr.size());
    res.body() = exBodyStr;
    res.prepare_payload();
    return res;
  }
  res.result(400);
  respBody["status"] = 400;
  respBody["message"] = "Invalid json";
  std::string exBodyStr = respBody.dump();
  res.set(http::field::content_length, exBodyStr.size());
  res.body() = exBodyStr;
  res.prepare_payload();
  return res;
}

http::response<http::string_body> ViewFollow::put() {
  return defaultPlug();
}
