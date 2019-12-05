#include <iostream>
#include <windows.h>
#include <mysql.h>
class MysqlConnection
{
public:
	MysqlConnection();
	~MysqlConnection();
private:
	MYSQL *conn;
};

