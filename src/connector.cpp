#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <regex>
#include <boost/regex.hpp>
#include <string>
#include <iostream>


int main() {
    // std::regex regular("(/chat/add/\\?)""(\\w+)""(=)""(\\d+)""(&)""(\\w+)""(=)""(\\d+)");
    std::string str = "/chat/create/?kek=12&lox=12";
    std::cmatch result;
    boost::regex regexik("/chat/(add|del)/\\?\\w+=\\d+&\\w+=\\d+");
    boost::smatch xresults;
    std::cout << boost::regex_match(str, xresults, regexik) << std::endl;
    //std::regex_match(str.c_str(), result, regular);
    std::regex regular("(/chat/create/\\?)""(title)""(=)""(\\w+)");
    for (auto x:string_params)
        params_list.push_back(x);
    return 0;
}