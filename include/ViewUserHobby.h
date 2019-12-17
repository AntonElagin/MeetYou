#ifndef TECHPROJECT_VIEWUSERHOBBY_H
#define TECHPROJECT_VIEWUSERHOBBY_H

#include "View.h"

class ViewUserHobby : public View {
  bool isHobby(const std::string &value);
public:
  ViewUserHobby(const http::request<http::string_body> &_req, const std::shared_ptr<sql::Connection> &_conn,
                int _userId);
//  Лист всех хобби юзера
//    Params : user_id
  http::response<http::string_body> get() override;

//  Добавление хобби юзеру
//    Params : hobby(array or string)
  http::response<http::string_body> post() override;

//  Удаление хобби юзера
//    Params : hobby(array or string)
  http::response<http::string_body> delete_() override;

//  Заглушка
  http::response<http::string_body> put() override;
};


#endif //TECHPROJECT_VIEWUSERHOBBY_H
