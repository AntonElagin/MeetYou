
#include "MessageChatView.h"

http::response<http::string_body> ViewMessageChat::post() {
    json j = json::parse(req.body());
    int chatid = -1;
    json body;
    std::string text;
    try {
        if (userId < 0) throw std::invalid_argument("you must be logged");
        if (j.contains("message") && j.contains("chatid")) {
            text = j.at("message");
            chatid = j.at("chatid");
            if (not permission_member_check(chatid)) {
                throw std::domain_error("access denied");
            } else {
                std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                        "INSERT INTO `message` (`publication_date`, `body`, `author_id`, "
                        "`chat_id`) VALUES (default, ?, ?, ?)"));
                stmt->setString(1, text);
                stmt->setInt(2, userId);
                stmt->setInt(3, chatid);
                stmt->executeUpdate();
                body["status"] = "OK";
                ResponseCreator resp(body, 0);
                return resp.get_resp();
            }
        } else
            throw std::invalid_argument("invalid json args must to be field message and chatid");
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

http::response<http::string_body> ViewMessageChat::delete_() {
    json j = json::parse(req.body());
    int messid, chatid;
    json body;
    try {
        if (userId < 0) throw std::invalid_argument("you must be logged");
        if (j.contains("messid") && j.contains("chatid")) {
            messid = j.at("messid");
            chatid = j.at("chatid");
            if (not(permission_admin_check(chatid) && permission_owner_check(messid))) {
                throw std::domain_error("you must to be author of this message or admin in the chat");
            } else {
                std::unique_ptr<sql::PreparedStatement> stmt(
                        conn->prepareStatement("DELETE FROM `message` WHERE `id` = ?"));
                stmt->setInt(1, messid);
                stmt->executeUpdate();
                body["status"] = "OK";
                ResponseCreator resp(body, 0);
                return resp.get_resp();
            }
        } else
            throw std::invalid_argument("invalid json args, must to be field messid and chatid");
    } catch (std::exception &e) {
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

http::response<http::string_body> ViewMessageChat::get() {
    json j = json::parse(req.body());
    int chatid = -1;
    json body;
    std::string text;
    int author_id = -1;
    int messid;
    try {
        if (userId < 0) throw std::invalid_argument("you must be logged");
        if (j.contains("messid")) {
            messid = j.at("messid");
            if (not permission_member_check(chatid)) {
                throw std::domain_error("to show message you must to be a member of chat");
            } else {
                std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                        "select body,author_id from Message where id=?"));
                stmt->setInt(1, messid);
                std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
                if (res->next()) {
                    text = res->getString("body");
                    author_id = res->getInt("author_id");
                }
                body["text"] = text;
                body["author_id"] = author_id;
                ResponseCreator resp(body, 0);
                return resp.get_resp();
            }
        } else throw std::invalid_argument("invalid json args, must to be field messid");
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

http::response<http::string_body> ViewMessageChat::put() {
    json j = json::parse(req.body());
    int messid, chatid;
    std::string text;
    json body;
    try {
        if (userId < 0) throw std::invalid_argument("you must be logged");
        if (j.contains("message") && j.contains("chatid") && j.contains("messid")) {
            messid = j.at("messid");
            chatid = j.at("chatid");
            text = j.at("message");
            if (not(permission_admin_check(chatid) &&
                    permission_owner_check(messid))) {
                throw std::domain_error("to update message you must to be an owner of message");
            } else {
                std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                        "UPDATE `message`  SET `body` = ? WHERE `id` = ?"));
                stmt->setString(1, text);
                stmt->setInt(2, messid);
                stmt->executeUpdate();
                body["status"] = "OK";
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

bool ViewMessageChat::permission_owner_check(const int messageid) {
    std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "select distinct author_id from message where id=?"));
    stmt->setInt(1, messageid);
    stmt->setInt(1, userId);
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
    if (res->next()) {
        if (userId == res->getInt(1)) return true;
    } else
        return false;
}