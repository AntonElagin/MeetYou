#ifndef PLUS_PROJECT_USERCHATVIEW_H
#define PLUS_PROJECT_USERCHATVIEW_H

#include "View.h"

using json = nlohmann::json;

class ViewUserChat : public View {
 public:
  ViewUserChat(http::request<http::string_body> _req,
               std::shared_ptr<sql::Connection> _conn, int userId)
      : View(_req, _conn, userId) {}

  http::response<http::string_body> get() override {
    return boost::beast::http::response<http::string_body>();
  }

  http::response<http::string_body> post() override ;

  http::response<http::string_body> delete_() override ;

  http::response<http::string_body> put() override;
};

#endif  // PLUS_PROJECT_USERCHATVIEW_H
