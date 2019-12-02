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

    IObject(int id, string name) : id(id), name(name) {}
};

class Chat : public IObject {
    boost::gregorian::date create_date;
    int members_count;
    vector<int> people;
public:
    int getid() override;

    Chat(int id, string name, int member_count, boost::gregorian::date date) : IObject(id, name),
                                                                               members_count(member_count), create_date(
                    boost::gregorian::day_clock::local_day()) {}

    //virtual shared_ptr<vector<int>> getpeople() = 0;

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
