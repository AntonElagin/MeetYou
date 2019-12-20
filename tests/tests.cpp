#include <gtest/gtest.h>
#include <cppconn/driver.h>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <string>
#include "CommonChatView.h"
#include "MessageChatView.h"

using ::testing::Return;
using ::testing::_;
using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

class QuickTest : public testing::Test {
protected:
    void SetUp() override {
        req.version(11);
        req.set(http::field::user_agent, "test");
        req.set(http::field::host, "localhost");
        req.prepare_payload();
        driver = get_driver_instance();
        std::shared_ptr<sql::Connection> conn(driver->connect("tcp://127.0.0.1:3306", "root", "167839"));
        con = conn;
        con->setSchema("meetyou");

    }


    void TearDown() override {
    }

    sql::Driver *driver;
    std::shared_ptr<sql::Connection> con;
    std::shared_ptr<sql::ResultSet> res;
    http::request<http::string_body> req;
};

TEST_F(QuickTest, requests) {
    int id = 2;
    req.body() = "{\n"
                 "  \"members_list\": [\n"
                 "    1,\n"
                 "    2,\n"
                 "    3,\n"
                 "    5,\n"
                 "    20\n"
                 "  ],\n"
                 "  \"admin_list\": [\n"
                 "    4,\n"
                 "    7\n"
                 "  ],\n"
                 "  \"title\": \"somechat\"\n"
                 "}";
    ViewChatCommon view_chat(req, con, id);
    auto kek = view_chat.post();
    boost::string_view heh = kek.body();///может быть чат с полностью одинаковым списком юзеров(будут разные id)
    ASSERT_STREQ(heh.to_string().c_str(), "{\"message\":\"chat created\",\"status\":\"OK\"}");
}

TEST_F(QuickTest, history) {
    int userid = 6;
    req.method(http::verb::get);
    req.target("/chat/history?chatid=1");
    ViewChatCommon view_chat(req, con, userid);
    auto kek = view_chat.get();
    boost::string_view heh = kek.body();
    ASSERT_STREQ(heh.to_string().c_str(), "[\"message\",\"access denied\",[\"status\",\"OK\"]]");
}

TEST_F(QuickTest, members_count) {
    int userid = 2;
    req.method(http::verb::get);
    req.target("/chat/members_count?chatid=1");
    ViewChatCommon view_chat(req, con, userid);
    auto kek = view_chat.get();
    boost::beast::string_view heh = kek.body();
    ASSERT_STREQ(heh.to_string().c_str(), "{\"members_count\":3,\"status\":\"OK\"}");
}

TEST_F(QuickTest, members_list) {
    int id = 2;
    req.target("/chat/members_list?chatid=1");
    ViewChatCommon view_chat(req, con, id);
    auto kek = view_chat.get();
    boost::string_view heh = kek.body();
    ASSERT_STREQ(heh.to_string().c_str(),
                 "[[2,\"Cchaddie Disdel\"],[3,\"Maxine Fanthome\"],[4,\"Elijah Attwell\"],[\"status\",\"OK\"]]");
}

TEST_F(QuickTest, chat_view) {
    int id = 2;
    req.body() = "{\n"
                 "  \"chatid\": 1,\n"
                 "  \"search_data\": \"members_list\"\n"
                 "}";
    Message_chat_view view_mesage(req, con, id);
//    auto kek = view_chat.get();
//    boost::string_view heh = kek.body();
//    ASSERT_STREQ(heh.to_string().c_str(),
//                 "{\"content\":[[2,\"Cchaddie Disdel\"],[3,\"Maxine Fanthome\"],[4,\"Elijah Attwell\"]],\"message\":\"OK\",\"status\":200}");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    //::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}