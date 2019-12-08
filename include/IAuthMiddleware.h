#ifndef TECHPROJECT_IAUTHMIDDLEWARE_H
#define TECHPROJECT_IAUTHMIDDLEWARE_H

class IAuthMiddleware {
 public:
  virtual bool isAuth() = 0;
};

#endif  // TECHPROJECT_IAUTHMIDDLEWARE_H
