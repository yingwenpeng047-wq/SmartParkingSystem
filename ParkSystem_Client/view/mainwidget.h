#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "parkinginnermagwidget.h"
#include <easypr.h>
#include <QDebug>
using namespace easypr;
#include "Thread/qvideoshow.h"
#include <QTimer>
#include <QDateTime>
#include "settingwidget.h"
#include "loginwidget.h"
#include "carinfocheckwidget.h"
#include "Thread/camerathread.h"


namespace Ui
{
    class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

    void init_connect();
    SettingWidget *getSetWidget() const;
    void setSetWidget(SettingWidget *value);

    QString formatTimeDifference(const QString &timeStr1, const QString &timeStr2);

    int checkMinutesAndSeconds(const QString &timeStr);
private slots:
    void on_entryModeBtn_clicked();

    void on_settingBtn_clicked();

    void on_innerMagBtn_clicked();

    void on_recognizeBtn1_clicked();

    void showTime();
    void showMyself();
    void showMyselfByLogin();
    void receiveFrame(Mat frame);
    void showLogin();
    void on_carSearchBtn_clicked();

    void on_recognizeCarBtn2_clicked();

    void showSettingByParking();

    void receiveFileInfo(int flag,char *info);
    void updateTable(int flag,char *info);
    void on_uploadPhotoBtn_clicked();
    void showOuterInfo(int flag,char *info);
    void on_canGoBtn_clicked();


    void on_monthBtn_clicked();

    void isCanGo(int flag,char *info);

    void on_modifyCarBtn_clicked();

    void on_modifyBtn1_clicked();

    void showmyselfbyCarinfoWin();

    void showTableinit(int num,const QVector<UPDATATABLE> &temptable);
    void on_tableWidget_doubleClicked(const QModelIndex &index);

signals:
    void send2SettingWidget();


private:
    WriteThread *wd;
    Ui::MainWidget *ui;
    int currentIndex;
    ParkingInnerMagWidget *parkingWidget;

    ReadThread *rd;

    CameraThread *camerathread;
    QImage img;
    QImage img2;
    Mat plateMat;
    Mat frame;
    CPlateRecognize pr;
    vector<CPlate>plateVec;
    CPlate plate;
    QTimer *timer;

    SettingWidget *setWidget;
    LoginWidget *loginWindget;
    CarInfoCheckWidget *carinfoCheckWin;

    QString photoName;

    CFileSplit *filethread;

    int tableIndex;

    HEAD head;
    INOUTRECORD inoutRecord;
    char buf[500];

    QImage outImg;
    QString outTime;

    QImage inImg;
    QString inTime;
    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // MAINWIDGET_H
