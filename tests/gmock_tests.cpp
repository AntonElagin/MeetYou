#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <boost/beast.hpp>
#include <string>
#include <cppconn/connection.h>
#include "CommonChatView.h"
#include "MessageChatView.h"
#include "UserChatView.h"

using ::testing::Return;
using ::testing::_; // Matcher for parameters
using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

class MockCommon : public View {
public:
    MOCK_METHOD0(get, http::response<http::string_body>());
};


TEST(OrderTest, Fill) {
    http::request<http::string_body> req;
    sql::Driver *driver;
    std::shared_ptr<sql::Connection> con;
    std::shared_ptr<sql::ResultSet> res;
    req.version(11);
    req.method(http::verb::post);
    req.set(http::field::user_agent, "test");
    req.set(http::field::host, "localhost");
    req.prepare_payload();
    driver = get_driver_instance();
    std::shared_ptr<sql::Connection> conn(driver->connect("tcp://127.0.0.1:3306", "root", "167839"));
    conn->setSchema("meetyou");
    MockCommon commonView(req, conn, 1);

}

int main(int argc, char **argv) {
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}