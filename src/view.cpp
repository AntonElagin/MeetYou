#include "../include/view.h"

#include <utility>

View::View(http::request<http::string_body> _req,
           std::shared_ptr<sql::Connection> _conn)
    : req(std::move(_req)), conn(std::move(_conn)) {}
