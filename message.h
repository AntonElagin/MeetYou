#ifndef PLUS_PROJECT_MESSAGE_H
#define PLUS_PROJECT_MESSAGE_H
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;
class Message {
    int chat_id;
    int creator_id;
    std::string text;
    boost::gregorian::date create_date;
public:
    explicit Message(int chat_id, int creator_id, int q_member, std::string text,
                  boost::gregorian::date date = boost::gregorian::day_clock::local_day()) :
            chat_id(chat_id), creator_id(creator_id),  text(text), create_date(date) {}

};


#endif //PLUS_PROJECT_MESSAGE_H
