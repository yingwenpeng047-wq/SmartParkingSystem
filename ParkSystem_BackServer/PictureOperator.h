#pragma once
#include <cppconn/prepared_statement.h>
#include <cppconn//resultset.h>
#include <iostream>
#include "DBConnection.h"
#include "protocol.h"
#include "LogOperator.h"
#include <unordered_map>
#include "User.h"

//图片信息数据库操作类
class PictureOperator
{
public:
	PictureOperator();
	~PictureOperator();

	/// <summary>
	/// 图片记录插入数据库
	/// </summary>
	/// <param name="picture_name">图片名称</param>
	/// <param name="picture_cpath">客户端图片路径</param>
	/// <param name="fd">用于查询用户账号</param>
	/// <returns>ture-成功  false-失败</returns>
	bool insertPicture(string picture_name, string picture_cpath, int fd = 0);

	/// <summary>
	/// 查询图片id，用于外键关联到其他数据表
	/// </summary>
	/// <param name="picture_name">图片名称，用于查询</param>
	/// <returns>成功-图片id  失败-返回 -1</returns>
	int queryPicture(string picture_name);

private:
	DBConnection* connection;
	sql::Connection* con;
};

