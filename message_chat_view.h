#ifndef PLUS_PROJECT_MESSAGE_CHAT_VIEW_H
#define PLUS_PROJECT_MESSAGE_CHAT_VIEW_H

#include "view.h"

class Message_chat_view : public View {
public:
    Message_chat_view(http::request<http::string_body> _req,
                      std::shared_ptr<sql::Connection> _conn, int userId) : View(_req, _conn, userId) {}

    http::response<http::string_body> get() override {
        json j = json::parse(req.body());
        int messid;
        std::string text;
        int author_id = -1;
        if (j.contains("messid")) {
            messid = j.at("messid");
            std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                    "select body,author_id from Message where id=?"));
            stmt->setInt(1, messid);
            std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
            while (res->next()) {
                text = res->getString("body");
                author_id = res->getInt("author_id");
            }
        }
        nlohmann::json body;
        body["status"] = 200;
        body["message"] = "OK";
        body["text"] = text;
        body["author_id"] = author_id;
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

    http::response<http::string_body> post() override {
        json j = json::parse(req.body());
        std::string text;
        int chatid;
        if (j.contains("message") && j.contains("chatid")) {
            text = j.at("message");
            chatid = j.at("chatid");
            std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                    "INSERT INTO `Message` (`publication_date`, `body`, `author_id`, `chat_id`) VALUES (NOW(), ?, ?, ?)"));
            stmt->setString(1, text);
            stmt->setInt(2, userId);
            stmt->setInt(3, chatid);
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
        int messid;
        int chatid;
        if (j.contains("messid") && j.contains("chatid")) {
            messid = j.at("messid");
            chatid = j.at("chatid");
            std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                    "DELETE FROM `Message` WHERE `id` = ?"));
            stmt->setInt(1, messid);
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

    http::response<http::string_body> put() override {
        json j = json::parse(req.body());
        int messid;
        int chatid;
        std::string text;
        if (j.contains("message") && j.contains("chatid") && j.contains("messid")) {
            messid = j.at("messid");
            chatid = j.at("chatid");
            text = j.at("message");
            std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                    "UPDATE `Message`  SET `body` = ? WHERE `id` = ?"));
            stmt->setString(1, text);
            stmt->setInt(1, messid);
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


#endif //PLUS_PROJECT_MESSAGE_CHAT_VIEW_H
