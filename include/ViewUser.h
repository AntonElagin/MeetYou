//
// Created by anton on 07.12.2019.
//

#ifndef TECHPROJECT_VIEWUSER_H
#define TECHPROJECT_VIEWUSER_H

#include "View.h"

class ViewUser : public View {
  static int validate(const std::string &name, const std::string &surname,
                      const std::string &sex, const std::string &location,
                      const std::string &birthday);

 public:
  ViewUser(const http::request<http::string_body> &_req,
           const std::shared_ptr<sql::Connection> &_conn, int _userId);

  // Просмотр user без приватных данных
  //    Params : (user_id or login)
  http::response<http::string_body> get() override;

  // Добавление user без приватных данных
  //    Params :  sex, name, surname, location, birthday
  http::response<http::string_body> post() override;

  // Заглушка
  http::response<http::string_body> delete_() override;

  // Заглушка(тк функционал совпадает с post)
  http::response<http::string_body> put() override;

  ~ViewUser() override = default;
};

#endif  // TECHPROJECT_VIEWUSER_H
