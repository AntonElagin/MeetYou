#ifndef TECHPROJECT_VIEWFOLLOW_H
#define TECHPROJECT_VIEWFOLLOW_H

#include "View.h"
#include <nlohmann/json.hpp>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/connection.h>

class ViewFollow : public View {
private:

public:
  ViewFollow(const http::request<http::string_body> &_req,
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

  ~ViewFollow() override = default;
};

#endif //TECHPROJECT_VIEWFOLLOW_H
