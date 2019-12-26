#include <iostream>
#include <memory>
#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <vector>
#include <set>
#include <string>
#include ""

std::vector<std::string> hobbyy = {"sky", "skate", "tea", "coffee", "socket", "c++", "hokkie", "somithing", "sleep",
                                   "baseball","basketball","tennis","cinema", "aaaaaa", "sleeep", "dream", "history",
                                   "cars", "planes", "dance", "russian books", "travlle"};



bool generate() {
  std::string login = "genLogin",
              email = "genEmail",
              emailEnd = "@mail.ru";
  sql::Driver *driver = get_driver_instance();
  std::unique_ptr<sql::Connection> conn(driver->connect("tcp://127.0.0.1:3306", "root", "12A02El99"));
  std::unique_ptr<sql::PreparedStatement> userStmt(
      conn->prepareStatement("Insert into user(email,login,password) values (?, ?, ?)"));
  for (int i = 0; i < 100000; ++i) {
    userStmt->setString(1, email + std::to_string(i) + emailEnd);
    userStmt->setString(2, login + std::to_string(i));
    userStmt->setString(3, md);

  }
  std::unique_ptr<sql::ResultSet> result(userStmt->executeQuery());
}


int main() {
  sql::Driver *driver = get_driver_instance();
  std::unique_ptr<sql::Connection> conn(driver->connect("tcp://127.0.0.1:3306", "root", "12A02El99"));
  std::unique_ptr<sql::PreparedStatement> userStmt(
      conn->prepareStatement("Select id From user where need_recommend = ?;"));
  userStmt->setBoolean(1, true);
  std::unique_ptr<sql::ResultSet> result(userStmt->executeQuery());
  while (result->next()) {
    std::map<int, std::map<std::string, int>> matrix;
    int userId = result->getInt(1);
    matrix.insert(std::make_pair(userId, std::map<std::string, int>()));
    std::unique_ptr<sql::PreparedStatement> followStmt(
        conn->prepareStatement("Select idol_id from userfollowers where user_id = ?;"));
    followStmt->setInt(1, userId);
    std::unique_ptr<sql::ResultSet> followRes(followStmt->executeQuery());
//    std::vector<int> userFolowList;
    std::set<int> userFolowList;
    while (followRes->next()) {
      userFolowList.insert(followRes->getInt(1));

      std::unique_ptr<sql::PreparedStatement> folFolStmt(
          conn->prepareStatement("Select idol_id from userfollowers where user_id = ?;"));
      folFolStmt->setInt(1, userId);
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
      std::unique_ptr<sql::ResultSet> hobbyRes(followStmt->executeQuery());
      while (hobbyRes->next()) {
        std::string key = hobbyRes->getString(1);
        noDuplicate.insert(key);
        user.second.insert(std::make_pair(key, 1));
      }

    }


  }
//  userStmt->setInt(1, );

}
