#include "CTcpServer.h"

CTcpServer::CTcpServer(unsigned short port, int type, int sin_family, int protocol) :CBaseSocket(type, sin_family, protocol)
{
	this->address = new CHostAddress(port);
}

CTcpServer::~CTcpServer()
{
	delete this->address;
}

void CTcpServer::work()
{
	int opt_val = 1;
	setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (const void*)(&opt_val), sizeof(opt_val));	//端口重用设置

	if (bind(socketfd, this->address->getAddr(), this->address->getLength()) == -1) // 绑定套接字到地址
	{
		perror("bind error");
		return;
	}

	if (listen(socketfd, 10) == -1)
	{
		perror("listen error");
		return;
	}
}

CHostAddress* CTcpServer::getAddress()
{
	return this->address;
}

void CTcpServer::setAddress(CHostAddress* address)
{
	this->address = address;
}
