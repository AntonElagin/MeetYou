//
// Created by anton on 09.12.2019.
//

#ifndef TECHPROJECT_VIEWEVENT_H
#define TECHPROJECT_VIEWEVENT_H

#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <nlohmann/json.hpp>
#include "View.h"

class ViewEvent : public View {
 private:
  int validate(const std::string &name, const std::string &type,
               const std::string &description, const std::string &_date);

 public:
  ViewEvent(const http::request<http::string_body> &_req,
            const std::shared_ptr<sql::Connection> &_conn, int _userId);

  // Получаем событие
  //    Params: name
  http::response<http::string_body> get() override;

  // Создаем события
  //    Params: name, type, description, date
  http::response<http::string_body> post() override;

  // Заглушка
  http::response<http::string_body> delete_() override;

  // Обнавляем данные события
  //    Params: (name or event_id), type, description, date
  http::response<http::string_body> put() override;

  ~ViewEvent() override = default;
};

#endif  // TECHPROJECT_VIEWEVENT_H
