#include "handler_classes.h"
#include "connector.h"

bool ChatHandler::add() {

}

bool ChatHandler::update() {
    return false;
}

bool ChatHandler::del() {
    return false;
}

bool ChatHandler::chat_history() {
    sql::PreparedStatement *pstmt;
    pstmt = connector->get_con()->prepareStatement("");
    return false;
}

bool ChatHandler::choicer() {
    if (params_list[1] == "history")
        chat_history();
    else if (params_list[1] == "del")
        del();
    else if (params_list[1] == "add")
        add();
    else if (params_list[1] == "members_count")
        members_count();
    return true;
}

bool ChatHandler::members_count() {
    return false;
}


bool MessageHandler::add() {
    sql::Connection *con = connector->get_con();
    sql::PreparedStatement *pstmt;
    string body = params_list[3];
    int chat_id = std::stoi(params_list.back());
    int author_id = std::stoi(params_list.back());///id from anton
    pstmt = con->prepareStatement(
            "insert into Message (id, publication_date, body, author_id, chat_id) values (null, CURDATE(), ?, ?, ?)");
    for (int i = 1; i <= 4; i++) {
        pstmt->setInt(2, i);//set author id
        pstmt->setInt(3, std::stoi(params_list.back()));///set chat id
        pstmt->setString(1, params_list[3]);///setbody
        pstmt->executeUpdate();
    }
    delete pstmt;
    return false;
}

bool MessageHandler::update() {
    sql::Connection *con = connector->get_con();
    sql::ResultSet *res;
    sql::PreparedStatement *pstmt;
    pstmt = con->prepareStatement(
            "delete from message where id=?");
    pstmt->setInt(1, std::stoi(params_list.back()));
    pstmt->executeUpdate();
    delete pstmt;
    return false;
    return false;
}

bool MessageHandler::del() {
    sql::Connection *con = connector->get_con();
    sql::ResultSet *res;
    sql::PreparedStatement *pstmt;
    pstmt = con->prepareStatement(
            "delete from Message where id=?");
    pstmt->setInt(1, std::stoi(params_list.back()));
    pstmt->executeUpdate();
    delete pstmt;
    return false;
}


bool UserHandler::add() {

    return false;
}

bool UserHandler::update() {
    return false;
}

bool UserHandler::del() {
    return false;
}

bool IObjHandler::choicer() {
    if (params_list[1] == "add")
        add();
    else if (params_list[1] == "del")
        del();
    else if (params_list[1] == "update")
        update();
}