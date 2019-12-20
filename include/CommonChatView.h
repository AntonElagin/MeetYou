#ifndef PLUS_PROJECT_COMMONCHATVIEW_H
#define PLUS_PROJECT_COMMONCHATVIEW_H

#include <boost/beast.hpp>
#include <regex>
#include <memory>
#include <boost/regex.hpp>
#include <boost/beast.hpp>
#include <utility>
#include "View.h"

namespace http = boost::beast::http;

using json=nlohmann::json;

class ViewChatCommon : public View {
public:
    ViewChatCommon(http::request<http::string_body> _req,
                   std::shared_ptr<sql::Connection> _conn, int userId) : View(_req, _conn, userId) {}

    http::response<http::string_body> post() override;


    http::response<http::string_body> get() override;


    json history(const int chatid, int &error);

    json members_count(const int chatid, int &error);

    json members_list(const int chatid, int &error);

    http::response<http::string_body> delete_() override;

    http::response<http::string_body> put() override {
        return boost::beast::http::response<http::string_body>();
    }
};

#endif //PLUS_PROJECT_COMMONCHATVIEW_H