#include "CommonChatView.h"

http::response<http::string_body> ViewChatCommon::post() {
    json j = json::parse(req.body());
    std::string title;
    std::vector<int> members_list, admin_list;
    try {
        if (j.contains("title") && j.contains("members_list") && j.contains("admin_list")) {
            json jsonik = j.at("members_list");
            title = j.at("title");
            if (jsonik.is_array()) {
                for (auto &element : jsonik)
                    members_list.push_back(element);
            }
            jsonik = j.at("admin_list");
            if (jsonik.is_array()) {
                for (auto &element : jsonik)
                    admin_list.push_back(element);
            }
            std::unique_ptr<sql::PreparedStatement> chatStmt;
            std::unique_ptr<sql::ResultSet> res;
            if (j.contains("event_id")) {
                int event_id = j.at("event_id");
                chatStmt.reset(conn->prepareStatement(
                        "select id from event where id=?"));///check that exsist event
                chatStmt->setInt(1, event_id);
                res.reset(chatStmt->executeQuery());
                if (!res->next())
                    throw "not found this event";
                chatStmt.reset(conn->prepareStatement(
                        "select admin from event where admin=? and id=?"));///проверяем что есть права админа
                chatStmt->setInt(1, userId);
                chatStmt->setInt(2, event_id);
                res.reset(chatStmt->executeQuery());
                if (res->next()) {
                    chatStmt.reset(conn->prepareStatement(
                            "insert into chat (id,create_date, title,event_id) values (null,default ,?,? )"));///create chat
                    chatStmt->setString(1, title);
                    chatStmt->setInt(2, event_id);
                    chatStmt->execute();
                } else {
                    throw "to create that chat you must to be admin of the event";
                }
            } else {
                chatStmt.reset(conn->prepareStatement(
                        "insert into chat (id,create_date, title,event_id) values (null,default ,?,null )"));///create chat
                chatStmt->setString(1, title);
                chatStmt->execute();
            }
            chatStmt.reset(conn->prepareStatement(
                    "select id from chat where title like ? order by create_date desc"));
            chatStmt->setString(1, title);
            res.reset(chatStmt->executeQuery());
            int chat_id;
            if (res->next())
                chat_id = res->getInt("id");
            else
                throw "error with search data into database";
            chatStmt.reset(conn->prepareStatement(
                    "INSERT INTO result_table (`chat_id`, `user_id`, `is_admin`) VALUES (?, ?, 1)"));
            for (auto adminid:admin_list) {
                chatStmt->setInt(1, chat_id);
                chatStmt->setInt(2, adminid);
                chatStmt->execute();
            }
            chatStmt.reset(conn->prepareStatement(
                    "INSERT INTO result_table (`chat_id`, `user_id`, `is_admin`) VALUES (?, ?, 0)"));
            for (auto memberid:members_list) {
                chatStmt->setInt(1, chat_id);
                chatStmt->setInt(2, memberid);
                chatStmt->execute();
            }
            json body;
            body["message"] = "chat created";
            ResponseCreator resp(body, 0);
            return resp.get_resp();
        } else
            throw "invalid list of arguments";
    }
    catch (sql::SQLException &e) {
        json body(exception_handler(e));
        ResponseCreator resp(body, 1);
        return resp.get_resp();
    }
    catch (const char *e) {
        json body;
        body["message"] = e;
        ResponseCreator resp(body, 1);
        return resp.get_resp();
    }
}

http::response<http::string_body> ViewChatCommon::get() {
    std::map<std::string, std::string> params_list;
    std::cmatch result;
    boost::string_view path = req.target();
    std::string path_str = path.to_string();
    std::regex regexik("/chat/(history|members_list|members_count)\\?chatid=(\\d+)");
    if (std::regex_match(path_str.c_str(), result, regexik)) {
        for (int i = 0; i < result.size(); i++) {
            if (i == 1)
                params_list.insert({"search_data", result[i]});
            if (i == 2)
                params_list.insert({"chatid", result[i]});
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
        if (jsonik.is_string())
            search_data = j.at("search_data");
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

json ViewChatCommon::history(const int chatid, int &error) {
    if (not permission_member_check(chatid)) {
        json j({"message", "access denied"});
        return j;
    }
    try {
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                "select login nick,body,publication_date pubdate from message, user where chat_id = ? and author_id = user.id order by publication_date asc;"));
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
    catch (sql::SQLException &e) {
        error = 1;
        return exception_handler(e);
    }
}

json ViewChatCommon::members_count(const int chatid, int &error) {
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
        if (res->next())
            members_count = res->getInt(1);
        json j;
        j["members_count"] = members_count;
        return std::move(j);
    }
    catch (sql::SQLException &e) {
        error = 1;
        return exception_handler(e);
    }
}

http::response<http::string_body> ViewChatCommon::delete_() {
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
                std::unique_ptr<sql::PreparedStatement> chatStmt(conn->prepareStatement(
                        "DELETE FROM chat WHERE `id` = ?"));
                chatStmt->setInt(1, chatid);
                chatStmt->executeUpdate();
                chatStmt.reset(conn->prepareStatement(
                        "DELETE FROM `result_table` WHERE `chat_id` = ?"));
                chatStmt->executeUpdate();
                body["message"] = "chat deleted";
                ResponseCreator resp(body, 0);
                return resp.get_resp();
            }
            catch (sql::SQLException &e) {
                json body(exception_handler(e));
                ResponseCreator resp(body, 1);
                return resp.get_resp();
            }
    }


}

json ViewChatCommon::members_list(const int chatid, int &error) {
    if (not permission_member_check(chatid)) {
        json j({"message", "access denied"});
        return j;
    }
    try {
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                "select user_id id, login nick from result_table, user where user_id=user.id and chat_id=?"));
        stmt->setInt(1, chatid);
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
        std::map<int, std::string> id_nick_array;
        while (res->next()) {
            id_nick_array[res->getInt("id")] = res->getString("nick");
        }
        json j(id_nick_array);
        return std::move(j);
    }
    catch (sql::SQLException &e) {
        error = 1;
        return exception_handler(e);
    }
}
