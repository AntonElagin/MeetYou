#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <regex>
#include <boost/regex.hpp>
#include <string>
#include <iostream>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

int main() {
    vector<std::string> params_list;
    std::cmatch result;
    string str = "/chat/del/user?userid=4&chatid=2";
    std::regex r_chat_create("(/chat/create\\?)""(title)""(=)""(\\w+)");
    std::regex r_chat_drop("(/chat/drop\\?)""(id)""(=)""(\\d+)");
    std::regex r_chat_history("(/chat/history\\?)""(id)""(=)""(\\d+)");
    std::regex r_add_user("/chat/(add|del|upd)/user\\?(userid)=(\\d+)&(chatid)=(\\d+)");
    std::regex_match(str.c_str(), result, r_add_user);
    map<string, string> container;
    container["kek"] = "mda";
    container["ahah"] = "lox";
    container["ahafsfh"] = "lsfsdfx";
    json jsik(container);
    string temp = "{\"pi\", 3.141},\n"
                  "  {\"happy\", true},\n"
                  "  {\"name\", \"Niels\"},\n"
                  "  {\"nothing\", nullptr},\n"
                  "  {\"answer\", {\n"
                  "    {\"everything\", 42}\n"
                  "  }},\n"
                  "  {\"list\", {1, 0, 2}},\n"
                  "  {\"object\", {\n"
                  "    {\"currency\", \"USD\"},\n"
                  "    {\"value\", 42.99}\n"
                  "  }}";
    json jsok{temp};
    cout << jsok["pi"];
    return 0;
}