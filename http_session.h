//
// Created by anton on 08.11.2019.
//

#ifndef TECHPROJECT_HTTP_SESSION_H
#define TECHPROJECT_HTTP_SESSION_H

#include <boost/beast.hpp>
#include <boost/asio.hpp>
//#include "fail.h"
#include "websocket_session.h"

namespace beast = boost::beast;                 // from <boost/beast.hpp>
namespace http = boost::beast::http;                   // from <boost/beast/http.hpp>
namespace websocket = boost::beast::websocket;         // from <boost/beast/websocket.hpp>
namespace net = boost::asio;                    // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;




// Handles an HTTP server connection
class http_session : public std::enable_shared_from_this<http_session>
{
    // This queue is used for HTTP pipelining.
    class queue
    {
        enum
        {
            // Maximum number of responses we will queue
                    limit = 8
        };

        // The type-erased, saved work item
        struct work
        {
            virtual ~work() = default;
            virtual void operator()() = 0;
        };

        http_session& self_;
        std::vector<std::unique_ptr<work>> items_;

    public:
        explicit
        queue(http_session& self);

        // Returns `true` if we have reached the queue limit
        bool
        is_full() const
        {
            return items_.size() >= limit;
        }

        // Called when a message finishes sending
        // Returns `true` if the caller should initiate a read
        bool on_write();

        // Called by the HTTP handler to send a response.
        template<bool isRequest, class Body, class Fields>
        void operator()(http::message<isRequest, Body, Fields>&& msg);
    };

    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    queue queue_;

    // The parser is stored in an optional container so we can
    // construct it from scratch it at the beginning of each new message.
    boost::optional<http::request_parser<http::string_body>> parser_;

public:
    // Take ownership of the socket
    http_session(tcp::socket&& socket);


    // Start the session
    void run();

private:
    void
    do_read();

    void
    on_read(beast::error_code ec, std::size_t bytes_transferred);

    void
    on_write(bool close, beast::error_code ec, std::size_t bytes_transferred);

    void do_close();



//------------------------------------------------------------------------------
    template<class Body, class Allocator, class Send>
    void handle_request( http::request<Body, http::basic_fields<Allocator>>&& req,
                        Send&& send);
};



#endif //TECHPROJECT_HTTP_SESSION_H
