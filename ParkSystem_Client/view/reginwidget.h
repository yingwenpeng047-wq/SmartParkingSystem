#ifndef REGINWIDGET_H
#define REGINWIDGET_H


#include <QWidget>
#include "Thread/readthread.h"
#include "Thread/writethread.h"
#include <QMessageBox>
#include <QRegExp>
#include <arpa/inet.h>
#include <QTimer>
#include <string>
#include "tool/ctool.h"
using namespace std;


namespace Ui {
    class ReginWidget;
}

class ReginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReginWidget(QWidget *parent = nullptr);
    ~ReginWidget();

    void init_connect();

private slots:
    void on_reginBtn_clicked();

    void on_backBtn_clicked();

    void on_testBtn_clicked();

    void testCannotClick();

    void receiveReginInfo(int flag,char *info);
signals:
    void regin2Login();

    void go2Loginwin(QString phone);
private:
    Ui::ReginWidget *ui;
    QTimer *timer;

    ReadThread *rd;
    WriteThread *wd;

    HEAD head;

    REGISTER reg;

    char buf[200];

    QString testNum;

    bool isRegin;

    int currentSecond;
};

#endif // REGINWIDGET_H
