#ifndef TECHPROJECT_VIEW_H
#define TECHPROJECT_VIEW_H

#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>
#include <utility>
#include "ResponseCreater.h"
//#include "json.hpp"

namespace http = boost::beast::http;
using json = nlohmann::json;

// template <class Body, class Allocator>
class View {
protected:
    std::shared_ptr<sql::Connection> conn;
    http::request<http::string_body> req;
    int userId;

    http::response<http::string_body> defaultPlug();

    http::response<http::string_body> templateReturn(int status,
                                                     const std::string &message);

public:
    View(http::request<http::string_body> _req,
         std::shared_ptr<sql::Connection> _conn, int _userId);

    virtual http::response<http::string_body> get() = 0;

    virtual http::response<http::string_body> post() = 0;

    virtual http::response<http::string_body> delete_() = 0;

    virtual http::response<http::string_body> put() = 0;

    json exception_handler(sql::SQLException &e);

    bool permission_member_check(const int chatid) ;

    bool permission_admin_check(const int chatid) ;

    virtual ~View() = default;
};

#endif  // TECHPROJECT_VIEW_H
