#pragma once
#include "protocol.h"
#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "CBaseTask.h"
#include "WriteMemShare.h"
using namespace std;

//任务类
class CSendTask:public CBaseTask
{
public:
	//参数：请求头-head 文件描述符-fd
	CSendTask(char* buf,int fd,int size);
	~CSendTask();

	//执行任务函数
	virtual void work();

	//getset方法
	HEAD getHead();
	void setHead(HEAD* head);

private:
	HEAD head;		//请求头
	char buf[2560];	//数据缓存
	int fd;			//网络文件描述符
};

