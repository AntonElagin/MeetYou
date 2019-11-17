#pragma once
#include <string>
#include <vector>
class EventCteator
{
public:
	EventCteator();
	~EventCteator();
	std::string event_name;
	change_type();
	change_description();
	change_gender();
	change_date();
	change_time();
	add_person();
	generator_of_signal(); // ��������� �������� char ��� int
	
private:
	int event_id;

protected:
	std::vector<std::string> people;
	std::vector<int> gender;
};

