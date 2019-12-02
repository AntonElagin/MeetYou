#include "../include/handler_classes.h"
#include "../include/connector.h"

bool ChatHandler::add() {
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;
    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "167839");
    con->setSchema("meetyou");
    stmt = con->createStatement();
    res = stmt->executeQuery(
            "insert into Chat (id, members_count, create_date, admin_id, title) values (null, 1, CURDATE(), 3, 'Cebus nigrittatus');");
    string result;
    while (res->next()) {
        result += res->getString(1) + " ";
    }
    delete res;
    delete stmt;
    delete con;
    return true;
}

bool ChatHandler::update() {
    return false;
}

bool ChatHandler::del() {
    return false;
}


bool MessageHandler::add() {
    return false;
}

bool MessageHandler::update() {
    return false;
}

bool MessageHandler::del() {
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