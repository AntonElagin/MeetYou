#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//------------------------------------------------------------------------------
// Report a failure
void fail(beast::error_code ec, char const *what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

// Sends a WebSocket message and prints the response
class session : public std::enable_shared_from_this<session> {
    tcp::resolver resolver_;
    websocket::stream<beast::tcp_stream> ws_;
    beast::flat_buffer buffer_;
    std::string host_;
    std::string token;
    std::string chatid_;
public:
    // Resolver and socket require an io_context
    explicit session(net::io_context &ioc) : resolver_(net::make_strand(ioc)), ws_(net::make_strand(ioc)) {}

    // Start the asynchronous operation
    void run(char const *host, char const *port, char const *token_, char const* chatid) {
        // Save these for later
        host_ = host;
        token = token_;
        chatid_=chatid;
        // Look up the domain name
        resolver_.async_resolve(host, port, beast::bind_front_handler(&session::on_resolve, shared_from_this()));
    }

    void on_resolve(beast::error_code ec, tcp::resolver::results_type results) {
        if (ec) return fail(ec, "resolve");
        // Set the timeout for the operation
        beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));
        // Make the connection on the IP address we get from a lookup
        beast::get_lowest_layer(ws_).async_connect(results,
                                                   beast::bind_front_handler(&session::on_connect, shared_from_this()));
    }

    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type) {
        if (ec)
            return fail(ec, "connect");
        // Turn off the timeout on the tcp_stream, because
        // the websocket stream has its own timeout system.
        beast::get_lowest_layer(ws_).expires_never();
        // Set suggested timeout settings for the websocket
        ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));
        // Set a decorator to change the User-Agent of the handshake
        std::string buffer = "access_token=" + token;
        ws_.set_option(websocket::stream_base::decorator([&buffer](websocket::request_type &req) {
            req.set(http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-coro");
            req.set(http::field::cookie, buffer);
        }));
        // Perform the websocket handshake
        ws_.async_handshake(host_, "/chat?id=" + chatid_,
                            beast::bind_front_handler(&session::on_handshake, shared_from_this()));
    }

    void on_handshake(beast::error_code ec) {
        if (ec)return fail(ec, "handshake");
        std::string textik;
        auto func = [&]() {
            while (true) {
                beast::flat_buffer buffer;
                ws_.read(buffer);
                if (ec) return fail(ec, "read");
                std::cout << beast::make_printable(buffer.data()) << std::endl;
            }
        };
        std::thread async_read(func);
        std::string str;
        while (true) {
            getline(std::cin, str);
            printf("\b\b");
            ws_.write(net::buffer(str), ec);
            if (ec) return fail(ec, "write");
        }
        ws_.async_write(net::buffer(token), beast::bind_front_handler(&session::on_write, shared_from_this()));
    }

    void on_write(beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);
        if (ec)
            return fail(ec, "write");
        // Read a message into our buffer
        ws_.async_read(buffer_, beast::bind_front_handler(&session::on_read, shared_from_this()));
    }

    void on_read(beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);
        if (ec)
            return fail(ec, "read");
        // Close the WebSocket connection
        ws_.async_close(websocket::close_code::normal,
                        beast::bind_front_handler(&session::on_close, shared_from_this()));
    }

    void on_close(beast::error_code ec) {
        if (ec)
            return fail(ec, "close");
        // If we get here then the connection is closed gracefully
        // The make_printable() function helps print a ConstBufferSequence
        std::cout << beast::make_printable(buffer_.data()) << std::endl;
    }
};

//------------------------------------------------------------------------------
int main(int argc, char **argv) {
    // Check command line arguments.
    if (argc != 5) {
        std::cerr <<
                  "Usage: websocket-client-async <host> <port> <login> <pass> <chatid_>\n" <<
                  "Example:\n" <<
                  "    127.0.0.1 8080 fsfkewr249 2";
        return EXIT_FAILURE;
    }
    auto const host = argv[1];
    auto const port = argv[2];
    auto const token = argv[3];
    auto const chatid = argv[4];
    // The io_context is required for all I/O
    net::io_context ioc;
    // Launch the asynchronous operation
    std::make_shared<session>(ioc)->run(host, port, token,chatid);
    // Run the I/O service. The call will return when
    // the socket is closed.
    ioc.run();
    return EXIT_SUCCESS;
}