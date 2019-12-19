#ifndef PLUS_PROJECT_MESSAGECHATVIEW_H
#define PLUS_PROJECT_MESSAGECHATVIEW_H

#include "View.h"

using json = nlohmann::json;

class ViewMessageChat : public View {
 public:
  ViewMessageChat(http::request<http::string_body> _req,
                  std::shared_ptr<sql::Connection> _conn, int userId)
      : View(_req, _conn, userId) {}

  http::response<http::string_body> get() override {
    json j = json::parse(req.body());
    int chatid = -1;
    json body;
    std::string text;
    int author_id = -1;
    int messid;
    if (j.contains("messid")) {
      messid = j.at("messid");
      if (not permission_member_check(chatid)) {
        json j({"message", "access denied"});
        body = j;
      } else
        try {
          std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
              "select body,author_id from Message where id=?"));
          stmt->setInt(1, messid);
          std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
          while (res->next()) {
            text = res->getString("body");
            author_id = res->getInt("author_id");
          }
          body["text"] = text;
          body["author_id"] = author_id;
          ResponseCreator resp(body, 0);
          return resp.get_resp();
        } catch (sql::SQLException &e) {
          json body(exception_handler(e));
          ResponseCreator resp(body, 1);
          return resp.get_resp();
        }
    }
  }

  http::response<http::string_body> post() override {
    json j = json::parse(req.body());
    int chatid = -1;
    json body;
    std::string text;
    if (j.contains("message") && j.contains("chatid")) {
      text = j.at("message");
      chatid = j.at("chatid");
      if (not permission_member_check(chatid)) {
        json j({"message", "access denied"});
        body = j;
      } else
        try {
          std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
              "INSERT INTO `Message` (`publication_date`, `body`, `author_id`, "
              "`chat_id`) VALUES (NOW(), ?, ?, ?)"));
          stmt->setString(1, text);
          stmt->setInt(2, userId);
          stmt->setInt(3, chatid);
          stmt->executeUpdate();
          body["status"] = "OK";
          ResponseCreator resp(body, 0);
          return resp.get_resp();
        } catch (sql::SQLException &e) {
          body = exception_handler(e);
          ResponseCreator resp(body, 1);
          return resp.get_resp();
        }
    }
  }

  http::response<http::string_body> delete_() override {
    json j = json::parse(req.body());
    int messid, chatid;
    json body;
    if (j.contains("messid") && j.contains("chatid")) {
      messid = j.at("messid");
      chatid = j.at("chatid");
      if (not(permission_admin_check(chatid) &&
              permission_owner_check(messid))) {
        json j({"message", "access denied"});
        body = j;
      } else
        try {
          std::unique_ptr<sql::PreparedStatement> stmt(
              conn->prepareStatement("DELETE FROM `Message` WHERE `id` = ?"));
          stmt->setInt(1, messid);
          stmt->executeUpdate();
          body["status"] = "OK";
          ResponseCreator resp(body, 0);
          return resp.get_resp();
        } catch (sql::SQLException &e) {
          body = exception_handler(e);
          ResponseCreator resp(body, 1);
          return resp.get_resp();
        }
    }
  }

  http::response<http::string_body> put() override {
    json j = json::parse(req.body());
    int messid, chatid;
    std::string text;
    json body;
    if (j.contains("message") && j.contains("chatid") && j.contains("messid")) {
      messid = j.at("messid");
      chatid = j.at("chatid");
      text = j.at("message");
      if (not(permission_admin_check(chatid) &&
              permission_owner_check(messid))) {
        json j({"message", "access denied"});
        body = j;
      } else
        try {
          std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
              "UPDATE `Message`  SET `body` = ? WHERE `id` = ?"));
          stmt->setString(1, text);
          stmt->setInt(1, messid);
          stmt->executeUpdate();
          body["status"] = "OK";
          ResponseCreator resp(body, 0);
          return resp.get_resp();
        } catch (sql::SQLException &e) {
          body = exception_handler(e);
          ResponseCreator resp(body, 1);
          return resp.get_resp();
        }
    }
  }

  bool permission_owner_check(const int messageid) {
    std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
        "select distinct author_id from Message where id=?"));
    stmt->setInt(1, messageid);
    stmt->setInt(1, userId);
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
    if (res->next()) {
      if (userId == res->getInt(1)) return true;
    } else
      return false;
  }
};

#endif  // PLUS_PROJECT_MESSAGECHATVIEW_H
