#ifndef TECHPROJECT_VIEWREGISTRATION_H
#define TECHPROJECT_VIEWREGISTRATION_H

#include <boost/beast.hpp>
#include <nlohmann/json.hpp>
#include <regex>
#include "View.h"

class ViewRegistration : public View {
 public:
  ViewRegistration(const http::request<http::string_body> &_req,
                   const std::shared_ptr<sql::Connection> &_conn, int _userId,
                   std::string _ip);

  // login
  //      Params: login, password
  http::response<http::string_body> get() override;

  // login
  //      Params: login, email, password
  http::response<http::string_body> post() override;

  // Заглушка
  http::response<http::string_body> delete_() override;

  //  Заглушка
  // TODO : Смена пароля
  http::response<http::string_body> put() override;

  virtual ~ViewRegistration() = default;

 private:
  std::string ip;

  int isDuplicate(const std::string &login, const std::string &email);

  bool isPassword(const std::string &value);

  bool isEmail(const std::string &value);

  bool isLogin(const std::string &value);

  int returnUser(const std::string &password, const std::string &value);

  static std::string randomString(int size);

  void createToken(const std::string &token, const int &userId);
};

#endif  // TECHPROJECT_VIEWREGISTRATION_H
