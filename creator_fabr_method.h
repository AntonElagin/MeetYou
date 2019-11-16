#ifndef PLUS_PROJECT_CREATOR_FABR_METHOD_H
#define PLUS_PROJECT_CREATOR_FABR_METHOD_H

#include "handler_classes.h"
#include "basic_classes.h"

class CreatorHandler {
public:
    virtual IObjHandler *create_handler(const string, shared_ptr<IObject>);

};

#endif //PLUS_PROJECT_CREATOR_FABR_METHOD_H
