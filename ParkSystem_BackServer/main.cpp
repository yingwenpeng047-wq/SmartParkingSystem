#include <iostream>
#include "ReadMemShare.h"
#include "WriteMemShare.h"
#include "CBaseTask.h"
#include <unordered_map>
using namespace std;

//用于记录客户端登录的fd与用户类实例
unordered_map<int, User*> userMap;

int main()
{
	//初始化CRC校验
	CBaseTask::init_crc_table();

	//初始化连接数据库
	DBConnection* connection = DBConnection::getInstance();
	sql::Connection* con = connection->getConnection();

	//读线程作为主线程运行中，创建实例顺序不可更改
	WriteMemShare* writeThread = new WriteMemShare();
	ReadMemShare* readThread = new ReadMemShare();

	readThread->readMsg();

	return 0;
}