//
// Created by anton on 12.12.2019.
//

#ifndef TECHPROJECT_VIEWUSERFOLLOW_H
#define TECHPROJECT_VIEWUSERFOLLOW_H



class ViewUserFollow : public View  {
private:

public:
  ViewUserFollow(const http::request<http::string_body> &_req,
             const std::shared_ptr<sql::Connection> &_conn, int _userId);

  http::response<http::string_body> get() override;
  http::response<http::string_body> post() override;
  http::response<http::string_body> delete_() override;
  http::response<http::string_body> put() override;

  ~ViewUserFollow() override = default;
};

#endif //TECHPROJECT_VIEWUSERFOLLOW_H
