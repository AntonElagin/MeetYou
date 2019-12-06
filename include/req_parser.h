#ifndef PLUS_PROJECT_REQ_PARSER_H
#define PLUS_PROJECT_REQ_PARSER_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <memory>
#include "json.hpp"
#include <iostream>
#include <regex>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using json=nlohmann::json;

class Request_parser {
    beast::string_view target;
    std::map<std::string, std::string> key_value_map;
public:
    Request_parser(const int id, const http::request<http::string_body> &req) : target(req.method_string()) {
        if (req.method() == http::verb::post) {
            std::string temp = req.target().to_string();
            std::string body = req.body();
            json j(body);
            std::cmatch result;
            std::regex r_user("/chat/(add|del|upd)/user\\?(userid)=(\\d+)&(chatid)=(\\d+)");
            if (std::regex_match(temp.c_str(), result, r_user))
                key_value_map["action"] = j;
            std::cout << key_value_map["action"];
        }
    }

    std::map<std::string, std::string> get_map() {
        return key_value_map;
    }


};


#endif //PLUS_PROJECT_REQ_PARSER_H
