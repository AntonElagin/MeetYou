#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <mysql.h>
#include "datetime.h"

#define MEDIUMTEXT 16777215

class EventCreator
{
public:
	EventCreator(MYSQL *connection) : conn(connection) {};
	~EventCreator();
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

