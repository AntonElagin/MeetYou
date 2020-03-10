#include "UserChatView.h"

http::response<http::string_body> ViewUserChat::post() {
    json j = json::parse(req.body());
    std::string text;
    int chatid = -1, new_member_id, is_admin;
    json body;
    try {
        if (userId < 0) throw std::invalid_argument("you must be logged");
        if (j.contains("new_member_id") && j.contains("chat_id") &&
            j.contains("is_admin")) {
            new_member_id = j.at("new_member_id");
            chatid = j.at("chat_id");
            is_admin = j.at("is_admin");
            if (not permission_member_check(chatid)) {
                throw std::domain_error("access denied, you must to be a member of chat");
            } else {
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
            }
        } else
            throw std::invalid_argument("invalid json args must to be field: is_admin; chat_id; new_member_id;");
    }
    catch (std::exception &e) {
        body["error"] = e.what();
        ResponseCreator resp(body, 1);
        return resp.get_resp();
    }
    catch (sql::SQLException &e) {
        body = exception_handler(e);
        ResponseCreator resp(body, 1);
        return resp.get_resp();
    }

}

http::response<http::string_body> ViewUserChat::delete_() {
    json j = json::parse(req.body());
    int chatid = -1, member_id;
    json body;
    try {
        if (userId < 0) throw std::invalid_argument("you must be logged");
        if (j.contains("member_id") && j.contains("chat_id")) {
            member_id = j.at("member_id");
            chatid = j.at("chatid");
            if (not permission_admin_check(chatid)) {
                throw std::domain_error("access denied, you must to be an admin of chat");
            } else {
                std::unique_ptr<sql::PreparedStatement> stmt(
                        conn->prepareStatement("DELETE FROM `result_table` WHERE "
                                               "`chat_id` = ? AND `user_id` = ?"));
                stmt->setInt(1, chatid);
                stmt->setInt(2, member_id);
                stmt->executeUpdate();
                body["message"] = "OK";
                ResponseCreator resp(body, 0);
                return resp.get_resp();
            }
        } else
            throw std::invalid_argument("invalid json args must to be field: is_admin; chat_id; member_id;");
    }
    catch (std::exception &e) {
        body["error"] = e.what();
        ResponseCreator resp(body, 1);
        return resp.get_resp();
    }
    catch (sql::SQLException &e) {
        body = exception_handler(e);
        ResponseCreator resp(body, 1);
        return resp.get_resp();
    }

}

http::response<http::string_body> ViewUserChat::put() {  ///сделать админом
    json j = json::parse(req.body());
    int chatid = -1, member_id, is_admin;
    json body;
    try {
        if (userId < 0) throw std::invalid_argument("you must be logged");
        if (j.contains("member_id") && j.contains("chatid") &&
            j.contains("is_admin")) {
            member_id = j.at("member_id");
            chatid = j.at("chatid");
            is_admin = j.at("is_admin");
            if (not permission_admin_check(chatid)) {
                throw std::domain_error("access denied, you must to be an admin of chat");
            } else {
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
            }
        }
    }
    catch (std::exception &e) {
        body["error"] = e.what();
        ResponseCreator resp(body, 1);
        return resp.get_resp();
    }
    catch (sql::SQLException &e) {
        body = exception_handler(e);
        ResponseCreator resp(body, 1);
        return resp.get_resp();
    }

}
