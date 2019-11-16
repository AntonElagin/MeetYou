#ifndef PLUS_PROJECT_VALIDATOR_H
#define PLUS_PROJECT_VALIDATOR_H

#include "vector"
#include <cstdlib>
#include <string>

using std::string;
using std::vector;

class IValidator {
public:
    virtual bool validate(vector<string>) = 0;
};

class AccessValidator : public IValidator {
public:
    bool validate(vector<string>) override;

};

class DataValidator : public IValidator {
public:
    bool validate(vector<string>) override;
};


#endif //PLUS_PROJECT_VALIDATOR_H
