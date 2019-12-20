#include <Md5.h>
#include <cppconn/driver.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include "ViewEventHobby.h"

class ViewEventHobbyTest : public testing::Test {
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
    std::unique_ptr<sql::PreparedStatement> del2(
        con->prepareStatement("Delete From event Where name = \"event_name\""));
    del->executeQuery();
    del2->executeQuery();

    std::unique_ptr<sql::PreparedStatement> Stmt(con->prepareStatement(
        "Insert into user(login, password, email) Values (?, ?, ?);"));
    Stmt->setString(1, "testlogin1321");
    Stmt->setString(2, md5("password123"));
    Stmt->setString(3, "email@kek.ru");
    Stmt->execute();

    std::unique_ptr<sql::PreparedStatement> stmt(
        con->prepareStatement("Select id from user where login = ?"));
    stmt->setString(1, "testlogin1321");
    std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());
    if (result->next()) userId = result->getInt(1);

    //
    //

    std::unique_ptr<sql::PreparedStatement> Stmt1(
        con->prepareStatement("Insert into event(name, type, description, "
                              "date, admin) Values (?, ?, ?, ?, ?);"));
    Stmt1->setString(1, "event_name");
    Stmt1->setString(2, "mytype");
    Stmt1->setString(3, "qwertyuwqeqwe");
    Stmt1->setString(4, "2050-10-10");
    Stmt1->setInt(5, userId);

    Stmt1->execute();
    std::unique_ptr<sql::PreparedStatement> stmt1(
        con->prepareStatement("Select id from event where name = ?"));
    stmt1->setString(1, "event_name");
    std::unique_ptr<sql::ResultSet> result1(stmt1->executeQuery());
    if (result1->next()) eventId = result1->getInt(1);

    std::unique_ptr<sql::PreparedStatement> hobby(con->prepareStatement(
        "Insert into eventhobby(event_id, hobby) Values ( ?, ?);"));
    hobby->setInt(1, eventId);
    hobby->setString(2, "myhobby");
    hobby->execute();
  }

  void TearDown() override {
    std::unique_ptr<sql::PreparedStatement> Stmt(con->prepareStatement(
        "Delete From user Where login = \"testlogin1321\""));
    Stmt->executeQuery();
    std::unique_ptr<sql::PreparedStatement> Stmt1(
        con->prepareStatement("Delete From eventhobby Where event_id = ?"));
    Stmt1->setInt(1, userId);
    Stmt1->executeQuery();
  }

  sql::Driver *driver{};
  std::shared_ptr<sql::Connection> con;
  http::request<http::string_body> req;
  int userId, eventId;
  std::string name;
};

TEST_F(ViewEventHobbyTest, goodGet) {
  nlohmann::json body;
  body["event_id"] = eventId;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewEventHobby view(req, con, userId);
  std::string respBody = view.get().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["hobbies"][0], "myhobby");
}

TEST_F(ViewEventHobbyTest, goodGet2) {
  nlohmann::json body;
  body["event_id"] = -1;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewEventHobby view(req, con, userId);
  std::string respBody = view.get().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid params or params count");
}

TEST_F(ViewEventHobbyTest, badGet1) {
  nlohmann::json body;
  //  body["user_id"] = 1000000;
  body["event_id"] = 10000000;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewEventHobby view(req, con, userId);
  std::string respBody = view.get().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_TRUE(body["hobbies"].size() == 0);
}

TEST_F(ViewEventHobbyTest, badGetJSON) {
  nlohmann::json body;
  std::string str = R"({"we":"")";
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewEventHobby view(req, con, userId);
  std::string respBody = view.get().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 400);
}

TEST_F(ViewEventHobbyTest, goodPost1) {
  nlohmann::json body;
  body["hobby"] = "myhobby";
  body["event_id"] = eventId;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewEventHobby view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 200);
}

TEST_F(ViewEventHobbyTest, badPost2) {
  nlohmann::json body;
  body["hobby1"] = "myhobby";
  body["event_id"] = eventId;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewEventHobby view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid params or params count");
}

TEST_F(ViewEventHobbyTest, goodPost4) {
  nlohmann::json body;
  body["hobby"] = "myhobby123";
  body["event_id"] = eventId;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewEventHobby view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 200);
}

TEST_F(ViewEventHobbyTest, goodPost3) {
  nlohmann::json body;
  std::vector<std::string> vector(2);
  vector.emplace_back("myhobby");
  vector.emplace_back("myhobby123");
  body["hobby"] = vector;
  body["event_id"] = eventId;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewEventHobby view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 200);
}

TEST_F(ViewEventHobbyTest, badPost) {
  nlohmann::json body;
  body["hobby1"] = "vector";
  body["event_id"] = eventId;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewEventHobby view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid params or params count");
}

TEST_F(ViewEventHobbyTest, badPostJSON) {
  nlohmann::json body;
  std::string str = R"({"we":"")";
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewEventHobby view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["status"], 400);
}

TEST_F(ViewEventHobbyTest, badDelete1) {
  nlohmann::json body;
  body["hobby1"] = "vector";
  body["event_id"] = eventId;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewEventHobby view(req, con, userId);
  std::string respBody = view.delete_().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid params or params count");
}

TEST_F(ViewEventHobbyTest, badDelete2) {
  nlohmann::json body;
  body["hobby"] = "vector";
  body["event_id"] = eventId;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewEventHobby view(req, con, userId);
  std::string respBody = view.delete_().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "OK");
}

TEST_F(ViewEventHobbyTest, goodDelete) {
  nlohmann::json body;
  body["hobby"] = "myhobby";
  body["event_id"] = eventId;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewEventHobby view(req, con, userId);
  std::string respBody = view.delete_().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "OK");
}

TEST_F(ViewEventHobbyTest, goodDelete2) {
  nlohmann::json body;
  std::vector<std::string> vector(2);
  vector.emplace_back("myhobby");
  vector.emplace_back("myhobby123");
  body["hobby"] = vector;
  body["event_id"] = eventId;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewEventHobby view(req, con, userId);
  std::string respBody = view.delete_().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "OK");
}

TEST_F(ViewEventHobbyTest, badDeleteJSON) {
  nlohmann::json body;
  std::string str = R"({"ddfd" : "dsdf")";
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewEventHobby view(req, con, userId);
  std::string respBody = view.delete_().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Invalid JSON");
}

TEST_F(ViewEventHobbyTest, accesDeniedDelete) {
  nlohmann::json body;
  body["hobby"] = "myhobby";
  body["event_id"] = 0;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewEventHobby view(req, con, userId);
  std::string respBody = view.delete_().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Access denied");
}

TEST_F(ViewEventHobbyTest, accesDeniedpost) {
  nlohmann::json body;
  body["hobby"] = "myhobby";
  body["event_id"] = 0;
  std::string str = body.dump();
  req.body() = str;
  req.set(http::field::content_length, str.length());
  ViewEventHobby view(req, con, userId);
  std::string respBody = view.post().body();
  body = nlohmann::json::parse(respBody);
  ASSERT_EQ(body["message"], "Access denied");
}