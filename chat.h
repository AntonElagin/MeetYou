#ifndef PLUS_PROJECT_CHAT_H
#define PLUS_PROJECT_CHAT_H

#include "boost/date_time/gregorian/gregorian.hpp"
#include <string>
#include "message.h"
#include "user.h"
using std::string;

class Chat {
    int chat_id;
    int creator_id;
    int q_member;
    string title;
    boost::gregorian::date create_date;
public:
    explicit Chat(int chat_id, int creator_id, int q_member, string title,
                  boost::gregorian::date date = boost::gregorian::day_clock::local_day()) :
            chat_id(chat_id), creator_id(creator_id), q_member(q_member), title(title), create_date(date) {}

    explicit Chat(int id);///when find chat from db
    bool save_chat() const;
    bool del_chat();
};

class Chat_message_handler : public Chat {
public:
    void add(Message &mes);

    void del(Message &mes);
};

class Chat_user_handler : public Chat {
public:
    void add(User &mes);

    void del(Message &mes);
};


#endif //PLUS_PROJECT_CHAT_H
