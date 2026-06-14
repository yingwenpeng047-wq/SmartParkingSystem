#pragma once
#include <iostream>
#include "UserOperator.h"
#include "RecordOperator.h"
#include "FileLogRecord.h"
#include "PictureOperator.h"
#include "VideoOperator.h"
using namespace std;

//静态工厂模式
class OperatorFactory
{
public:
	enum RepositoryType {
		USER,LOG,CARRECORD,FILELOG,PICTURECORED,
		VIDEORECORD
	};
	static OperatorFactory* getInstance();
	~OperatorFactory();
	void* createRepository(RepositoryType type);

private:
	static OperatorFactory* of;
	OperatorFactory();
	UserOperator* uo;			//用户操作类
	RecordOperator* ro;			//车辆出入库信息类
	FileLogRecord* fr;			//日志文件类
	PictureOperator* po;		//图片信息类
	LogOperator* lo;			//数据库日志类
	VideoOperator* vo;			//视频信息操作类
};

