#include "../include/router.h"

IObjHandler *Chat_router::parse(std::string target) {
    vector<std::string> params_list;
    std::cmatch result;
    IObjHandler *return_handler = nullptr;
    std::regex r_add_chat("/chat/(add)\\?(title)=(\\w+)");
    std::regex r_del_chat("/chat/(del)\\?(chatid)=(\\d+)");
    std::regex r_chat_history("/chat/(history)\\?(chatid)=(\\d+)");
    std::regex r_chat_memb_count("/chat/(members_count)\\?(chatid)=(\\d+)");
    std::regex r_add_message("/chat/(add)/message\\?(body)=(\\w+)&(chatid)=(\\d+)");
    std::regex r_del_message("/chat/(del)/message\\?(id)=(\\d+)");
    std::regex r_upd_message("/chat/(upd)/message\\?(body)=(\\w+)&(chatid)=(\\d+)");
    std::regex r_add_user("/chat/(add)/user\\?(userid)=(\\d+)&(chatid)=(\\d+)");
    std::regex r_del_user("/chat/(del)/user\\?(userid)=(\\d+)&(chatid)=(\\d+)");
    std::regex r_upd_user("/chat/(upd)/user\\?(role)=(admin|user)&(chatid)=(\\d+)");
    if (std::regex_match(target.c_str(), result, r_add_chat)) {
        return_handler = new ChatHandler;
    } else if (std::regex_match(target.c_str(), result, r_del_chat))
        return_handler = new ChatHandler;
    else if (std::regex_match(target.c_str(), result, r_chat_history))
        return_handler = new ChatHandler;
    else if (std::regex_match(target.c_str(), result, r_chat_memb_count))
        return_handler = new ChatHandler;
    else if (std::regex_match(target.c_str(), result, r_add_message))
        return_handler = new MessageHandler;
    else if (std::regex_match(target.c_str(), result, r_del_message))
        return_handler = new MessageHandler;
    else if (std::regex_match(target.c_str(), result, r_upd_message))
        return_handler = new MessageHandler;
    else if (std::regex_match(target.c_str(), result, r_add_user))
        return_handler = new UserHandler;
    else if (std::regex_match(target.c_str(), result, r_del_user))
        return_handler = new UserHandler;
    else if (std::regex_match(target.c_str(), result, r_upd_user))
        return_handler = new UserHandler;
    for (auto &x:result)
        return_handler->params_list.push_back(x);
    return return_handler;
}


IObjHandler *Common_router::parse(std::string request) {

}