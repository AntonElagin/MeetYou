#ifndef PLUS_PROJECT_ROUTER_H
#define PLUS_PROJECT_ROUTER_H

#include <cstdlib>
#include <string>
#include <memory>
#include "validator.h"

using std::shared_ptr;
using std::string;

class Router {
    IValidator *validator;
public:
    virtual int parse(string request) = 0;
};

class Chat_route : public Router {
public:
    int parse(string request) override;
};

class Common_router : public Router {
    shared_ptr<Router> chat_route;

public:
    int parse(string request) override;
};


#endif //PLUS_PROJECT_ROUTER_H
