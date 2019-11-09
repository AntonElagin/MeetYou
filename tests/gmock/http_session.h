#ifndef TECHPROJECT_HTTP_SESSION_H
#define TECHPROJECT_HTTP_SESSION_H

#include <gmock/gmock.h>
#include "http_session.h"


class mock_http_session {
public:
  MOCK_METHOD(void, do_read, (), ());
  MOCK_METHOD(void, do_close, (), ());
};

#endif //TECHPROJECT_HTTP_SESSION_H
