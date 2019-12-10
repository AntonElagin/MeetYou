//
// Created by anton on 09.12.2019.
//

#ifndef TECHPROJECT_VIEWEVENT_H
#define TECHPROJECT_VIEWEVENT_H

#include "View.h"
#include <nlohmann/json.hpp>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/connection.h>

class ViewEvent : public View  {
private:
  bool isName(const std::string &value);
  bool isDescription(const std::string &value);
public:
  ViewEvent(const http::request<http::string_body> &_req,
            const std::shared_ptr<sql::Connection> &_conn, int _userId);

  http::response<http::string_body> get() override;
  http::response<http::string_body> post() override;
  http::response<http::string_body> delete_() override;
  http::response<http::string_body> put() override;

  ~ViewEvent() override = default;
};


#endif //TECHPROJECT_VIEWEVENT_H
