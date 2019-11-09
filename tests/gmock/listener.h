#ifndef TECHPROJECT_LISTENER_H
#define TECHPROJECT_LISTENER_H

#include <gmock/gmock.h>
#include "listener.h"

class mock_listener {
public:
  MOCK_METHOD(void, run, (),(const));
};


#endif //TECHPROJECT_LISTENER_H
