//
// Created by anton on 17.12.2019.
//

#ifndef TECHPROJECT_VIEWUSERHOBBY_H
#define TECHPROJECT_VIEWUSERHOBBY_H

#include "View.h"

class ViewUserHobby : public View {
  bool isHobby(const std::string &value);

  http::response<http::string_body> workTemplate(const nlohmann::json &json, const std::string &sql);

public:
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
