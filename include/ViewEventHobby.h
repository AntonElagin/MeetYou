#ifndef TECHPROJECT_VIEWEVENTHOBBY_H
#define TECHPROJECT_VIEWEVENTHOBBY_H

#include "View.h"

class ViewEventHobby : public View {
  bool isHobby(const std::string &value);

public:
  ViewEventHobby(const http::request<http::string_body> &_req, const std::shared_ptr<sql::Connection> &_conn,
                int _userId);
//  Лист всех ивентов
//    Params : user_id
  http::response<http::string_body> get() override;

//  Добавление хобби ивента
//    Params : hobby(array or string)
  http::response<http::string_body> post() override;

//  Удаление хобби ивента
//    Params : hobby(array or string)
  http::response<http::string_body> delete_() override;

//  Заглушка
  http::response<http::string_body> put() override;
};


#endif //TECHPROJECT_VIEWEVENTHOBBY_H
