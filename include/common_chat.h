#ifndef PLUS_PROJECT_COMMON_CHAT_H
#define PLUS_PROJECT_COMMON_CHAT_H

#include <boost/beast.hpp>
#include <regex>
#include <memory>
#include <cppconn/connection.h>
#include <boost/beast.hpp>
#include <utility>
#include "json.hpp"
#include "view.h"

namespace http = boost::beast::http;


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
        return boost::beast::http::response<http::string_body>();
    }

    http::response<http::string_body> delete_() override {
        return boost::beast::http::response<http::string_body>();
    }

    http::response<http::string_body> put() override {
        return boost::beast::http::response<http::string_body>();
    }


};


#endif //PLUS_PROJECT_COMMON_CHAT_H
