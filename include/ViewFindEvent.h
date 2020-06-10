#ifndef TECHPROJECT_VIEWFINDEVENT_H
#define TECHPROJECT_VIEWFINDEVENT_H

#include "View.h"

class ViewFindEvent : public View {
 public:
  ViewFindEvent(const http::request<http::string_body> &_req,
                const std::shared_ptr<sql::Connection> &_conn, int _userId);

  //  Поиск ивента по вхождиню подстроки
  //    Params: key
  http::response<http::string_body> get() override;

  // Заглушка
  http::response<http::string_body> post() override;

  // Заглушка
  http::response<http::string_body> delete_() override;

  // Заглушка
  http::response<http::string_body> put() override;

  ~ViewFindEvent() override = default;
};

#endif  // TECHPROJECT_VIEWFINDEVENT_H
