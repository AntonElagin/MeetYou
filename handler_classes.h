#ifndef PLUS_PROJECT_HANDLER_CLASSES_H
#define PLUS_PROJECT_HANDLER_CLASSES_H

#include "basic_classes.h"
#include "accept_actions.h"

class IObjHandler {
protected:
    IObject *object;
    IAccept *acceptor;
public:
    virtual bool add() = 0;

    virtual bool del() = 0;

    bool set_obj(IObject *);
};

class ChatHandler : public IObjHandler {
    bool add() override;

    bool del() override;

};

class MessageHandler : public IObjHandler {
    bool add() override;

    bool del() override;
};

class UserHandler : public IObjHandler {
    bool add() override;

    bool del() override;
};

#endif //PLUS_PROJECT_HANDLER_CLASSES_H
