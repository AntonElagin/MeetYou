//
// Created by anton on 12.12.2019.
//

#ifndef TECHPROJECT_VIEWFOLLOW_H
#define TECHPROJECT_VIEWFOLLOW_H

#include "View.h"
#include <nlohmann/json.hpp>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/connection.h>

class ViewFollow : public View  {
private:

public:
  ViewFollow(const http::request<http::string_body> &_req,
            const std::shared_ptr<sql::Connection> &_conn, int _userId);

  http::response<http::string_body> get() override;
  http::response<http::string_body> post() override;
  http::response<http::string_body> delete_() override;
  http::response<http::string_body> put() override;

  ~ViewFollow() override = default;
};

#endif //TECHPROJECT_VIEWFOLLOW_H
