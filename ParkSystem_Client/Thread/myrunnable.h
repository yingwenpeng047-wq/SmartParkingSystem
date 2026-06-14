#ifndef MYRUNNABLE_H
#define MYRUNNABLE_H

#include <QRunnable>
#include "protocol/protocol.h"
#include <string.h>
#include "tool/ctool.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <QDebug>


class MyRunnable:public QRunnable
{
public:
    MyRunnable(FILEINFO fileinfo);
    ~MyRunnable();

    // QRunnable interface
public:
    void run();

    void setSocketfd(int value);

private:
    HEAD head;
    FILEINFO fileinfo;

    int socketfd;
    char buf[sizeof(HEAD)+sizeof(FILEINFO)];

};

#endif // MYRUNNABLE_H
