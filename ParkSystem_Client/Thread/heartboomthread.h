#ifndef HEARTBOOMTHREAD_H
#define HEARTBOOMTHREAD_H

#include <QThread>
#include "protocol/protocol.h"
#include "tool/ctool.h"
#include <arpa/inet.h>
#include "Thread/writethread.h"
#include "socketconnect/socketconnect.h"

//隔3分钟发送
class HeartBoomThread:public QThread
{
public:
    ~HeartBoomThread();
    static HeartBoomThread *getInstance();
    void setIsRun(bool value);

    void setIsStop(bool value);
    void activate();
    void setSocketfd(int value);

private:
    HeartBoomThread();
    static HeartBoomThread *heartboomThread;

    bool isRun;
    bool isStop;

    HEAD head;

    char buf[100];

    WriteThread *wd;
    int socketfd;
    // QThread interface
protected:
    void run();
};

#endif // HEARTBOOMTHREAD_H
