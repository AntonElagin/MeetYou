#ifndef PLUS_PROJECT_VIEW_H
#define PLUS_PROJECT_VIEW_H

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/connection.h>
#include "cppconn/prepared_statement.h"
#include <boost/beast.hpp>
#include <utility>
#include <memory>
#include "json.hpp"
#include "ResponseCreater.h"

namespace http = boost::beast::http;
using json=nlohmann::json;

// template <class Body, class Allocator>
class View {
public:
    std::shared_ptr<sql::Connection> conn;
    http::request<http::string_body> req;
    int userId;

    View(http::request<http::string_body> _req,
         std::shared_ptr<sql::Connection> _conn, int userId) : req(_req), conn(_conn), userId(userId) {};

    virtual http::response<http::string_body> get() = 0;

    virtual http::response<http::string_body> post() = 0;

    virtual http::response<http::string_body> delete_() = 0;

    virtual http::response<http::string_body> put() = 0;

    json exception_handler(sql::SQLException &e) {
        std::map<std::string, std::string> error_map;
        std::string str = e.what();
        error_map.insert({"err", e.what()});
        error_map.insert({"MYSQL error code", std::to_string(e.getErrorCode())});
        error_map.insert({"MYSQL error code", e.getSQLState()});
        json j(error_map);
        return j;
    }

    bool permission_member_check(const int chatid) {
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                "select distinct chat_id,user_id from result_table where chat_id=? and user_id=?"));
        stmt->setInt(1, chatid);
        stmt->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
        if (res->next())
            return true;
        else return false;
    }

    bool permission_admin_check(const int chatid) {
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                "select distinct chat_id,user_id,is_admin from result_table where chat_id=? and user_id=?"));
        stmt->setInt(1, chatid);
        stmt->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
        if (res->next() && res->getInt(3))
            return true;
        else return false;
    }

    virtual ~View() = default;
};


#endif //PLUS_PROJECT_VIEW_H
