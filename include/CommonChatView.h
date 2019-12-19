#ifndef PLUS_PROJECT_COMMONCHATVIEW_H
#define PLUS_PROJECT_COMMONCHATVIEW_H

#include <boost/beast.hpp>
#include <boost/regex.hpp>
#include <memory>
#include <regex>
#include <utility>
#include "View.h"

namespace http = boost::beast::http;

using json = nlohmann::json;

class ViewChatCommon : public View {
 public:
  ViewChatCommon(http::request<http::string_body> _req,
                 std::shared_ptr<sql::Connection> _conn, int userId)
      : View(_req, _conn, userId) {}

  http::response<http::string_body> post() override {
    json j = json::parse(req.body());
    std::string title;
    std::vector<int> members_list, admin_list;
    try {
      if (j.contains("title") && j.contains("members_list") &&
          j.contains("admin_list")) {
        json jsonik = j.at("members_list");
        if (jsonik.is_array()) {
          for (auto &element : jsonik) members_list.push_back(element);
        }
        jsonik = j.at("admin_list");
        if (jsonik.is_array()) {
          for (auto &element : jsonik) admin_list.push_back(element);
        }
        std::unique_ptr<sql::PreparedStatement> chatStmt(
            conn->prepareStatement("insert into Chat (id,create_date, title) "
                                   "values (null,CURDATE() ,? )"));
        chatStmt->setString(1, title);
        chatStmt->executeUpdate();
        chatStmt.reset(
            conn->prepareStatement("select id from Chat where title like ? "
                                   "order by create_date desc"));
        chatStmt->setString(1, title);
        std::unique_ptr<sql::ResultSet> res(chatStmt->executeQuery());
        int chat_id = -1;
        if (res->next()) chat_id = res->getInt("id");
        std::unique_ptr<sql::PreparedStatement> temp_table_Stmt(
            conn->prepareStatement("INSERT INTO result_table (`chat_id`, "
                                   "`user_id`, `is_admin`) VALUES (?, ?, 1)"));
        for (auto adminid : admin_list) {
          temp_table_Stmt->setInt(1, chat_id);
          temp_table_Stmt->setInt(2, adminid);
        }
        temp_table_Stmt.reset(
            conn->prepareStatement("INSERT INTO result_table (`chat_id`, "
                                   "`user_id`, `is_admin`) VALUES (?, ?, 0)"));
        for (auto memberid : members_list) {
          temp_table_Stmt->setInt(1, chat_id);
          temp_table_Stmt->setInt(2, memberid);
        }
        json body;
        body["message"] = "chat created";
        ResponseCreator resp(body, 0);
        return resp.get_resp();
      }
    } catch (sql::SQLException &e) {
      json body(exception_handler(e));
      ResponseCreator resp(body, 1);
      return resp.get_resp();
    }
  }

  http::response<http::string_body> get() override {
    std::map<std::string, std::string> params_list;
    std::cmatch result;
    boost::string_view path = req.target();
    std::string path_str = path.to_string();
    std::regex regexik(
        "/chat/(history|members_list|members_count)\\?chatid=(\\d+)");
    if (std::regex_match(path_str.c_str(), result, regexik)) {
      for (int i = 0; i < result.size(); i++) {
        if (i == 1) params_list.insert({"search_data", result[i]});
        if (i == 2) params_list.insert({"chatid", result[i]});
      }
    }
    json j(params_list);
    json res_body;
    std::string chatikid;
    int chatid;
    int error = 0;
    std::string search_data;
    if (j.contains("chatid") && j.contains("search_data")) {
      json jsonik = j.at("chatid");
      chatikid = j.at("chatid");
      chatid = std::stoi(chatikid);
      jsonik = j.at("search_data");
      if (jsonik.is_string()) search_data = j.at("search_data");
      if (search_data == "history")
        res_body = history(chatid, error);
      else if (search_data == "members_count")
        res_body = members_count(chatid, error);
      else if (search_data == "members_list")
        res_body = members_list(chatid, error);
      else {
        res_body = {"message", "incorrect url"};
        error = 1;
      }
      ResponseCreator resp(res_body, error);
      return resp.get_resp();
    }
  }

  json history(const int chatid, int &error) {
    if (not permission_member_check(chatid)) {
      json j({"message", "access denied"});
      return j;
    }
    try {
      std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
          "select username nick,body,publication_date pubdate from Message, "
          "User where chat_id = ? and author_id = User.id order by "
          "publication_date asc;"));
      stmt->setInt(1, chatid);
      std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
      std::string text;
      std::string author;
      std::string date;
      json j;
      while (res->next()) {
        text = res->getString("body");
        author = res->getString("nick");
        date = res->getString("pubdate");
        std::vector<std::string> message_data = {text, author, date};
        j.push_back(message_data);
      }
      return std::move(j);
    } catch (sql::SQLException &e) {
      error = 1;
      return exception_handler(e);
    }
  }

  json members_count(const int chatid, int &error) {
    if (not permission_member_check(chatid)) {
      json j({"message", "access denied"});
      return j;
    }
    try {
      std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
          "select count(user_id) quant from result_table where chat_id=?"));
      stmt->setInt(1, chatid);
      std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
      int members_count = -1;
      if (res->next()) members_count = res->getInt(1);
      json j;
      j["members_count"] = members_count;
      return std::move(j);
    } catch (sql::SQLException &e) {
      error = 1;
      return exception_handler(e);
    }
  }

  json members_list(const int chatid, int &error) {
    if (not permission_member_check(chatid)) {
      json j({"message", "access denied"});
      return j;
    }
    try {
      std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
          "select user_id id, username nick from result_table, User where "
          "user_id=User.id and chat_id=?"));
      stmt->setInt(1, chatid);
      std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
      std::map<int, std::string> id_nick_array;
      while (res->next()) {
        id_nick_array[res->getInt("id")] = res->getString("nick");
      }
      json j(id_nick_array);
      return std::move(j);
    } catch (sql::SQLException &e) {
      error = 1;
      return exception_handler(e);
    }
  }

  http::response<http::string_body> delete_() override {
    json j = json::parse(req.body());
    json body;
    int chatid = -1;
    if (j.contains("chatid")) {
      json jsonik = j.at("chatid");
      if (jsonik.is_number()) {
        chatid = j.at("chatid");
      }
      if (not permission_admin_check(chatid)) {
        json j({"message", "access denied"});
        body = j;
      } else
        try {
          std::unique_ptr<sql::PreparedStatement> chatStmt(
              conn->prepareStatement("DELETE FROM `Chat` WHERE `id` = ?"));
          chatStmt->setInt(1, chatid);
          chatStmt->executeUpdate();
          chatStmt.reset(conn->prepareStatement(
              "DELETE FROM `result_table` WHERE `chat_id` = ?"));
          chatStmt->executeUpdate();
          body["message"] = "Chat deleted";
          ResponseCreator resp(body, 0);
          return resp.get_resp();
        } catch (sql::SQLException &e) {
          json body(exception_handler(e));
          ResponseCreator resp(body, 1);
          return resp.get_resp();
        }
    }
  }

  http::response<http::string_body> put() override {
    return boost::beast::http::response<http::string_body>();
  }
};

#endif  // PLUS_PROJECT_COMMONCHATVIEW_H
