#ifndef CFILESPLIT_H
#define CFILESPLIT_H
#include <QThread>
#include <QQueue>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <QDebug>
#include "socketconnect/socketconnect.h"
#include <QThreadPool>
#include "myrunnable.h"
#include "Thread/writethread.h"
#include <QDebug>

//负责拆分文件
class CFileSplit:public QThread
{
    Q_OBJECT
public:
    static CFileSplit *getInstance();
    ~CFileSplit();

    //QString
    void addFile(const QString &filePath);

    QString getUser_account() const;
    void setUser_account(const QString &value);

private:
    CFileSplit();
    static CFileSplit *cfileSplit;

    QQueue<QString> manyFile;            //多个文件路径,从里面读取路径，然后开始拆分
    QQueue<FILEINFO> fileinfo;

    FILEINFO tempFileInfo;
    //尾包信息
    FILECHECK filecheck;

    HEAD head;

    QString user_account;

    SocketConnect *sc;
    QThreadPool *pool;

    WriteThread *writethread;

    QString path;
    char buf[1000];

    // QThread interface
protected:
    void run();
};

#endif // CFILESPLIT_H
