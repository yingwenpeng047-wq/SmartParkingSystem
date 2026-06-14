#ifndef WRITETHREAD_H
#define WRITETHREAD_H
#include <QThread>
#include "protocol/protocol.h"
#include <QDebug>
#include <unistd.h>
#include <string.h>

class WriteThread:public QThread
{
    Q_OBJECT
public:
    static WriteThread *getInstance();
    void run();
    void setData(void *value,int size);
    void setSocketfd(int value);
    bool getIsStart() const;
    void setIsStart(bool value);

private:
    WriteThread();
    static WriteThread *writeThread;
    int socketfd;
    char data[1024];
    int length;

    bool isStart;
};

#endif // WRITETHREAD_H
