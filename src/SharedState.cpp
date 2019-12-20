#include "SharedState.h"
#include "WebsocketSession.h"


SharedState::
SharedState(){
}

void
SharedState::
join(WebsocketSession *session, int chatid) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_[chatid].insert(session);
}

void
SharedState::
leave(WebsocketSession *session, int chatid) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_[chatid].erase(session);
}

// Broadcast a message to all websocket client sessions
void SharedState::send(std::string message, int chatid) {
    // Put the message in a shared pointer so we can re-use it for each client
    auto const ss = boost::make_shared<std::string const>(std::move(message));
    // Make a local list of all the weak pointers representing
    // the sessions, so we can do the actual sending without
    // holding the mutex:
    std::vector<boost::weak_ptr<WebsocketSession>> v;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        v.reserve(sessions_[chatid].size());
        for (auto p : sessions_[chatid])
            v.emplace_back(p->weak_from_this());
    }
    // For each session in our local list, try to acquire a strong
    // pointer. If successful, then send the message on that session.
    for (auto const &wp : v)
        if (auto sp = wp.lock())
            sp->send(ss);
}