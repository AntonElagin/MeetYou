#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "chat.h"
#include "message.h"
#include "request_chat_handler.h"
#include "user.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <string>

using ::testing::Return;
using ::testing::_;
using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;
class Mock_chat : public Chat {
    MOCK_CONST_METHOD0(save_chat, bool());
};
std::shared_ptr<http::request<http::string_body>> generate_request_post(string body) {
    auto req=std::make_shared<http::request<http::string_body>>(http::verb::post, "/chat/560",11);
    req->set(http::field::user_agent, "Beast");
    req->body() = body;
    req->prepare_payload();
    return req;
}
std::shared_ptr<Chat> generate_chat(int bigid=9999999){
    int creator_id=rand()%200; ///планируется взять id из бд
    boost::gregorian::date d(2002,boost::gregorian::Jan,10);
    auto chat = std::make_shared<Chat>(bigid,creator_id,100,"sometext",d);
    return chat;
}
std::shared_ptr<Chat> find_chat(int id){
    //поиск в бд
    //if (founded) generate_chat(1);
    boost::gregorian::date d(2002,boost::gregorian::Jan,10);
    auto chat = std::make_shared<Chat>(1,1,1,"",boost::gregorian::day_clock::local_day());
    return chat;
}
std::shared_ptr<User> find_user(int userid){
    //поиск юзера с заданным id
    auto user=std::make_shared<User>(rand()%100,"some_text");
    return user;
}

TEST(chat_validate_test, call_validate) {
//    Mock_chat chat;
//    EXPECT_CALL(chat, PenDown())
//            .Times(AtLeast(1));
}
TEST(chat_test, save_chat) {
    auto some_chat=generate_chat();
    EXPECT_TRUE(some_chat->save_chat());
}
TEST(chat_test, delete_chat) {
    std::shared_ptr<Chat> some_chat=find_chat(1);
    EXPECT_TRUE(some_chat->del_chat());
}
TEST(user_in_chat, user_not_in_chat) {
    auto some_user=find_user(1);
    bool in_chat=false;
    //поиск среди участников чата
    //если найден то in_chat=true;
    EXPECT_FALSE(in_chat);
    //поиск другого участника
    EXPECT_TRUE(in_chat);
}
TEST(request_test, validate) {
    string body_text="{'message':'hello'}";
    auto req=std::make_shared<request_chat_handler>(generate_request_post(body_text));
    auto req2=std::make_shared<request_chat_handler>(generate_request_post(body_text+"some_text"));
    EXPECT_FALSE(req2->validate_request());
    EXPECT_TRUE(req->validate_request());
}



int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
  //  ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}