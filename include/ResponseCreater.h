#ifndef PLUS_PROJECT_RESPONSECREATER_H
#define PLUS_PROJECT_RESPONSECREATER_H

#include <boost/beast.hpp>
#include "json.hpp"

using json=nlohmann::json;
namespace http = boost::beast::http;

class ResponseCreator {
    http::response<http::string_body> resp;
public:
    ResponseCreator(json &base_body, int err_code) {
        json body(base_body);
        if (err_code == 1) {
            body.push_back({"status", "Error"});
            resp.result(500);
        } else {
            body.push_back({"status", "OK"});
            resp.result(200);
        }
        resp.set(http::field::version, 11);
        resp.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        resp.set(http::field::content_type, "application/json");
        resp.body() = body.dump();
        resp.content_length(body.size());
        resp.prepare_payload();
    }

    http::response<http::string_body> get_resp() { return resp; }
};

#endif //PLUS_PROJECT_RESPONSECREATER_H
