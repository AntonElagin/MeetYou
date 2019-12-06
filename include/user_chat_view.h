#ifndef PLUS_PROJECT_USER_CHAT_VIEW_H
#define PLUS_PROJECT_USER_CHAT_VIEW_H

#include "view.h"
#include "json.hpp"

using json=nlohmann::json;

class User_chat_view : public View {
public:
    http::response<http::string_body> get() override {
        return boost::beast::http::response<http::string_body>();
    }

    http::response<http::string_body> post() override {
        json j = json::parse(req.body());
        std::string text;
        int chatid;
        int new_member_id;
        int is_admin;
        if (j.contains("new_member_id") && j.contains("chatid") && j.contains("is_admin")) {
            new_member_id = j.at("new_member_id");
            chatid = j.at("chatid");
            is_admin = j.at("is_admin");
            std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                    "INSERT INTO `result_table` (`chat_id`, `user_id`, `is_admin`) VALUES (?, ?, ?)"));
            stmt->setInt(1, chatid);
            stmt->setInt(2, new_member_id);
            stmt->setInt(3, is_admin);
            stmt->executeUpdate();
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

    http::response<http::string_body> delete_() override {
        json j = json::parse(req.body());
        std::string text;
        int chatid;
        int member_id;
        if (j.contains("member_id") && j.contains("chatid")) {
            member_id = j.at("new_member_id");
            chatid = j.at("chatid");
            std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                    "DELETE FROM `result_table` WHERE `chat_id` = ? AND `user_id` = ?"));
            stmt->setInt(1, chatid);
            stmt->setInt(2, member_id);
            stmt->executeUpdate();
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

    http::response<http::string_body> put() override {///сделать админом
        json j = json::parse(req.body());
        std::string text;
        int chatid;
        int member_id;
        int is_admin;
        if (j.contains("member_id") && j.contains("chatid") && j.contains("is_admin")) {
            member_id = j.at("member_id");
            chatid = j.at("chatid");
            is_admin = j.at("is_admin");
            std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                    "UPDATE `result_table` t SET t.`is_admin` = 1 WHERE t.`chat_id` = 3 AND t.`user_id` = 3 AND t.`is_admin` = ?"));
            stmt->setInt(1, chatid);
            stmt->setInt(2, member_id);
            stmt->setInt(3, is_admin);
            stmt->executeUpdate();
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

};


#endif //PLUS_PROJECT_USER_CHAT_VIEW_H
