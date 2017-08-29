#ifndef _MYSQL_
#define _MYSQL_

#include<iostream>
#include<mysql.h>
#include<string.h>
#include<stdlib.h>
using  namespace std;
class SqlApi
{
public:
	SqlApi(const string& host,
		   const int   & port,
		   const string& user = "root",
		   const string& passwd="",
		   const string& database="wxeast");
	~SqlApi();
	bool connect();
	bool insert(const string& name,
				const string& sex,
				const string& age);
	bool select();
private:
	SqlApi();
	SqlApi(const SqlApi&);
	SqlApi& operator=(const SqlApi&);
private:
	MYSQL * _mysql;//表示的数据库的句柄
	MYSQL_RES * _res;
	string _host;
	string _user;
	string _passwd;
	string _database;
	int    _port;

};

#endif
