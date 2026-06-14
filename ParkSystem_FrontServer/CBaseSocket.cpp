#include "CBaseSocket.h"

CBaseSocket::CBaseSocket(int type, int sin_family, int protocol)
{
    this->type = type;
    this->sin_family = sin_family;
    this->protocol = protocol;
    
    this->startConnect();
}

int CBaseSocket::getSocketfd()
{
    return 0;
}

void CBaseSocket::startConnect()
{
    this->socketfd = socket(this->sin_family, this->type, this->protocol);
    if (this->socketfd < 0)
    {
        perror("socket error");
    }
}

void CBaseSocket::stopConnect()
{
    if (this->socketfd > 0)
    {
        close(this->socketfd);
        this->socketfd = 0;
    }
}
