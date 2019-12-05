#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <mysql.h>
#include "datetime.h"

class EventEditor
{
public:
	EventEditor(MYSQL *connection) : conn(connection) {};
	~EventEditor();
private:
	MYSQL *conn;
	int event_id;
	std::vector<int> event_hobies;
	std::string description;
	int creator_id;
	std::string event_gender_id;
	Date date;
	Time time;
	int needed_number_people;
};

