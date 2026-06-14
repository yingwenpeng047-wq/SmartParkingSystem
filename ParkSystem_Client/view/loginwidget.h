#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include "reginwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include "mycode.h"
#include "socketconnect/socketconnect.h"
#include <QCryptographicHash>
#include "Thread/cfilesplit.h"
#include "Thread/camerathread.h"
#include "Thread/heartboomthread.h"


namespace Ui {
    class LoginWidget;
}

class LoginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();
    void init_connect();

private:
    Ui::LoginWidget *ui;
    ReginWidget *reginWidget;
    QTimer *timer;

    SocketConnect *sc;

    HeartBoomThread *heartThread;

    MyCode *code;                        //验证码对象
    QString codeNumber;                  //验证码数字

    ReadThread *rd;
    WriteThread *wd;

    HEAD head;

    USER login;

    char buf[200];

    CFileSplit *thread;

    CameraThread *camerathread;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
private slots:
    void on_reginBtn_clicked();
    void showLoginWin();

    void showLoginWin(QString phone);
    void on_loginBtn_clicked();

    void receiveLoginInfo1(int flag,char *info);
    void receiveLoginInfo2(int num,const QVector<UPDATATABLE> &temptable);
    void sendheart();

signals:
    void go2Mainwin();
    void tableshow(int num, const QVector<UPDATATABLE> &temptable);
};

#endif // LOGINWIDGET_H
