#include "../include/router.h"

IObjHandler *Chat_router::parse(std::string target) {
    vector<std::string> params_list;
    std::cmatch result;
    IObjHandler *return_handler = nullptr;
    std::regex r_chat_create("(/chat/create\\?)""(title)""(=)""(\\w+)");
    std::regex r_chat_drop("(/chat/drop\\?)""(id)""(=)""(\\d+)");
    std::regex r_chat_history("/chat/history\\?""(id)""=""(\\d+)");
    std::regex r_add_message("/chat/(add)/message\\?(body)=(\\w+)&(chatid)=(\\d+)");
    if (std::regex_match(target.c_str(), result, r_chat_create)) {
        return_handler = new ChatHandler;
    } else if (std::regex_match(target.c_str(), result, r_chat_drop))
        return_handler = new ChatHandler;
    else if (std::regex_match(target.c_str(), result, r_chat_history))
        return_handler = new ChatHandler;
    else if (target == "/chat/members_count")
        return_handler = new ChatHandler;
    else if (std::regex_match(target.c_str(), result, r_add_message))
        return_handler = new MessageHandler;
    else if (target == "/chat/del/message")
        return_handler = new MessageHandler;
    else if (target == "/chat/update/message")
        return_handler = new MessageHandler;
    else if (target == "/chat/add/user")
        return_handler = new UserHandler;
    else if (target == "/chat/del/user")
        return_handler = new UserHandler;
    else if (target == "/chat/update/user")
        return_handler = new UserHandler;
    for (auto &x:result)
        return_handler->params_list.push_back(x);
    return return_handler;
}


IObjHandler *Common_router::parse(std::string request) {

}