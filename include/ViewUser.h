//
// Created by anton on 07.12.2019.
//

#ifndef TECHPROJECT_VIEWUSER_H
#define TECHPROJECT_VIEWUSER_H

#include "View.h"

class ViewUser : public View {
public:
  ViewUser(const http::request<http::string_body> &_req,
            const std::shared_ptr<sql::Connection> &_conn, int _userId);

  http::response<http::string_body> get() override;
  http::response<http::string_body> post() override;
  http::response<http::string_body> delete_() override;
  http::response<http::string_body> put() override;

  ~ViewUser() override = default;
};


#endif //TECHPROJECT_VIEWUSER_H
