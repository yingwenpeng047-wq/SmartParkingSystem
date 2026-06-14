#pragma once
#include <cppconn/prepared_statement.h>
#include <cppconn//resultset.h>
#include <iostream>
#include "DBConnection.h"
#include "protocol.h"
#include <string.h>
#include "LogOperator.h"
#include "WriteMemShare.h"
#include <unordered_map>
using namespace std;

//查询车辆出入场记录
class RecordOperator
{
public:
	RecordOperator();
	~RecordOperator();

	/// <summary>
	/// 车辆出入场记录查询，传入车牌号和时间段，进行出入场查询
	/// </summary>
	/// <param name="car_number">车牌号</param>
	/// <param name="in_time">入场时间</param>
	/// <param name="out_time">出场时间</param>
	/// <param name="page">页码</param>
	/// <returns>车辆信息查询返回体指针</returns>
	INFOBACK queryRecord(string car_number, string in_time, string out_time, int page, int user_id);

	/// <summary>
	/// 插入车辆入库记录
	/// </summary>
	/// <param name="car_plate">车牌号</param>
	/// <param name="car_intime">入库时间</param>
	/// <param name="car_inplace">入库地点</param>
	/// <param name="in_img_id">入库图片id</param>
	/// <returns>true-成功 false-失败</returns>
	bool insertRecord(string car_plate,string car_intime,string car_inplace,int in_img_id);

	/// <summary>
	/// 更新车辆出库记录
	/// </summary>
	/// <param name="car_plate">车牌号</param>
	/// <param name="car_outtime">出库时间</param>
	/// <param name="car_outplace">出库地点</param>
	/// <param name="out_img_id">出库图片id</param>
	/// <param name="car_cost">停车金额</param>
	/// <returns>true-成功 false-失败</returns>
	bool updateRecord(string car_plate,string car_outtime,string car_outplace,int out_img_id,int car_cost);

	/// <summary>
	/// 查询车辆入库时间
	/// </summary>
	/// <param name="number"></param>
	/// <returns>返回车辆入库时间，若未查到返回空串</returns>
	string queryTime(string number);

	/// <summary>
	/// 查询当日最新的四条车辆入场记录
	/// </summary>
	/// <returns>入场表格结构体</returns>
	TABLES queryInRecord(int user_id);

private:
	DBConnection* connection;
	sql::Connection* con;
};

