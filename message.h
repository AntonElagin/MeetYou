#ifndef PLUS_PROJECT_MESSAGE_H
#define PLUS_PROJECT_MESSAGE_H


class Message {
    int chat_id;
    int creator_id;
    std::string text;
    boost::gregorian::date create_date;
public:
    explicit Message(int chat_id, int creator_id, int q_member, std::string text,
                  boost::gregorian::date date = boost::gregorian::day_clock::local_day()) :
            chat_id(chat_id), creator_id(creator_id),  text(text), create_date(date) {}

};


#endif //PLUS_PROJECT_MESSAGE_H
