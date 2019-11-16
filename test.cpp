#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "chat.h"
#include "accept_actions.h"
#include "basic_classes.h"
#include "creator_fabr_method.h"
#include "handler_classes.h"
#include "router.h"
#include "validator.h"
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

class MockRouter : public Router {
public:
    //MOCK_METHOD(int, parse,(string request), (override);
    MOCK_METHOD1(parse, int(string
            request));

};

class MockChat {
public:
    MOCK_METHOD0(gethistory, vector<IObject>*());
};


TEST(chat_validate_test, call_validate) {
    MockRouter router;
    EXPECT_CALL(router, parse("somestring"))
            .Times(testing::AtLeast(1));

}

TEST(chat_call, get_people) {
    MockChat chat;
    EXPECT_CALL(chat, gethistory()).Times(0);

}

TEST(chat_validate_test, validate) {
    DataValidator validator;
    vector<string> some_parsed_string = {"get", "chat", "566", "history"};
    EXPECT_TRUE(validator.validate(some_parsed_string));
}

TEST(chat_get_people, online_chat) {
    OnlineChat chat;
    shared_ptr<vector<int>> people = chat.getpeople();
    vector<int> some_people_id = {1, 5, 12, 30, 45};
    EXPECT_EQ(*people, some_people_id);
}

TEST(chat_history, get_history) {
    HistoryChat<Message> historychat(new OfflineChat);
    Message mes1, mes2;
    vector<Message> hist2 = {mes1, mes2};

    EXPECT_EQ(historychat.get_history(), hist2);
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}