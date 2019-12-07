//#include "gmock/http_session.h"
#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include "ViewOther.h"
#include "ViewRegistration.h"
#include "gmock/GHttpSession.h"
#include "gmock/Gistener.h"

class ViewTest : public testing::Test {
 protected:
  void SetUp() override {
    req.version(11);
    req.set(http::field::user_agent, "test");
    req.set(http::field::host, "localhost");
    req.prepare_payload();
    driver = get_driver_instance();
    std::shared_ptr<sql::Connection> conn(
        driver->connect("tcp://127.0.0.1:3306", "root", "12A02El99"));
    con = conn;
    con->setSchema("MeetYou");
    req.target("/auth");
  }

  void TearDown() override {
    std::unique_ptr<sql::Statement> st(con->createStatement());
    //    st->executeQuery("Delete from user where login = 'Cheburashka';");
    st->execute("Delete from user where login = 'Cheburashka';");
  }

  sql::Driver *driver;
  std::shared_ptr<sql::Connection> con;
  http::request<http::string_body> req;
};

TEST_F(ViewTest, Registeration) {
  nlohmann::json body;
  body["login"] = "Cheburashka";
  body["password"] = "Henna02";
  body["email"] = "hepoklak@zlo.me";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "OK");
}

TEST_F(ViewTest, badRegisteration) {
  nlohmann::json body;
  body["logi"] = "Cheburashka";
  body["password"] = "Henna02";
  body["email"] = "hepoklak@zlo.me";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "JSON error");
}

TEST_F(ViewTest, badEmailRegisteration) {
  nlohmann::json body;
  body["login"] = "Cheburashka";
  body["password"] = "Henna02";
  body["email"] = "hepoklak@zlo";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["email_message"], "Invalid");
}

TEST_F(ViewTest, badLoginRegisteration) {
  nlohmann::json body;
  body["login"] = "Ch";
  body["password"] = "Henna02";
  body["email"] = "hepoklak@zlo.me";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["login_message"], "Invalid");
}
TEST_F(ViewTest, badPasswordRegisteration) {
  nlohmann::json body;
  body["login"] = "Cheburashka";
  body["password"] = "He";
  body["email"] = "hepoklak@zlo.me";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["password_message"], "Invalid");
}