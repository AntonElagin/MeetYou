#ifndef PLUS_PROJECT_HANDLER_CLASSES_H
#define PLUS_PROJECT_HANDLER_CLASSES_H

#include "basic_classes.h"
#include "accept_actions.h"
#include "connector.h"
#include <string>

class IObjHandler {
protected:
    IObject *object;
    IAccept *acceptor;
    Connector *connector = nullptr;
public:
    virtual std::string add() = 0;

    virtual std::string update() = 0;

    virtual std::string choicer();

    virtual std::string del() = 0;

    bool set_bd_connector(Connector *conn) { connector = conn; }

    vector<std::string> params_list;
};

class ChatHandler : public IObjHandler {
public:
    std::string add() override;

    std::string update() override;

    std::string del() override;

    std::string chat_history();

    std::string members_count();

    std::string choicer() override;
};

class MessageHandler : public IObjHandler {
public:
    std::string add() override;

    std::string update() override;

    std::string del() override;

};

class UserHandler : public IObjHandler {
public:
    std::string add() override;

    std::string update() override;

    std::string del() override;
};

#endif //PLUS_PROJECT_HANDLER_CLASSES_H
