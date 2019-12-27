#include <set>
#include "ViewRecommend.h"

http::response<http::string_body> ViewRecommend::get() {
  std::map<int, std::map<std::string, int>> matrix;
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

  auto sortFunc = [] (const std::pair<int,int> &a, const std::pair<int,int> &b)
  {
    return (a.second>b.second);
  };

  auto &currentUser = matrix[userId];
  std::vector<std::pair<int,int> > vector;
  for (auto &rec : matrix) {
    double cos = con(rec.second, currentUser) / sqrt(con(currentUser, currentUser) * con(rec.second, rec.second));
    double_t op1 = con(rec.second, currentUser);
    double_t op2 = con(rec.second, rec.second);
    double_t op3 = op1 / op2;
    if (userId != rec.first)
      vector.emplace_back(rec.first, cos * 100);
  }
  std::sort(vector.begin(),vector.end(),sortFunc);
  nlohmann::json body;
  for (int i = 0; i < 5; ++i) {
    if (vector[i].second > 70) {
      std::unique_ptr<sql::PreparedStatement> respStmt(
          conn->prepareStatement("Select login, name, surname, sex from user where id = ?;"));
      respStmt->setInt(1, vector[i].second);
      std::unique_ptr<sql::ResultSet> respRes(respStmt->executeQuery());
      std::cout << "1213312321321312" << std::endl;
      if (respRes->next()) {
        body["users"] += {{"login", respRes->getString(1)},
                          {"name", respRes->getString(2)},
                          {"surname", respRes->getString(3)},
                          {"sex", respRes->getString(4)},
                          {"%", vector[i].second}};
      }
    }
  }

  if (!body.contains("users"))
    body["users"] = std::vector<int> ();

  http::response<http::string_body> res;
  res.result(200);
  res.set(http::field::content_type, "json/application");
  res.keep_alive(req.keep_alive());
  std::string s = body.dump();
  res.body() = s;
  res.set(http::field::content_length, s.length());
  return res;
}

http::response<http::string_body> ViewRecommend::post() {
 return defaultPlug();
}

http::response<http::string_body> ViewRecommend::delete_() {
  return defaultPlug();
}

http::response<http::string_body> ViewRecommend::put() {
  return defaultPlug();
}

ViewRecommend::ViewRecommend(http::request<http::string_body> _req, std::shared_ptr<sql::Connection> _conn, int _userId)
    : View(_req, _conn, _userId) {

}
