#include <iostream>
#include <memory>
#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <vector>
#include <set>
#include <string>
#include <random>
#include "Md5.h"
#include <math.h>


std::vector<std::string> hobbyy = {"sky", "skate", "tea", "coffee", "socket", "c++", "hokkie", "somithing", "sleep",
                                   "baseball", "basketball", "tennis", "cinema", "aaaaaa", "sleeep", "dream", "history",
                                   "cars", "planes", "dance", "russian books", "travlle"};


bool generateUser() {
  std::string login = "genLogin",
      email = "genEmail",
      emailEnd = "@mail.ru";
  sql::Driver *driver = get_driver_instance();
  std::unique_ptr<sql::Connection> conn(driver->connect("tcp://127.0.0.1:3306", "root", "12A02El99"));
  conn->setSchema("MeetYou");
  std::unique_ptr<sql::PreparedStatement> userStmt(
      conn->prepareStatement("Insert into user(email,login,password) values (?, ?, ?)"));
  for (int i = 0; i < 100000; ++i) {
    userStmt->setString(1, email + std::to_string(i) + emailEnd);
    userStmt->setString(2, login + std::to_string(i));
    userStmt->setString(3, md5("mypassword"));
    userStmt->executeQuery();
  }
  std::unique_ptr<sql::ResultSet> result(userStmt->executeQuery());
}

bool generateHobby() {
  sql::Driver *driver = get_driver_instance();
  std::unique_ptr<sql::Connection> conn(driver->connect("tcp://127.0.0.1:3306", "root", "12A02El99"));
  conn->setSchema("MeetYou");
  std::unique_ptr<sql::PreparedStatement> userStmt(
      conn->prepareStatement("Insert into userhobby(user_id, hobby) values (?, ?)"));
  for (int i = 1; i < 10002; ++i) {
    std::cout << i << std::endl;
    userStmt->setInt(1, i);

//    random(time(0));
    for (int j = 0; j < (rand() % hobbyy.size() + 1); ++j) {
      userStmt->setString(2, hobbyy[rand() % hobbyy.size()]);
      userStmt->executeQuery();
    }
  }
  std::unique_ptr<sql::ResultSet> result(userStmt->executeQuery());
}

bool generateFollowers() {
  sql::Driver *driver = get_driver_instance();
  std::unique_ptr<sql::Connection> conn(driver->connect("tcp://127.0.0.1:3306", "root", "12A02El99"));
  conn->setSchema("MeetYou");
  std::unique_ptr<sql::PreparedStatement> followStmt(
      conn->prepareStatement("Insert into userfollowers(user_id, idol_id) values (?, ?)"));
  for (int i = 1; i < 10002; ++i) {
    std::cout << i << std::endl;
    followStmt->setInt(1, i);


    for (int j = 0; j < (rand() % 40 + 10); ++j) {
      int idol = rand() % 10000 + 1;
      if (idol != i) {
        followStmt->setInt(2, idol);
        followStmt->executeQuery();
      }
    }
  }
  std::unique_ptr<sql::ResultSet> result(followStmt->executeQuery());
}

int main() {
  srand(time(0));
//  generate();
//  generateHobby();
//  generateFollowers();
  sql::Driver *driver = get_driver_instance();
  std::unique_ptr<sql::Connection> conn(driver->connect("tcp://127.0.0.1:3306", "root", "12A02El99"));
  conn->setSchema("MeetYou");

  std::unique_ptr<sql::PreparedStatement> userStmt(
      conn->prepareStatement("Select id From user where need_recommend = ?;"));
  userStmt->setBoolean(1, true);
  std::unique_ptr<sql::ResultSet> result(userStmt->executeQuery());
  while (result->next()) {
    std::map<int, std::map<std::string, int>> matrix;
    int userId = result->getInt(1);
    std::set<int> userFolowList;
    userFolowList.insert(userId);
    matrix.insert(std::make_pair(userId, std::map<std::string, int>()));
    std::unique_ptr<sql::PreparedStatement> followStmt(
        conn->prepareStatement("Select idol_id from userfollowers where user_id = ?;"));
    followStmt->setInt(1, userId);
    std::unique_ptr<sql::ResultSet> followRes(followStmt->executeQuery());
//    std::vector<int> userFolowList;

    while (followRes->next()) {
      userFolowList.insert(followRes->getInt(1));
    }
    for (auto &a : userFolowList) {
      std::unique_ptr<sql::PreparedStatement> folFolStmt(
          conn->prepareStatement("Select idol_id from userfollowers where user_id = ?;"));
      folFolStmt->setInt(1, a);
      std::unique_ptr<sql::ResultSet> folFolRes(followStmt->executeQuery());

      while (folFolRes->next()) {
        int recomId = folFolRes->getInt(1);
        if (userFolowList.find(recomId) != userFolowList.end())
          matrix.insert(std::make_pair(recomId, std::map<std::string, int>()));
      }
    }

    std::set<std::string> noDuplicate;
    for (auto &user : matrix) {
      std::unique_ptr<sql::PreparedStatement> hobbyStmt(
          conn->prepareStatement("Select hobby from userhobby where user_id = ?;"));
      hobbyStmt->setInt(1, user.first);
      std::unique_ptr<sql::ResultSet> hobbyRes(hobbyStmt->executeQuery());
      while (hobbyRes->next()) {
        std::string key = hobbyRes->getString(1);
        noDuplicate.insert(key);
        user.second.insert(std::make_pair(key, 1));
      }

    }

    auto con = [](std::map<std::string, int> &a, std::map<std::string, int> &b) {
      double sum = 0;
      for (const auto &val1 : a)
        if (b.find(val1.first) != b.end())
          sum += b[val1.first] * val1.second;
      return sum;
    };

    auto &currentUser = matrix[userId];
    for (auto &rec : matrix) {
      double cos = con(rec.second, currentUser) / sqrt(con(currentUser, currentUser) * con(rec.second, rec.second));
      double_t op1 = con(rec.second, currentUser);
      double_t op2 = con(rec.second, rec.second);
      double_t op3 = op1 / op2;
      std::cout << cos << "     " <<  op2 << "     " << rec.first << std::endl;
    }
    std::cout << std::endl;
  }
//  userStmt->setInt(1, );

}
