#ifndef PLUS_PROJECT_BASIC_OBJECTS_H
#define PLUS_PROJECT_BASIC_OBJECTS_H

#include <cstdlib>
#include <vector>
#include <string>
#include <memory>
#include "boost/date_time/gregorian/gregorian.hpp"

using std::shared_ptr;
using std::string;
using std::vector;

class IObject {
protected:
    int id;
    string name;
public:
    virtual int getid() = 0;

};

class Chat : public IObject {
    boost::gregorian::date create_date;
    vector<int> member;
public:
    int getid() override;

    explicit Chat();

    virtual shared_ptr<vector<int>> getpeople();

};

class Message : public IObject {
    boost::gregorian::date create_date;
public:
    int getid() override;

    explicit Message();
};

class User : public IObject {
    string email;
public:
    int getid() override;

    explicit User();
};

#endif //PLUS_PROJECT_BASIC_OBJECTS_H
