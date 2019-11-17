#pragma once
#include <string>
class SQLConnection
{
public:
	SQLConnection();
	~SQLConnection();
	back();
	select();
	update();
	insert();
	exec_procedure();
	get_password();
	get_login();
private:
	std::string login;
	std::string password;
};

