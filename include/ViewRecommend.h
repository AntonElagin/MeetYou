#ifndef TECHPROJECT_VIEWRECOMMEND_H
#define TECHPROJECT_VIEWRECOMMEND_H

#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>
#include <utility>
#include "ResponseCreater.h"
#include "View.h"
//#include "json.hpp"

namespace http = boost::beast::http;
using json = nlohmann::json;

// template <class Body, class Allocator>
class ViewRecommend : public View {

public:
  ViewRecommend(http::request<http::string_body>
                _req,
                std::shared_ptr<sql::Connection> _conn,
                int _userId
  );

  http::response<http::string_body> get() override;

  http::response<http::string_body> post() override;

  http::response<http::string_body> delete_() override;

  http::response<http::string_body> put() override;

  virtual ~ViewRecommend() = default;
};

#endif //TECHPROJECT_VIEWRECOMMEND_H
