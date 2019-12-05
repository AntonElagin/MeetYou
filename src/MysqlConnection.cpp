#include "pch.h"
#include "../include/MysqlConnection.h"


MysqlConnection::MysqlConnection()
{
	conn = mysql_init(NULL);
	if (conn == NULL)
	{
		// ���� ���������� �� ������� � ������� ��������� �� ������
		fprintf(stderr, "Error: can't create MySQL-descriptor\n");
	}
	// ������������ � �������
	if (!mysql_real_connect(conn, "localhost", "root", "KsenyaAlexPall", "meetyou", 3306, NULL, 0))
	{
		// ���� ��� ����������� ���������� ���������� � �������� 
		// ���� ������ ������� ��������� �� ������
		fprintf(stderr, "Error: can't connect to database %s\n", mysql_error(conn));
	}
	else
	{
		// ���� ���������� ������� ����������� ������� ����� - "Success!"
		fprintf(stdout, "Success!\n");
		/*
		const char* query = "SELECT * FROM test";
		int qstate = mysql_query(conn, query);
		if (!qstate) {
			MYSQL_RES *res = mysql_store_result(conn);
			MYSQL_ROW row;
			while (row = mysql_fetch_row(res))
				printf("ID: %s, Name: %s, Value: %s\n", row[0], row[1], row[2]);
		}
		*/
	}
}


MysqlConnection::~MysqlConnection()
{
	// ��������� ���������� � �������� ���� ������
	mysql_close(conn);
}
