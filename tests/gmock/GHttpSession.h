#ifndef TECHPROJECT_HTTP_SESSION_H
#define TECHPROJECT_HTTP_SESSION_H

#include <gmock/gmock.h>
#include "IHttpSession.h"

//class mockHttpSession : public IHttpSession {
// public:
//  MOCK_METHOD(void, onWrite,
//              (bool close, beast::error_code ec, std::size_t bytes_transferred),
//              (override));
//  MOCK_METHOD(beast::tcp_stream&, getStream, (), ());
//  MOCK_METHOD(void, run, (), (override));
//};

#endif  // TECHPROJECT_HTTP_SESSION_H
