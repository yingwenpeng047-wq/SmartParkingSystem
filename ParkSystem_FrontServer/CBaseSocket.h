#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
using namespace std;

//网络连接基类
class CBaseSocket
{
public:
	//构造函数	
	CBaseSocket(int type, int sin_family = AF_INET, int protocol = 0);

	//获取网络连接文件描述符
	int getSocketfd();
	//开始网络连接
	void startConnect();
	//断开网络连接
	void stopConnect();
	//纯虚函数 IO复用 和 bind函数
	virtual void work() = 0;

protected:
	int socketfd;	//网络连接文件描述符
	int type;		//通信协议 TCP/UDP
	int sin_family;	//协议簇
	int protocol;	//控制类型
	int acceptfd;	//连接成功的客户端文件描述符
	
};

