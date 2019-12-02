#ifndef PLUS_PROJECT_ACCEPT_ACTIONS_H
#define PLUS_PROJECT_ACCEPT_ACTIONS_H

#include "basic_classes.h"

class IAccept {
public:
    virtual bool compare_obj(IObject *) = 0;
};

class AcceptPermission : public IAccept {
    bool compare_obj(IObject *) override;
};

class AcceptData : public IAccept {
    bool compare_obj(IObject *) override;
};


#endif //PLUS_PROJECT_ACCEPT_ACTIONS_H
