#include "ViewFindEvent.h"

http::response<http::string_body> ViewFindEvent::get() {
  http::response<http::string_body> res;
  res.set(http::field::content_type, "json/application");
  res.keep_alive(req.keep_alive());

  nlohmann::json reqBody;
  try {
    reqBody = nlohmann::json::parse(req.body());
  } catch (...) {
    return templateReturn(400, "Invalid JSON");
  }

  if (!(reqBody.contains("key")))
    return templateReturn(400, "Invalid params or params count");
  std::string key = reqBody["key"];

  if (key.length() < 1 || key.length() > 99)
    return templateReturn(400, "Invalid key");

  std::unique_ptr<sql::PreparedStatement> findStmt(conn->prepareStatement(
      "select id, name, type, description, date from event where INSTR(name, "
      "?) > 0 or INSTR(description, ?) > 0  "));
  findStmt->setString(1, key);
  findStmt->setString(2, key);
  std::unique_ptr<sql::ResultSet> findRes(findStmt->executeQuery());

  nlohmann::json resBody;
  while (findRes->next()) {
    resBody["hobbies"] += {{"id", findRes->getInt(1)},
                           {"name", findRes->getString(2)},
                           {"type", findRes->getString(3)},
                           {"description", findRes->getString(4)},
                           {"date", findRes->getString(5)}};
  }

  if (!resBody.contains("hobbies"))
    resBody["hobbies"] = std::vector<std::string>();

  std::string str = resBody.dump();
  res.body() = str;
  res.set(http::field::content_length, str.length());
  return res;
}

http::response<http::string_body> ViewFindEvent::post() {
  return defaultPlug();
}

http::response<http::string_body> ViewFindEvent::delete_() {
  return defaultPlug();
}

http::response<http::string_body> ViewFindEvent::put() { return defaultPlug(); }

ViewFindEvent::ViewFindEvent(const http::request<http::string_body> &_req,
                             const std::shared_ptr<sql::Connection> &_conn,
                             int _userId)
    : View(_req, _conn, _userId) {}
