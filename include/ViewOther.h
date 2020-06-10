//
// Created by anton on 06.12.2019.
//

#ifndef TECHPROJECT_VIEWOTHER_H
#define TECHPROJECT_VIEWOTHER_H

#include "View.h"

class ViewOther : public View {
 private:
  http::response<http::string_body> badRequestPlug();

 public:
  ViewOther(const http::request<http::string_body> &_req,
            const std::shared_ptr<sql::Connection> &_conn, int _userId);

  http::response<http::string_body> get() override;
  http::response<http::string_body> post() override;
  http::response<http::string_body> delete_() override;
  http::response<http::string_body> put() override;

  ~ViewOther() override = default;
};

#endif  // TECHPROJECT_VIEWOTHER_H
