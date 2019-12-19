#ifndef PLUS_PROJECT_USERCHATVIEW_H
#define PLUS_PROJECT_USERCHATVIEW_H

#include "View.h"

using json = nlohmann::json;

class ViewUserChat : public View {
 public:
  ViewUserChat(http::request<http::string_body> _req,
               std::shared_ptr<sql::Connection> _conn, int userId)
      : View(_req, _conn, userId) {}

  http::response<http::string_body> get() override {
    return boost::beast::http::response<http::string_body>();
  }

  http::response<http::string_body> post() override {
    json j = json::parse(req.body());
    std::string text;
    int chatid = -1, new_member_id, is_admin;
    json body;
    if (j.contains("new_member_id") && j.contains("chatid") &&
        j.contains("is_admin")) {
      new_member_id = j.at("new_member_id");
      chatid = j.at("chatid");
      is_admin = j.at("is_admin");
      if (not permission_member_check(chatid)) {
        json j({"message", "access denied"});
        body = j;
      } else
        try {
          std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
              "INSERT INTO `result_table` (`chat_id`, `user_id`, `is_admin`) "
              "VALUES (?, ?, ?)"));
          stmt->setInt(1, chatid);
          stmt->setInt(2, new_member_id);
          stmt->setInt(3, is_admin);
          stmt->executeUpdate();
          body["message"] = "OK";
          ResponseCreator resp(body, 0);
          return resp.get_resp();
        } catch (sql::SQLException &e) {
          json body(exception_handler(e));
          ResponseCreator resp(body, 1);
          return resp.get_resp();
        }
    }
  }

  http::response<http::string_body> delete_() override {
    json j = json::parse(req.body());
    int chatid = -1, member_id;
    json body;
    if (j.contains("member_id") && j.contains("chatid")) {
      member_id = j.at("new_member_id");
      chatid = j.at("chatid");
      if (not permission_admin_check(chatid)) {
        json j({"message", "access denied"});
        body = j;
      } else
        try {
          std::unique_ptr<sql::PreparedStatement> stmt(
              conn->prepareStatement("DELETE FROM `result_table` WHERE "
                                     "`chat_id` = ? AND `user_id` = ?"));
          stmt->setInt(1, chatid);
          stmt->setInt(2, member_id);
          stmt->executeUpdate();
          body["message"] = "OK";
          ResponseCreator resp(body, 0);
          return resp.get_resp();
        } catch (sql::SQLException &e) {
          json body(exception_handler(e));
          ResponseCreator resp(body, 1);
          return resp.get_resp();
        }
    }
  }

  http::response<http::string_body> put() override {  ///сделать админом
    json j = json::parse(req.body());
    int chatid = -1, member_id, is_admin;
    json body;
    if (j.contains("member_id") && j.contains("chatid") &&
        j.contains("is_admin")) {
      member_id = j.at("member_id");
      chatid = j.at("chatid");
      is_admin = j.at("is_admin");
      if (not permission_admin_check(chatid)) {
        json j({"message", "access denied"});
        body = j;
      } else
        try {
          std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
              "UPDATE `result_table` t SET t.`is_admin` = 1 WHERE t.`chat_id` "
              "= 3 AND t.`user_id` = 3 AND t.`is_admin` = ?"));
          stmt->setInt(1, chatid);
          stmt->setInt(2, member_id);
          stmt->setInt(3, is_admin);
          stmt->executeUpdate();
          body["message"] = "OK";
          ResponseCreator resp(body, 0);
          return resp.get_resp();
        } catch (sql::SQLException &e) {
          json body(exception_handler(e));
          ResponseCreator resp(body, 1);
          return resp.get_resp();
        }
    }
  }
};

#endif  // PLUS_PROJECT_USERCHATVIEW_H
