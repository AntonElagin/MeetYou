#include "ViewUserHobby.h"
#include <cppconn/driver.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <Md5.h>


class ViewUserHobbyTest : public testing::Test {
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

    std::unique_ptr<sql::PreparedStatement> hobby(con->prepareStatement(
        "Insert into userhobby(user_id, hobby) Values ( ?, ?);"));
    hobby->setInt(1, userId);
    hobby->setString(2, "myhobby");
    hobby->execute();

  }

  void TearDown() override {
    std::unique_ptr<sql::PreparedStatement> Stmt(
        con->prepareStatement("Delete From user Where login = \"testlogin1321\""));
    Stmt->executeQuery();
    std::unique_ptr<sql::PreparedStatement> Stmt1(
        con->prepareStatement("Delete From userhobby Where user_id = ?"));
    Stmt1->setInt(1, userId);
    Stmt1->executeQuery();
  }

  sql::Driver *driver{};
  std::shared_ptr<sql::Connection> con;
  http::request<http::string_body> req;
  int userId;//, eventId;
  std::string name;
};

TEST_F(ViewUserHobbyTest, goodGet) {
  nlohmann::json body;
  body["user_id"] = userId;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserHobby view(req, con, userId);
  std::string respBody =  view.get().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["hobbies"][0], "myhobby");
}

TEST_F(ViewUserHobbyTest, goodGet2) {
  nlohmann::json body;
  body["user_id"] = -1;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserHobby view(req, con, userId);
  std::string respBody =  view.get().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid params or params count");
}

TEST_F(ViewUserHobbyTest, badGet1) {
  nlohmann::json body;
  body["user_id"] = 1000000;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserHobby view(req, con, userId);
  std::string respBody =  view.get().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_TRUE(body["hobbies"].size() == 0);
}

TEST_F(ViewUserHobbyTest, badGetJSON) {
  nlohmann::json body;
  std::string str = R"({"we":"")";
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserHobby view(req, con, userId);
  std::string respBody =  view.get().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 400);
}

TEST_F(ViewUserHobbyTest, goodPost1) {
  nlohmann::json body;
  body["hobby"] = "myhobby";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserHobby view(req, con, userId);
  std::string respBody =  view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 200);
}

TEST_F(ViewUserHobbyTest, badPost2) {
  nlohmann::json body;
  body["hobby1"] = "myhobby";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserHobby view(req, con, userId);
  std::string respBody =  view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid params or params count");
}

TEST_F(ViewUserHobbyTest, goodPost4) {
  nlohmann::json body;
  body["hobby"] = "myhobby123";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserHobby view(req, con, userId);
  std::string respBody =  view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 200);
}

TEST_F(ViewUserHobbyTest, goodPost3) {
  nlohmann::json body;
  std::vector<std::string> vector(2);
  vector.emplace_back("myhobby");
  vector.emplace_back("myhobby123");
  body["hobby"] = vector;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserHobby view(req, con, userId);
  std::string respBody =  view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 200);
}

TEST_F(ViewUserHobbyTest, badPost) {
  nlohmann::json body;
  body["hobby1"] = "vector";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserHobby view(req, con, userId);
  std::string respBody =  view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid params or params count");
}

TEST_F(ViewUserHobbyTest, badPostJSON) {
  nlohmann::json body;
  std::string str = R"({"we":"")";
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserHobby view(req, con, userId);
  std::string respBody =  view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 400);
}

TEST_F(ViewUserHobbyTest, badDelete1) {
  nlohmann::json body;
  body["hobby1"] = "vector";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserHobby view(req, con, userId);
  std::string respBody =  view.delete_().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid params or params count");
}

TEST_F(ViewUserHobbyTest, badDelete2) {
  nlohmann::json body;
  body["hobby"] = "vector";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserHobby view(req, con, userId);
  std::string respBody =  view.delete_().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "OK");
}

TEST_F(ViewUserHobbyTest, goodDelete) {
  nlohmann::json body;
  body["hobby"] = "myhobby";
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserHobby view(req, con, userId);
  std::string respBody =  view.delete_().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "OK");
}

TEST_F(ViewUserHobbyTest, goodDelete2) {
  nlohmann::json body;
  std::vector<std::string> vector(2);
  vector.emplace_back("myhobby");
  vector.emplace_back("myhobby123");
  body["hobby"] = vector;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserHobby view(req, con, userId);
  std::string respBody =  view.delete_().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "OK");
}


TEST_F(ViewUserHobbyTest, badDeleteJSON) {
  nlohmann::json body;
  std::string str = R"({"ddfd" : "dsdf")";
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewUserHobby view(req, con, userId);
  std::string respBody =  view.delete_().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid JSON");
}