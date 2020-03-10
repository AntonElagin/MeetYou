//
// Created by anton on 20.12.2019.
//

#ifndef TECHPROJECT_VIEWUSERFOLLIST_H
#define TECHPROJECT_VIEWUSERFOLLIST_H

#include "View.h"

class ViewUserFolList : public View {
private:
public:
  ViewUserFolList(const http::request<http::string_body> &_req,
                  const std::shared_ptr<sql::Connection> &_conn, int _userId);

  // Получить свои подписки
  //      Params: NULL
  http::response<http::string_body> get() override;

  //  Подписаться на event
  //      Params: event_id
  http::response<http::string_body> post() override;

  //  Отписаться от event
  //      Params: event_id
  http::response<http::string_body> delete_() override;

  //  Заглушка
  http::response<http::string_body> put() override;

  ~ViewUserFolList() override = default;
};

#endif //TECHPROJECT_VIEWUSERFOLLIST_H
