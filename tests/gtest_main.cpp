#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "../include/View.h"
#include <boost/beast.hpp>
#include <map>
#include "../include/bd_tables_conectors.h"

TEST(auth_request, is_auth) {
  auth_request<http::empty_body, http::basic_fields<std::allocator<char>>> req;
  req.version(11);
  req.method(boost::beast::http::verb::head);
  boost::beast::string_view str = reinterpret_cast<const char *>('/str');
  req.target(str);
  req.set(boost::beast::http::field::user_agent, "test");
  req.set(boost::beast::http::field::set_cookie,'valid_token');

  req.set(boost::beast::http::field::host, "localhost");
  auth_request<boost::beast::http::empty_body, boost::beast::http::fields> auth(req);
  ASSERT_TRUE(auth.is_auth());
}

TEST(auth_request, return_id) {
  auth_request<http::empty_body, http::basic_fields<std::allocator<char>>> req;
  req.version(11);
  req.method(boost::beast::http::verb::head);
  boost::beast::string_view str = reinterpret_cast<const char *>('/str');
  req.target(str);
  req.set(boost::beast::http::field::user_agent, "test");
  req.set(boost::beast::http::field::set_cookie,'valid_token');

  req.set(boost::beast::http::field::host, "localhost");
  auth_request<boost::beast::http::empty_body, boost::beast::http::fields> auth(req);
  auth.is_auth();
//  ASSERT_TRUE(auth.is_auth());
  ASSERT_GE(auth.get_user_id(),1);
}

TEST(db, auth_user) {
  auth_user user;
  ASSERT_TRUE(user.is_login_valid("1231","12312"));
}

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


