#ifndef READTHREAD_H
#define READTHREAD_H
#include <QThread>
#include "protocol/protocol.h"
#include <unistd.h>
#include <string.h>
#include <QDebug>
#include "tool/ctool.h"
#include <arpa/inet.h>
#include <QStringList>
#include <QVector>
using namespace std;

Q_DECLARE_METATYPE(VIDEO);
//Q_DECLARE_METATYPE(CARINFO);
//Q_DECLARE_METATYPE(UPDATATABLE);

class ReadThread:public QThread
{
    Q_OBJECT
public:
    ~ReadThread();
    static ReadThread* getInstance();

    int getSocketfd() const;
    void setSocketfd(int value);

    bool getIsStart() const;
    void setIsStart(bool value);

private:
    ReadThread();
    static ReadThread* readThread;
    int socketfd;
    HEAD head;
    RESULTBACK resultback;
    FILEBACK fileback;
    INFOBACK carinfoback;
    VIDEODATE dayinfoback;
    VIDEOBACK videoback;
    TABLES tableback;

    char buf[200];

    bool isStart;

    QVector<VIDEO> tempvideo;

signals:
    void Send2ReginWin(int flag,char *info);
    void send2LoginWin(int flag,char *info);
    void send2MainWin(int flag,char *info);
    void send2ParkingWin(int flag,char *info);
    void send2MainWin2(int flag,char *info);
    void send2MainWin3(int flag,char *info);
    void send2MainWin4(int flag,char *info);
    void send2ParkinWin2(int flag,char *info);
    void send2CarinfoWin(int num,const QVector<CARINFO> &carinfo);
    void send2MonitorWin(int num,QStringList list);
    void send2MonitorWin2(int num,QStringList list);

    void send2MonitorWin3(int num,const QVector<VIDEO> &tempvideo);

    void send2LoginWin5(int num,const QVector<UPDATATABLE> &tableinfo);
    void send2MonitorWin4(int flag);
    // QThread interface
protected:
    void run();
};

#endif // READTHREAD_H

