#pragma once
#include <string>

class User
{
public:
	User();
	~User();
	std::string User_name;

	create_event();
	delete_event();
	change_event();
	sort_events();
	select_events();
	show_events();
	respond_events();
	generator_of_signal();

private:
	std::string Login;
	std::string Password;
};

