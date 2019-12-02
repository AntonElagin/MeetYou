#ifndef PLUS_PROJECT_ROUTER_H
#define PLUS_PROJECT_ROUTER_H

#include <cstdlib>
#include <string>
#include <memory>
#include <regex>
#include "validator.h"
#include "handler_classes.h"

using std::shared_ptr;
using std::string;

class Router {
    IValidator *validator;
public:
    virtual IObjHandler *parse(string request) = 0;

};

class Chat_router : public Router {

public:
    IObjHandler *parse(string request) override;
};

class Common_router : public Router {
    shared_ptr<Router> chat_route;

public:
    IObjHandler *parse(string request) override;
};


#endif //PLUS_PROJECT_ROUTER_H
