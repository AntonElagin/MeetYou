#include <regex>
#include <boost/regex.hpp>
#include <string>
#include <iostream>

using namespace std;

int main() {
    // std::regex regular("(/chat/add/\\?)""(\\w+)""(=)""(\\d+)""(&)""(\\w+)""(=)""(\\d+)");
    std::string str = "/chat/members_count?chat_id=1";
    std::cmatch result;
    vector<string> params_list;
    std::regex regexik("/chat/(history|members_list|members_count)\\?chatid=(\\d+)");
//    boost::smatch xresults;
//    boost::regex_match(str, xresults, regexik);
    std::regex_match(str.c_str(), result, regexik);
//    std::regex regular("(/chat/create/\\?)""(title)""(=)""(\\w+)");
    for (auto &x:result)
        params_list.push_back(x);
    return 0;
}