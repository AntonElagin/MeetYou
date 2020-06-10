#include "View.h"

#include <utility>

View::View(http::request<http::string_body> _req,
           std::shared_ptr<sql::Connection> _conn, int _userId)
        : req(std::move(_req)), conn(std::move(_conn)), userId(_userId) {}

http::response<http::string_body> View::defaultPlug() {
    http::response<http::string_body> res;
    res.result(405);
    res.set(http::field::content_type, "json/application");
    res.keep_alive(req.keep_alive());
    nlohmann::json body;
    body["status"] = 405;
    body["message"] = "Invalid method";
    std::string s = body.dump();
    res.body() = s;
    res.set(http::field::content_length, s.length());
    return res;
}

http::response<http::string_body> View::templateReturn(
        int status, const std::string &message) {
    http::response<http::string_body> res;
    res.result(status);
    res.set(http::field::content_type, "json/application");
    res.keep_alive(req.keep_alive());
    nlohmann::json body;
    body["status"] = status;
    body["message"] = message;
    std::string s = body.dump();
    res.body() = s;
    res.set(http::field::content_length, s.length());
    return res;
}

json View::exception_handler(sql::SQLException &e) {
    std::map<std::string, std::string> error_map;
    std::string str = e.what();
    error_map.insert({"err", e.what()});
    error_map.insert({"MYSQL error code", std::to_string(e.getErrorCode())});
    error_map.insert({"MYSQL error code", e.getSQLState()});
    json j(error_map);
    return j;
}

bool View::permission_member_check(const int chatid) {
    if (permission_admin_check(chatid))
        return true;
    std::unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement("select distinct chat_id,user_id from "
                                   "result_table where chat_id=? and user_id=?"));
    stmt->setInt(1, chatid);
    stmt->setInt(2, userId);
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
    if (res->next())
        return true;
    else
        return false;
}

bool View::permission_admin_check(const int chatid) {
    std::unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement("select distinct chat_id,user_id,is_admin from "
                                   "result_table where chat_id=? and user_id=?"));
    stmt->setInt(1, chatid);
    stmt->setInt(2, userId);
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
    if (res->next() && res->getInt(3))
        return true;
    else
        return false;
}