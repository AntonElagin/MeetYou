#ifndef PLUS_PROJECT_VIEW_H
#define PLUS_PROJECT_VIEW_H

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/connection.h>
#include <boost/beast.hpp>
#include <utility>
#include <memory>

namespace http = boost::beast::http;

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

    virtual ~View() = default;
};


#endif //PLUS_PROJECT_VIEW_H
