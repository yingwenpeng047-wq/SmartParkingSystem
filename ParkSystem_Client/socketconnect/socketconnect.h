#ifndef SOCKETCONNECT_H
#define SOCKETCONNECT_H
//#include "protocol.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
using namespace std;
#include <QString>
#include <QDebug>


//Netconnect class
class SocketConnect
{
public:
    static SocketConnect *getInstance();

    bool createConnect();
    void Close();
    int getSocketfd() const;
    void setSocketfd(int value);

    unsigned short getPort() const;
    void setPort(unsigned short value);

    QString getIp() const;
    void setIp(const QString &value);

private:
    SocketConnect();
    static SocketConnect *sc;
    struct sockaddr_in s_addr;
    int socketfd;
    int len;

    unsigned short port;
    QString ip;
};



#endif // SOCKETCONNECT_H
