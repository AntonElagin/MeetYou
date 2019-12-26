#include "SharedState.h"
#include "WebsocketSession.h"


SharedState::SharedState() {}

void SharedState::join(WebsocketSession *session, int chatid) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_[chatid].insert(session);
}

void SharedState::leave(WebsocketSession *session, int chatid) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_[chatid].erase(session);
}

// Broadcast a message to all websocket client sessions
void SharedState::send(std::string message, int chatid) {
    auto const ss = boost::make_shared<std::string const>(std::move(message));
    std::vector<boost::weak_ptr<WebsocketSession>> v;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        v.reserve(sessions_[chatid].size());
        for (auto p : sessions_[chatid])
            v.emplace_back(p->weak_from_this());
    }
    for (auto const &wp : v)
        if (auto sp = wp.lock())
            sp->send(ss);
}