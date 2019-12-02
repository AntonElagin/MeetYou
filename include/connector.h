#ifndef PLUS_PROJECT_CONNECTOR_H
#define PLUS_PROJECT_CONNECTOR_H

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

using namespace std;
class Connector {
public:
    sql::Driver *driver;
    sql::Connection *con;

    Connector(const string &endpoint, const string &username, const string &password, const string &schema) {
        sql::SQLString endpoint_sql = endpoint.c_str();
        sql::SQLString username_sql = username.c_str();
        sql::SQLString password_sql = password.c_str();
        sql::SQLString schema_sql = schema.c_str();
        driver = get_driver_instance();
        con = driver->connect(endpoint_sql, username_sql, password_sql);
        con->setSchema(schema_sql);
    }

    Connector(const Connector &) = delete;

    Connector(const Connector &&) = delete;

    sql::Driver *get_driver() {
        return driver;
    }

    sql::Connection *get_con() {
        return con;
    }

    ~Connector() = default;
};

#endif //PLUS_PROJECT_CONNECTOR_H
