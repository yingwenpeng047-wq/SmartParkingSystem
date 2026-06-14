#ifndef CARINFOCHECKWIDGET_H
#define CARINFOCHECKWIDGET_H

#include <QWidget>
#include "protocol/protocol.h"
#include "Thread/readthread.h"
#include "Thread/writethread.h"
#include <QMessageBox>
#include <QDateTime>
#include <QFileInfo>

namespace Ui {
class CarInfoCheckWidget;
}

class CarInfoCheckWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CarInfoCheckWidget(QWidget *parent = nullptr);
    ~CarInfoCheckWidget();

    void init_connect();
private slots:
    void on_checkBtn_clicked();
    void showCarInfo(int num,const QVector<CARINFO> &carinfo);
    void on_backMainWinBtn_clicked();

    void on_lastPageBtn_clicked();

    void on_nextPageBtn_clicked();

    void on_dataexportBtn_clicked();

private:
    Ui::CarInfoCheckWidget *ui;

    WriteThread *wd;
    ReadThread *rd;
    HEAD head;
    INFOSEARCH carinfo;

    char buf[100];
    int currentPage;
    int maxPage;
    bool isnext;
    QVector<CARINFO> tempcarinfo;
    CARINFO car;

signals:
    void backMain();
};

#endif // CARINFOCHECKWIDGET_H
