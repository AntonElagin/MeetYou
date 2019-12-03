#include "handler_classes.h"
#include "connector.h"

std::string ChatHandler::add() {
    sql::Connection *con = connector->get_con();
    sql::PreparedStatement *pstmt;
    pstmt = con->prepareStatement(
            "insert into Chat (id, members_count, create_date, admin_id, title) values (null, 0,CURDATE() , 6, 'some title')");
    pstmt->setInt(1, std::stoi(params_list.back()));
    pstmt->executeUpdate();
    delete pstmt;
    return "okey, chat with id=" + params_list.back() + " created";
}

std::string ChatHandler::update() {
    return "ok";
}

std::string ChatHandler::del() {
    sql::Connection *con = connector->get_con();
    sql::PreparedStatement *pstmt;
    pstmt = con->prepareStatement("delete from Message where chat_id=?");
    pstmt->setInt(1, std::stoi(params_list.back()));
    pstmt->executeUpdate();
    pstmt = con->prepareStatement("delete from Chat where id=?");
    pstmt->setInt(1, std::stoi(params_list.back()));
    pstmt->executeUpdate();
    pstmt = con->prepareStatement("delete from result_table where chat_id=?");
    pstmt->setInt(1, std::stoi(params_list.back()));
    pstmt->executeUpdate();
    delete pstmt;
    return "okey, chat with id=" + params_list.back() + " is deleted";
}

string ChatHandler::chat_history() {
    sql::PreparedStatement *pstmt;
    std::string result_str = "{";
    pstmt = connector->get_con()->prepareStatement(
            "select body text,username nick from User,Message where chat_id=? and author_id=User.id");
    pstmt->setInt(1, std::stoi(params_list.back()));
    sql::ResultSet *res = pstmt->executeQuery();
    while (res->next()) {
        result_str += res->getString("text") + " : ";
        result_str += res->getString("nick") + " ";
    }
    return result_str;
}

std::string ChatHandler::choicer() {
    if (params_list[1] == "history")
        return chat_history();
    else if (params_list[1] == "del")
        return del();
    else if (params_list[1] == "add")
        return add();
    else if (params_list[1] == "members_count")
        return members_count();
    return "some error with routing";
}

std::string ChatHandler::members_count() {
    sql::PreparedStatement *pstmt;
    std::string result_str = "{";
    pstmt = connector->get_con()->prepareStatement(
            "select count(user_id)from result_table where chat_id=?");
    pstmt->setInt(1, std::stoi(params_list.back()));
    sql::ResultSet *res = pstmt->executeQuery();
    while (res->next()) {
        result_str += res->getInt(1);
    }
    return result_str;
}


std::string MessageHandler::add() {
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
    return "ok, message with body " + params_list[3] + ", to chatid=" + params_list.back() + "is added";
}

std::string MessageHandler::update() {
    sql::Connection *con = connector->get_con();
    sql::ResultSet *res;
    sql::PreparedStatement *pstmt;
    pstmt = con->prepareStatement(
            "delete from message where id=?");
    pstmt->setInt(1, std::stoi(params_list.back()));
    pstmt->executeUpdate();
    delete pstmt;
    return "ok";
}

std::string MessageHandler::del() {
    sql::Connection *con = connector->get_con();
    sql::ResultSet *res;
    sql::PreparedStatement *pstmt;
    pstmt = con->prepareStatement(
            "delete from Message where id=?");
    pstmt->setInt(1, std::stoi(params_list.back()));
    pstmt->executeUpdate();
    delete pstmt;
    return "ok";
}


std::string UserHandler::add() {

    return "";
}

std::string UserHandler::update() {
    return "";
}

std::string UserHandler::del() {
    return "";
}

std::string IObjHandler::choicer() {
    if (params_list[1] == "add")
        add();
    else if (params_list[1] == "del")
        del();
    else if (params_list[1] == "update")
        update();
}