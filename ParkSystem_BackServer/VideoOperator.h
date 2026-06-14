#pragma once
#include <cppconn/prepared_statement.h>
#include <cppconn//resultset.h>
#include <iostream>
#include "DBConnection.h"
#include "protocol.h"
#include <string.h>
#include <ctime>
#include <iomanip>

//视频信息数据库操作类
class VideoOperator
{
public:
	VideoOperator();
	~VideoOperator();

	bool updateVideo(VIDEO& videoInfo, int user_id);

	/// <summary>
	/// 客户端上传视频记录，执行数据库数据插入业务
	/// </summary>
	/// <param name="videoInfo">视频信息结构体</param>
	/// <param name="user_id">用户id</param>
	/// <returns>成功-true  失败-false</returns>
	bool insertVideo(VIDEO& videoInfo, int user_id);

	/// <summary>
	/// 视频记录查询，至多返回9条数据，采用分页查询方式
	/// 仅查询请求者的客户端下的视频
	/// </summary>
	/// <param name="infoSearch">视频信息查询结构体</param>
	/// <param name="user_id">查询所属用户的id</param>
	/// <returns>视频播放信息返回结构体</returns>
	VIDEOBACK queryVideo(VIDEOPLAY& infoSearch, int user_id);

	/// <summary>
	/// 视频列表日期查询，返回服务器中保存的当月的视频日期
	/// 传入用户id用户查询指定的用户所属视频列表信息
	/// </summary>
	/// <param name="user_id">用户id</param>
	/// <returns>视频日期结构体</returns>
	VIDEODATE queryDay(int user_id);

	/// <summary>
	/// 视频列表日期查询，返回服务器中保存的当年的视频日期月份
	/// 传入用户id用户查询指定的用户所属视频列表信息
	/// </summary>
	/// <param name="user_id"></param>
	/// <returns>视频日期结构体</returns>
	VIDEODATE queryMonth(int user_id);

private:
	DBConnection* connection;
	sql::Connection* con;
};

