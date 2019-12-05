#ifndef TECHPROJECT_AUTH_MIDDLEWARE_INTERFACE_H
#define TECHPROJECT_AUTH_MIDDLEWARE_INTERFACE_H

class AuthMiddlewareInterface {
 public:
  virtual bool isAuth() = 0;
};

#endif  // TECHPROJECT_AUTH_MIDDLEWARE_INTERFACE_H
