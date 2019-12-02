#ifndef PLUS_PROJECT_HANDLER_CLASSES_H
#define PLUS_PROJECT_HANDLER_CLASSES_H

#include "basic_classes.h"
#include "accept_actions.h"
#include "connector.h"

class IObjHandler {
protected:
    IObject *object;
    IAccept *acceptor;
    Connector *connector = nullptr;
public:
    virtual bool add() = 0;

    virtual bool update() = 0;

    virtual bool choicer();

    virtual bool del() = 0;

    bool set_bd_connector(Connector *conn) { connector = conn; }

    vector<std::string> params_list;
};

class ChatHandler : public IObjHandler {
public:
    bool add() override;

    bool update() override;

    bool del() override;

    bool chat_history();

    bool members_count();

    bool choicer() override;
};

class MessageHandler : public IObjHandler {
public:
    bool add() override;

    bool update() override;

    bool del() override;

};

class UserHandler : public IObjHandler {
public:
    bool add() override;

    bool update() override;

    bool del() override;
};

#endif //PLUS_PROJECT_HANDLER_CLASSES_H
