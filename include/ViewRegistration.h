#ifndef TECHPROJECT_VIEWREGISTRATION_H
#define TECHPROJECT_VIEWREGISTRATION_H

#include <boost/beast.hpp>
#include <nlohmann/json.hpp>
#include <regex>
#include "View.h"

class ViewRegistration : public View {
 public:
  ViewRegistration(const http::request<http::string_body> &_req, const std::shared_ptr<sql::Connection> &_conn,
                   int _userId, std::string _ip);

  http::response<http::string_body> get() override;
  http::response<http::string_body> post() override;
  http::response<http::string_body> delete_() override;
  http::response<http::string_body> put() override;

  virtual ~ViewRegistration() = default;

 private:
  std::string ip;

  bool isPassword(const std::string &value);
  bool isEmail(const std::string &value);
  bool isLogin(const std::string &value);
  int returnUser(const std::string &password, const std::string &value);
  static std::string randomString(int size);
  void createToken(const std::string &token, const int &userId);
};

#endif  // TECHPROJECT_VIEWREGISTRATION_H
