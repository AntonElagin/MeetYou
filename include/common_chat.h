#ifndef PLUS_PROJECT_COMMON_CHAT_H
#define PLUS_PROJECT_COMMON_CHAT_H

#include <boost/beast.hpp>
#include <regex>
#include <memory>
#include <boost/beast.hpp>
#include <utility>
#include "json.hpp"
#include "view.h"


namespace http = boost::beast::http;

using json=nlohmann::json;

class ViewChatCommon : public View {
public:
    ViewChatCommon(http::request<http::string_body> _req,
                   std::shared_ptr<sql::Connection> _conn, int userId) : View(_req, _conn, userId) {}

    http::response<http::string_body> post() override {
        json j = json::parse(req.body());
        std::string title;
        std::vector<int> members_list, admin_list;
        if (j.contains("title") && j.contains("members_list") && j.contains("admin_list")) {
            json jsonik = j.at("members_list");
            if (jsonik.is_array()) {
                for (auto &element : jsonik)
                    members_list.push_back(element);
            }
            jsonik = j.at("admin_list");
            if (jsonik.is_array()) {
                for (auto &element : jsonik)
                    admin_list.push_back(element);
            }
            std::unique_ptr<sql::PreparedStatement> chatStmt(conn->prepareStatement(
                    "insert into Chat (id,create_date, title) values (null,CURDATE() ,? )"));
            chatStmt->setString(1, title);
            chatStmt->executeUpdate();
            chatStmt.reset(conn->prepareStatement(
                    "select id from Chat where title like ? order by create_date desc"));
            chatStmt->setString(1, title);
            std::unique_ptr<sql::ResultSet> res(chatStmt->executeQuery());
            int chat_id = -1;
            if (res->next())
                chat_id = res->getInt("id");
            std::unique_ptr<sql::PreparedStatement> temp_table_Stmt(conn->prepareStatement(
                    "INSERT INTO result_table (`chat_id`, `user_id`, `is_admin`) VALUES (?, ?, 1)"));
            for (auto adminid:admin_list) {
                temp_table_Stmt->setInt(1, chat_id);
                temp_table_Stmt->setInt(2, adminid);
            }
            temp_table_Stmt.reset(conn->prepareStatement(
                    "INSERT INTO result_table (`chat_id`, `user_id`, `is_admin`) VALUES (?, ?, 0)"));
            for (auto memberid:members_list) {
                temp_table_Stmt->setInt(1, chat_id);
                temp_table_Stmt->setInt(2, memberid);
            }
            nlohmann::json body;
            body["status"] = 200;
            body["message"] = "OK";
            boost::beast::http::response<http::string_body> resp{
                    std::piecewise_construct,
                    std::make_tuple(std::move(body.dump())),
                    std::make_tuple(http::status::ok, req.version())};
            resp.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            resp.set(http::field::content_type, "application/json");
            resp.content_length(body.size());
            resp.prepare_payload();
            return resp;
        }
    }

    http::response<http::string_body> get() override {
        json j = json::parse(req.body());
        json res_body;
        int chatid;
        std::string search_data;
        if (j.contains("chatid") && j.contains("search_data")) {
            json jsonik = j.at("chatid");
            if (jsonik.is_number())
                chatid = j.at("chatid");
            jsonik = j.at("search_data");
            if (jsonik.is_string())
                search_data = j.at("search_data");
            if (search_data == "history")
                res_body = history(chatid);
            else if (search_data == "members_count")
                res_body = members_count(chatid);
            else if (search_data == "members_list")
                res_body = members_list(chatid);
            nlohmann::json body;
            body["status"] = 200;
            body["message"] = "OK";
            body["content"] = res_body;
            boost::beast::http::response<http::string_body> resp{
                    std::piecewise_construct,
                    std::make_tuple(std::move(body.dump())),
                    std::make_tuple(http::status::ok, req.version())};
            resp.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            resp.set(http::field::content_type, "application/json");
            resp.content_length(body.size());
            resp.prepare_payload();
            return resp;
        }
        return boost::beast::http::response<http::string_body>();
    }

    json history(const int chatid) {
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                "select username nick,body,publication_date pubdate from Message, User where chat_id = ? and author_id = User.id order by publication_date asc;"));
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
    }

    json members_count(const int chatid) {
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                "select count(user_id) quant from result_table where chat_id=?"));
        stmt->setInt(1, chatid);
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
        res->next();
        int members_count = res->getInt(1);
        json j;
        j["members_count"] = members_count;
        return j;
    }

    json members_list(const int chatid) {
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                "select user_id id, username nick from result_table, User where user_id=User.id and chat_id=?"));
        stmt->setInt(1, chatid);
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
        std::map<int, std::string> id_nick_array;
        while (res->next()) {
            id_nick_array[res->getInt("id")] = res->getString("nick");
        }
        json j(id_nick_array);
        return j;
    }

    http::response<http::string_body> delete_() override {
        json j = json::parse(req.body());
        int chatid;
        if (j.contains("chatid")) {
            json jsonik = j.at("chatid");
            if (jsonik.is_number()) {
                chatid = j.at("chatid");
            }
            std::unique_ptr<sql::PreparedStatement> chatStmt(conn->prepareStatement(
                    "DELETE FROM `Chat` WHERE `id` = ?"));
            chatStmt->setInt(1, chatid);
            chatStmt->executeUpdate();
            chatStmt.reset(conn->prepareStatement(
                    "DELETE FROM `result_table` WHERE `chat_id` = ?"));
            chatStmt->executeUpdate();
        }
        nlohmann::json body;
        body["status"] = 200;
        body["message"] = "Chat deleted";
        boost::beast::http::response<http::string_body> resp{
                std::piecewise_construct,
                std::make_tuple(std::move(body.dump())),
                std::make_tuple(http::status::ok, req.version())};
        resp.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        resp.set(http::field::content_type, "application/json");
        resp.content_length(body.size());
        resp.prepare_payload();
        return resp;
    }

    http::response<http::string_body> put() override {
        return boost::beast::http::response<http::string_body>();
    }
};

#endif //PLUS_PROJECT_COMMON_CHAT_H
