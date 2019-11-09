#ifndef PLUS_PROJECT_REQUEST_CHAT_HANDLER_H
#define PLUS_PROJECT_REQUEST_CHAT_HANDLER_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <string>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>
class request_chat_handler {
public:
    explicit request_chat_handler(std::shared_ptr<http::request<http::string_body>> req) : req(*req) {}

    int add_chat();///return id added chat
    int delete_chat();///return id deleted chat
    bool validate_request();

    http::request<http::string_body> req;
};


#endif //PLUS_PROJECT_REQUEST_CHAT_HANDLER_H
