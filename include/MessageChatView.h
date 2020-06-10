#ifndef PLUS_PROJECT_MESSAGECHATVIEW_H
#define PLUS_PROJECT_MESSAGECHATVIEW_H

#include "View.h"

using json = nlohmann::json;

class ViewMessageChat : public View {
 public:
  ViewMessageChat(http::request<http::string_body> _req,
                  std::shared_ptr<sql::Connection> _conn, int userId)
      : View(_req, _conn, userId) {}

  http::response<http::string_body> get() override ;

  http::response<http::string_body> post() override ;

  http::response<http::string_body> delete_() override ;

  http::response<http::string_body> put() override ;

  bool permission_owner_check(const int messageid) ;
};

#endif  // PLUS_PROJECT_MESSAGECHATVIEW_H
