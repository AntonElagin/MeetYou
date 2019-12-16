#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include <Md5.h>
#include "ViewOther.h"
#include "ViewRegistration.h"


class ViewRegTest : public testing::Test {
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
    std::unique_ptr<sql::PreparedStatement> Stmt1(
        con->prepareStatement("Delete From user Where login = \"Cheburashka\""));
    Stmt1->executeQuery();
    std::unique_ptr<sql::PreparedStatement> Stmt2(
        con->prepareStatement("Delete From user Where email = \"email@email.ru\""));
    Stmt2->executeQuery();
    std::unique_ptr<sql::PreparedStatement> Stmt(
        con->prepareStatement("Insert into user(login, password, email) Values (\"login12\", ?, \"email@email.ru\");"));
    Stmt->setString(1, md5("qwerty1234"));
    Stmt->executeQuery();


  }

  void TearDown() override {
    std::unique_ptr<sql::PreparedStatement> Stmt(
        con->prepareStatement("Delete From user Where login = \"Cheburashka\""));
    std::unique_ptr<sql::PreparedStatement> Stmt2(con->prepareStatement("Delete From user Where login = \"login12\""));
    Stmt->executeQuery();
    Stmt2->executeQuery();
  }

  sql::Driver *driver{};
  std::shared_ptr<sql::Connection> con;
  http::request<http::string_body> req;
};

TEST_F(ViewRegTest, Registeration) {
  nlohmann::json body;
  body["login"] = "Cheburashka";
  body["password"] = "Henna02";
  body["email"] = "hepoklak@zlo.me";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1, "8.128.10.1");
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "OK");
}


TEST_F(ViewRegTest, duplicateRegisteration1) {
  nlohmann::json body;
  body["login"] = "login12";
  body["password"] = "Henna02";
  body["email"] = "email@mawel.ru";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1, "8.128.10.1");
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Duplicate login");
}


TEST_F(ViewRegTest, duplicateRegisteration2) {
  nlohmann::json body;
  body["login"] = "login12187y";
  body["password"] = "Henna02";
  body["email"] = "email@email.ru";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1, "8.128.10.1");
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Duplicate email");
}

TEST_F(ViewRegTest, duplicateRegisteration3) {
  nlohmann::json body;
  body["login"] = "login12";
  body["password"] = "Henna02";
  body["email"] = "email@email.ru";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1, "8.128.10.1");
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Duplicate login and email");
}

TEST_F(ViewRegTest, badRegisterationJSON) {
  nlohmann::json body;
  std::string str = "{ \"ds\": 34";
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1, "8.128.10.1");
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "JSON error");
}


TEST_F(ViewRegTest, badLoginJSON) {
  nlohmann::json body;
  std::string str = "{ \"ds\": 34";
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1, "8.128.10.1");
  std::string respBody = view.get().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 400);
}

TEST_F(ViewRegTest, badLogin) {
  nlohmann::json body;
  body["email"] = "login12";
  body["password"] = "qwerty1234";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1, "8.128.10.1");
  std::string respBody = view.get().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 400);
}

TEST_F(ViewRegTest, LoginWithLogin) {
  nlohmann::json body;
  body["login"] = "login12";
  body["password"] = "qwerty1234";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1, "8.128.10.1");
  std::string respBody = view.get().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 200);
}


TEST_F(ViewRegTest, LoginWithEmail) {
  nlohmann::json body;
  body["login"] = "email@email.ru";
  body["password"] = "qwerty1234";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1, "8.128.10.1");
  std::string respBody = view.get().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 200);
}

TEST_F(ViewRegTest, badParamReg) {
  nlohmann::json body;
  body["logi"] = "Cheburashka";
  body["password"] = "Henna02";
  body["email"] = "hepoklak@zlo.me";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1, "8.128.10.1");
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 400);
}

TEST_F(ViewRegTest, badParamsReg) {
  nlohmann::json body;
  body["login"] = "Cheburashka";
  body["passwor"] = "Henna02";
  body["email"] = "hepoklak@zlo.me";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1, "8.128.10.1");
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 400);
}

TEST_F(ViewRegTest, badParamsReg2) {
  nlohmann::json body;
  body["login"] = "Cheburashka";
  body["password"] = "Henna02";
  body["emai"] = "hepoklak@zlo.me";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1, "8.128.10.1");
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 400);
}

TEST_F(ViewRegTest, badEmailRegisteration) {
  nlohmann::json body;
  body["login"] = "Cheburashka";
  body["password"] = "Henna02";
  body["email"] = "hepoklak@zlo";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1, "8.128.10.1");
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 400);
}

TEST_F(ViewRegTest, badLoginRegisteration) {
  nlohmann::json body;
  body["login"] = "Ch";
  body["password"] = "Henna02";
  body["email"] = "hepoklak@zlo.me";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1, "8.128.10.1");
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid login");
}

TEST_F(ViewRegTest, badPasswordRegisteration) {
  nlohmann::json body;
  body["login"] = "Cheburashka";
  body["password"] = "He";
  body["email"] = "hepoklak@zlo.me";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewRegistration view(req, con, -1, "8.128.10.1");
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid password");
}