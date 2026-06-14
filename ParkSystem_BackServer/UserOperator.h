#pragma once
#include <cppconn/prepared_statement.h>
#include <cppconn//resultset.h>
#include <iostream>
#include "DBConnection.h"
#include "protocol.h"
#include "LogOperator.h"
#include "CBaseTask.h"
#include <unordered_map>
#include "User.h"
using namespace std;

class UserOperator
{
public:
	UserOperator();
	~UserOperator();

	/// <summary>
	/// 用户查询方法 传入手机号进行用户查询
	/// </summary>
	/// <param name="user_account">手机号码</param>
	/// <returns>true-用户存在 false-用户不存在</returns>
	bool queryUser(string& user_account);

	/// <summary>
	/// 用户注册方法 传入手机号用户名进行数据库插入数据
	/// 默认用户密码123456的MD5加密数据
	/// </summary>
	/// <param name="user_account">手机号</param>
	/// <param name="user_name">用户名</param>
	/// <returns>true-注册成功 false-注册失败</returns>
	bool registerUser(string user_account, string user_name);

	/// <summary>
	/// 数据库处理函数，处理用户登录
	/// 根据传入的用户信息结构体进行对应查询
	/// </summary>
	/// <param name="user">用户信息结构体</param>
	/// <returns>查找到对应用户返回用户名，否则返回空字符串</returns>
	string loginUser(USER& user,int fd);

private:
	DBConnection* connection;
	sql::Connection* con;
	int res;

};

