#ifndef PLUS_PROJECT_CHAT_H
#define PLUS_PROJECT_CHAT_H

#include "boost/date_time/gregorian/gregorian.hpp"
#include <string>
#include "basic_classes.h"

using std::string;

class GroupChat : public Chat {
protected:
    int adminid;
public:
    shared_ptr<vector<int>> getpeople() override;

    explicit GroupChat();
};

class PersonalChat : public Chat {
public:
    shared_ptr<vector<int>> getpeople() override;

    explicit PersonalChat();
};

class OnlineChat : public GroupChat {
public:
    shared_ptr<vector<int>> getpeople() override;//get online member

    explicit OnlineChat();
};

class OfflineChat : public GroupChat {
public:
    shared_ptr<vector<int>> getpeople() override;//get all member in chat

    explicit OfflineChat();
};

template<class T>
class HistoryChat {
    shared_ptr<Chat> chat;
    vector<T> history_list;
public:
    HistoryChat(Chat *);

    vector<IObject> get_history();
};


#endif //PLUS_PROJECT_CHAT_H