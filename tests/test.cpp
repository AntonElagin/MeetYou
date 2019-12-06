#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "basic_classes.h"
#include "handler_classes.h"
#include "req_parser.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <string>
#include "common_chat.h"
#include <cppconn/connection.h>

using ::testing::Return;
using ::testing::_;
using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

class QuickTest : public testing::Test {
protected:
    void SetUp() override {
        req.version(11);
        req.method(http::verb::post);
        req.target("/chat/history");
        req.set(http::field::user_agent, "test");
        req.set(http::field::host, "localhost");
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
        req.prepare_payload();
        driver = get_driver_instance();
        shared_ptr<sql::Connection> conn(driver->connect("tcp://127.0.0.1:3306", "root", "167839"));
        con = conn;
        con->setSchema("meetyou");

    }


    void TearDown() override {
    }

    sql::Driver *driver;
    shared_ptr<sql::Connection> con;
    shared_ptr<sql::ResultSet> res;
    http::request<http::string_body> req;
};

TEST_F(QuickTest, requests) {
    int id = 2;
    ViewChatCommon view_chat(req, con, id);
    auto kek = view_chat.post();
    boost::string_view heh = kek.body();
    ASSERT_STREQ(heh.to_string().c_str(), "{\"message\":\"OK\",\"status\":200}");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    //::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}