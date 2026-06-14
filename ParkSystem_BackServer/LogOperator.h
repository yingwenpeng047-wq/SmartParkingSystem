#pragma once
#include <cppconn/prepared_statement.h>
#include <cppconn//resultset.h>
#include <iostream>
#include "DBConnection.h"
#include "protocol.h"
#include <ctime>
#include <chrono>
using namespace std;

//数据库日志操作类
class LogOperator
{
public:
	LogOperator();
	~LogOperator();

	/// <summary>
	/// 插入日志数据到数据库
	/// </summary>
	/// <param name="user_id">用户id</param>
	/// <param name="log_operator">日志操作</param>
	/// <returns>成功返回true  失败返回false</returns>
	bool insertLog(int user_id, string log_operator);

private:
	sql::Connection* con;
	DBConnection* connection;
};

