#pragma once
#include <string>
#include <vector>
class EventEditor
{
public:
	EventEditor();
	~EventEditor();
	std::string event_name;
	change_type();
	change_description();
	change_gender();
	change_date();
	change_time();
	add_person();
	remove_person();
	generator_of_signal(); // принимает значение char или int

private:
	int event_id;

protected:
	std::vector<std::string> people;
	std::vector<int> gender;
};

