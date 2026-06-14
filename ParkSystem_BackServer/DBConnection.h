#pragma once
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <memory>
#include <iostream>
#include <mutex>
using namespace std;
using namespace sql::mysql;

const string localhost = "127.0.0.1";
const string localuser = "root";
const string localpasswd = "Password_123";
const string localport = "3306";
const string localschema = "OMO241212";

//数据库连接类
class DBConnection
{
public:
	static DBConnection* dbconn;
	static DBConnection* getInstance();
	sql::Connection* getConnection();
	// 数据库重连方法
	bool reconnect();
	~DBConnection();
	void disConnection();
private:
	static std::mutex conn_mutex;  // 添加互斥锁
	DBConnection();
	MySQL_Driver* driver;
	std::unique_ptr<sql::Connection> conn;
	string path;
};
