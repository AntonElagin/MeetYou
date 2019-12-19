#include <Md5.h>
#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include "ViewUser.h"

class ViewUserTest : public testing::Test {
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
    con->setSchema("MeetYouTest");

    //
    std::unique_ptr<sql::PreparedStatement> del(con->prepareStatement(
        "Delete From user Where login = \"testlogin1321\""));
    del->executeQuery();
    //
    try {
      std::unique_ptr<sql::PreparedStatement> Stmt(con->prepareStatement(
          "Insert into user(login, password, email, name, surname, sex, "
          "birthday, location) Values (?, ?, ?, ?, ?, ?, ?, ?);"));
      Stmt->setString(1, "testlogin1321");
      Stmt->setString(2, md5("password123"));
      Stmt->setString(3, "email@kek.ru");
      Stmt->setString(4, "namename");
      Stmt->setString(5, "surname");
      Stmt->setString(6, "male");
      Stmt->setDateTime(7, "1999-01-12");
      Stmt->setString(8, "qwertyu");
      Stmt->execute();
    } catch (sql::SQLException &e) {
      auto str = e.what();
      std::cout << e.what();
    }
    std::unique_ptr<sql::PreparedStatement> stmt(
        con->prepareStatement("Select id from user where login = ?"));
    stmt->setString(1, "testlogin1321");
    std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());
    if (result->next()) userId = result->getInt(1);
  }

  void TearDown() override {
    std::unique_ptr<sql::PreparedStatement> Stmt(
        con->prepareStatement("Delete From user Where login = \"testlogin\""));
    Stmt->executeQuery();
  }

  sql::Driver *driver{};
  std::shared_ptr<sql::Connection> con;
  http::request<http::string_body> req;
  int userId;
};

TEST_F(ViewUserTest, goodPost) {
  nlohmann::json body;
  body["name"] = "qwerty1202";
  body["surname"] = "asdfgt";
  body["sex"] = "male";
  body["birthday"] = "1999-12-12";
  body["location"] = "kek check1";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUser view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "OK");
}

TEST_F(ViewUserTest, goodGet1) {
  nlohmann::json body;
  body["user_id"] = userId;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUser view(req, con, userId);
  std::string respBody = view.get().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["name"], "namename");
}

TEST_F(ViewUserTest, goodGet2) {
  nlohmann::json body;
  body["login"] = "testlogin1321";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUser view(req, con, userId);
  std::string respBody = view.get().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["name"], "namename");
}

TEST_F(ViewUserTest, badGet1) {
  nlohmann::json body;
  body["qwerty"] = "testlogin1321";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUser view(req, con, userId);
  std::string respBody = view.get().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 400);
}

TEST_F(ViewUserTest, badGetJSON) {
  nlohmann::json body;
  std::string str = "{\"ds\" : 12";
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUser view(req, con, userId);
  std::string respBody = view.get().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 400);
}

TEST_F(ViewUserTest, badPostJSON) {
  nlohmann::json body;
  std::string str = "{\"ds\" : 12";
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUser view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 400);
}

TEST_F(ViewUserTest, badPost_1) {
  nlohmann::json body;
  body["nam"] = "qwerty1202";
  body["surname"] = "asdfgt";
  body["sex"] = "male";
  body["birthday"] = "1999-12-12";
  body["location"] = "kek check";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUser view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid params or params count");
}

TEST_F(ViewUserTest, badPost_2) {
  nlohmann::json body;
  body["name"] = "qwerty1202";
  body["surnam"] = "asdfgt";
  body["sex"] = "male";
  body["birthday"] = "1999-12-12";
  body["location"] = "kek check";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUser view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid params or params count");
}

TEST_F(ViewUserTest, badPost_3) {
  nlohmann::json body;
  body["name"] = "qwerty1202";
  body["surname"] = "asdfgt";
  body["sex11"] = "male";
  body["birthday"] = "1999-12-12";
  body["location"] = "kek check";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUser view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid params or params count");
}

TEST_F(ViewUserTest, badPost_4) {
  nlohmann::json body;
  body["name"] = "qwerty1202";
  body["surname"] = "asdfgt";
  body["sex"] = "male";
  body["birthday34"] = "1999-12-12";
  body["location"] = "kek check";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUser view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid params or params count");
}

TEST_F(ViewUserTest, badPost_5) {
  nlohmann::json body;
  body["name"] = "qwerty1202";
  body["surname"] = "asdfgt";
  body["sex"] = "male";
  body["birthday"] = "1999-12-12";
  body["locationqwe"] = "kek check";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUser view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid params or params count");
}

TEST_F(ViewUserTest, badPost_name) {
  nlohmann::json body;
  body["name"] = "q";
  body["surname"] = "asdfgt";
  body["sex"] = "male";
  body["birthday"] = "1999-12-12";
  body["location"] = "kek check";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUser view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid name");
}

TEST_F(ViewUserTest, badPost_surname) {
  nlohmann::json body;
  body["name"] = "qsdas";
  body["surname"] = "a";
  body["sex"] = "male";
  body["birthday"] = "1999-12-12";
  body["location"] = "kek check";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUser view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid surname");
}

TEST_F(ViewUserTest, badPost_sex) {
  nlohmann::json body;
  body["name"] = "qsdas";
  body["surname"] = "adsdssd";
  body["sex"] = "maled";
  body["birthday"] = "1999-12-12";
  body["location"] = "kek check";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUser view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid sex");
}

TEST_F(ViewUserTest, badPost_birthday) {
  nlohmann::json body;
  body["name"] = "qsdas";
  body["surname"] = "adfdsd";
  body["sex"] = "male";
  body["birthday"] = "2019-12-12";
  body["location"] = "kek check";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUser view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid birthday");
}

TEST_F(ViewUserTest, badPost_birthday2) {
  nlohmann::json body;
  body["name"] = "qsdas";
  body["surname"] = "adfdsd";
  body["sex"] = "male";
  body["birthday"] = "year_month_day";
  body["location"] = "kek check";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUser view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid birthday");
}

TEST_F(ViewUserTest, badPost_location) {
  nlohmann::json body;
  body["name"] = "qsdas";
  body["surname"] = "adsdssd";
  body["sex"] = "male";
  body["birthday"] = "1999-12-12";
  body["location"] = "ke";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUser view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid location");
}