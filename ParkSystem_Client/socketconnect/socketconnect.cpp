#include "socketconnect.h"

SocketConnect *SocketConnect::sc=nullptr;

SocketConnect *SocketConnect::getInstance()
{
    if(SocketConnect::sc==nullptr)
    {
        SocketConnect::sc=new SocketConnect();
    }
    return SocketConnect::sc;
}

SocketConnect::SocketConnect()
{
    this->socketfd=socket(AF_INET, SOCK_STREAM, 0);
    this->len=sizeof(s_addr);
}

QString SocketConnect::getIp() const
{
    return ip;
}

void SocketConnect::setIp(const QString &value)
{
    ip = value;
}

unsigned short SocketConnect::getPort() const
{
    return port;
}

void SocketConnect::setPort(unsigned short value)
{
    port = value;
}


bool SocketConnect::createConnect()
{
    if (socketfd==-1)
    {
        perror("socket error");
        return false;
    }
    else
    {
        this->s_addr.sin_family = AF_INET;
        //设置端口号 0-65535 10000以下很多都是系统使用+已知常用应用程序，自定义端口10000以后
        this->s_addr.sin_port = htons(this->port);				//大小端转换--端口号
        //客户端设置的是服务器的IP地址，在网络中寻找服务器
        //this->s_addr.sin_addr.s_addr = inet_addr("192.168.184.128");
        this->s_addr.sin_addr.s_addr = inet_addr(this->ip.toStdString().c_str());
        //this->s_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        len = sizeof(this->s_addr);
        if (connect(socketfd, (struct sockaddr*)(&this->s_addr), this->len) == -1)
        {
            perror("connect error");
            return false;
        }
        else{
            qDebug()<<"connect success";
            return true;
        }
    }
    return false;
}

void SocketConnect::Close()
{
    if(this->socketfd==-1)
    {
        perror("socket error");
    }
    else{
        close(this->socketfd);
    }
}

int SocketConnect::getSocketfd() const
{
    return this->socketfd;
}

void SocketConnect::setSocketfd(int value)
{
    this->socketfd=value;
}






