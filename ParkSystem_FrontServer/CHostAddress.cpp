#include "CHostAddress.h"

CHostAddress::CHostAddress(unsigned short port, unsigned int ip)
{
    this->port = port;
    this->ip = ip;

    this->addr.sin_family = AF_INET;
    this->addr.sin_port = htons(port);
    this->addr.sin_addr.s_addr = this->ip;

    this->length = sizeof(this->addr);
}

CHostAddress::~CHostAddress()
{
    
}

int CHostAddress::getLength()
{
    return this->length;
}

sockaddr* CHostAddress::getAddr()
{
    return (struct sockaddr*)(&this->addr);
}

sockaddr_in CHostAddress::getAddr_in()
{
    return this->addr;
}

unsigned short CHostAddress::getPort()
{
    return this->port;
}

void CHostAddress::setPort(unsigned short port)
{
    this->port = port;
}

unsigned int CHostAddress::getIP()
{
    return this->ip;
}

void CHostAddress::setIP(unsigned int ip)
{
    this->ip = ip;
}
