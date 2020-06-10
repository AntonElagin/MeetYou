#ifndef TECHPROJECT_VIEWEVENTFOLLOW_H
#define TECHPROJECT_VIEWEVENTFOLLOW_H

#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <nlohmann/json.hpp>
#include "View.h"

class ViewEventFollow : public View {
 private:
 public:
  ViewEventFollow(const http::request<http::string_body> &_req,
                  const std::shared_ptr<sql::Connection> &_conn, int _userId);

  // Заглушка
  http::response<http::string_body> get() override;

  //  Подписаться на event
  //      Params: event_id
  http::response<http::string_body> post() override;

  //  Отписаться от event
  //      Params: event_id
  http::response<http::string_body> delete_() override;

  //  Заглушка
  http::response<http::string_body> put() override;

  ~ViewEventFollow() override = default;
};

#endif  // TECHPROJECT_VIEWEVENTFOLLOW_H
