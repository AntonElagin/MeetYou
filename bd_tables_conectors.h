//
// Created by anton on 09.11.2019.
//

#ifndef TECHPROJECT_BD_TABLES_CONECTORS_H
#define TECHPROJECT_BD_TABLES_CONECTORS_H

#include <string>

class auth_user {
  unsigned int _id;
  std::string _username;
  std::string _email;
  std::string _password;
  std::string _token;
  int _token_lifetime;
public:
  auth_user();
  unsigned int id();
  std::string username();
  std::string email();
  std::string password();
  std::string token();
  int token_lifetime();
  void get_by_id(unsigned int);
  void get_by_token(std::string token);
  void add_user(unsigned int _id, std::string _username,
      std::string _email, std::string _password,
      std::string _token, int _token_lifetime;);
  void set_password(std::string password);
  bool is_login_valid(std::string email, std::string password);
};


#endif //TECHPROJECT_BD_TABLES_CONECTORS_H
