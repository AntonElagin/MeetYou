#include "Router.h"
#include "CommonChatView.h"
#include "MessageChatView.h"
#include "UserChatView.h"
#include <regex>

Router::Router(http::request<http::string_body> req, const std::string &ip)
    : req(std::move(req)), userId(-1), ip(ip) {
  authGetMap = {{"/auth",  false},
                {"/user",  false},
                {"/event", true}};
  driver = get_driver_instance();
  std::shared_ptr<sql::Connection> con(
      driver->connect("tcp://127.0.0.1:3306", "root", "12A02El99"));
  con->setSchema("MeetYou");
  conn = std::move(con);
}

std::unique_ptr<View> Router::getView(const std::string &path) {
  std::regex reg{"/chat/(history|members_list|members_count)"};
  if (path == "/auth" || path == "/")
    return std::unique_ptr<View>(new ViewRegistration(req, conn, userId, ip));
  else if (path == "/user")
    return std::unique_ptr<View>(new ViewUser(req, conn, userId));
  else if (path == "/event")
    return std::unique_ptr<View>(new ViewEvent(req, conn, userId));
  else if (path == "/user/follow")
    return std::unique_ptr<View>(new ViewUserFollow(req, conn, userId));
  else if (path == "/event/follow")
    return std::unique_ptr<View>(new ViewFollow(req, conn, userId));
  else if (path == "/chat/message")
    return std::unique_ptr<View>(new ViewMessageChat(req, conn, userId));
  else if (path == "/chat/user")
    return std::unique_ptr<View>(new ViewUserChat(req, conn, userId));
  else if (std::regex_search(path, reg))
    return std::unique_ptr<View>(new ViewChatCommon(req, conn, userId));
  else
    return std::unique_ptr<View>(new ViewOther(req, conn, userId));
}

http::response<http::string_body> Router::authException() {
  http::response<http::string_body> res;
  res.result(401);
  res.set(http::field::content_type, "json/application");
  res.keep_alive(req.keep_alive());
  nlohmann::json body;
  body["status"] = 401;
  body["message"] = "Please auth!";
  body["redirect_url"] = "/auth";
  std::string s = body.dump();
  res.body() = s;
  res.set(http::field::content_length, s.length());
  return res;
}

http::response<http::string_body> Router::methodException() {
  http::response<http::string_body> res;
  res.result(405);
  res.set(http::field::content_type, "json/application");
  res.keep_alive(req.keep_alive());
  nlohmann::json body;
  body["status"] = 401;
  body["message"] = "Invalid method";
  body["methods"] += "get";
  body["methods"] += "post";
  body["methods"] += "delete";
  body["methods"] += "put";
  std::string s = body.dump();
  res.body() = s;
  res.set(http::field::content_length, s.length());
  return res;
}
