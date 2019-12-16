#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include <Md5.h>
#include "ViewUserFollow.h"


class ViewUserFollowTest : public testing::Test {
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

    //
    std::unique_ptr<sql::PreparedStatement> del(
        con->prepareStatement("Delete From user Where login = \"testlogin1321\""));
    del->executeQuery();
    std::unique_ptr<sql::PreparedStatement> del2(
        con->prepareStatement("Delete From event Where name = \"event_name\""));
    del->executeQuery();
    del2->executeQuery();
//
//
    std::unique_ptr<sql::PreparedStatement> Stmt(con->prepareStatement(
        "Insert into user(login, password, email) Values (?, ?, ?);"));
    Stmt->setString(1, "testlogin1321");
    Stmt->setString(2, md5("password123"));
    Stmt->setString(3, "email@kek.ru");
    Stmt->execute();


    std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(
        "Select id from user where login = ?"));
    stmt->setString(1, "testlogin1321");
    std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());
    if (result->next())
      userId = result->getInt(1);
//
//

    std::unique_ptr<sql::PreparedStatement> Stmt1(con->prepareStatement(
        "Insert into event(name, type, description, date, admin) Values (?, ?, ?, ?, ?);"));
    Stmt1->setString(1, "event_name");
    Stmt1->setString(2, md5("mytype"));
    Stmt1->setString(3, "qwertyuwqeqwe");
    Stmt1->setString(4, "2050-10-10");
    Stmt1->setInt(5, userId);

    Stmt1->execute();
    std::unique_ptr<sql::PreparedStatement> stmt1(con->prepareStatement(
        "Select id from event where name = ?"));
    stmt->setString(1, "event_name");
    std::unique_ptr<sql::ResultSet> result1(stmt->executeQuery());
    if (result1->next())
      eventId = result1->getInt(1);

  }

  void TearDown() override {
    std::unique_ptr<sql::PreparedStatement> Stmt(
        con->prepareStatement("Delete From user Where login = \"testlogin1321\""));
    Stmt->executeQuery();
  }

  sql::Driver *driver{};
  std::shared_ptr<sql::Connection> con;
  http::request<http::string_body> req;
  int userId, eventId;
  std::string name;
};

TEST_F(ViewUserFollowTest, goodPost) {
  nlohmann::json body;
  body["idol_id"] = eventId;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserFollow view(req, con, userId);
  view.post();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserFollow view2(req, con, userId);
  std::string respBody = view2.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 200);
}

TEST_F(ViewUserFollowTest, badPost) {
  nlohmann::json body;
  body["idol_i"] = eventId;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserFollow view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid data");
}

TEST_F(ViewUserFollowTest, badPost2) {
  nlohmann::json body;
  body["idol_id"] = "dasd";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserFollow view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid event_id");
}


TEST_F(ViewUserFollowTest, badPostJSON) {
  nlohmann::json body;
  body["idol_i"] = eventId;
  std::string str = R"({"sd" :: "df"})";
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserFollow view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "JSON error");
}

TEST_F(ViewUserFollowTest, badDeleteJSON) {
  nlohmann::json body;
  body["idol_i"] = eventId;
  std::string str = R"({"sd" :: "df"})";
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserFollow view(req, con, userId);
  std::string respBody = view.delete_().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "JSON error");
}

TEST_F(ViewUserFollowTest, goodDelte) {
  nlohmann::json body;
  body["idol_id"] = eventId;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserFollow view(req, con, userId);
  std::string respBody = view.delete_().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 200);
}

TEST_F(ViewUserFollowTest, badDelete) {
  nlohmann::json body;
  body["idol_i"] = eventId;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserFollow view(req, con, userId);
  std::string respBody = view.delete_().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid data");
}

TEST_F(ViewUserFollowTest, badDelete2) {
  nlohmann::json body;
  body["idol_id"] = "dasd";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserFollow view(req, con, userId);
  std::string respBody = view.delete_().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid event_id");
}