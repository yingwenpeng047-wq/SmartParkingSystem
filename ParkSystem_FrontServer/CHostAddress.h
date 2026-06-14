#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>


//地址类
class CHostAddress
{
public:
	CHostAddress(unsigned short port, unsigned int ip = INADDR_ANY);
	~CHostAddress();

	int getLength();
	struct sockaddr* getAddr();
	struct sockaddr_in getAddr_in();

	unsigned short getPort();
	void setPort(unsigned short port);

	unsigned int getIP();
	void setIP(unsigned int ip);

private:
	unsigned short port;		//端口号
	unsigned int ip;			//IP地址
	struct sockaddr_in addr;	//socket结构体
	int length;					//结构体长度
};

