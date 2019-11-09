#ifndef PLUS_PROJECT_USER_H
#define PLUS_PROJECT_USER_H

#include <string>
using std::string;

class User {
    int id;
    string some_user_data;
public:
    explicit User(int id, string data) : id(id), some_user_data(data) {}
};


#endif //PLUS_PROJECT_USER_H
