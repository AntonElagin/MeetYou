
#ifndef BOOST_BEAST_EXAMPLE_WEBSOCKET_CHAT_MULTI_SHARED_STATE_HPP
#define BOOST_BEAST_EXAMPLE_WEBSOCKET_CHAT_MULTI_SHARED_STATE_HPP

#include <boost/smart_ptr.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <map>

// Forward declaration
class WebsocketSession;

// Represents the shared server state
class SharedState {
    // This mutex synchronizes all access to sessions_
    std::mutex mutex_;
    // Keep a list of all the connected clients
    std::map<int, std::unordered_set<WebsocketSession *>> sessions_;

public:
    explicit
    SharedState();

    void join(WebsocketSession *session, int chatid);

    void leave(WebsocketSession *session, int chatid);

    void send(std::string message, int chatid);
};

#endif