#ifndef TECHPROJECT_VIEW_REGISTRATION_H
#define TECHPROJECT_VIEW_REGISTRATION_H

#include <boost/beast.hpp>
#include <nlohmann/json.hpp>
#include "view.h"

class ViewRegistration : public View {
 public:
  ViewRegistration(const http::request<http::string_body>& _req,
                   const std::shared_ptr<sql::Connection>& _conn);

  http::response<http::string_body> get() override;
  http::response<http::string_body> post() override;
  http::response<http::string_body> delete_() override;
  http::response<http::string_body> put() override;

  virtual ~ViewRegistration() = default;
};

#endif  // TECHPROJECT_VIEW_REGISTRATION_H
