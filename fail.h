//
// Created by anton on 08.11.2019.
//

#ifndef TECHPROJECT_FAIL_H
#define TECHPROJECT_FAIL_H

#include <boost/beast.hpp>
#include <iostream>

// Report a failure
void fail(boost::beast::error_code ec, char const* what);

#endif  // TECHPROJECT_FAIL_H
