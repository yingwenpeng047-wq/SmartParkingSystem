#ifndef PARKINGINNERMAGWIDGET_H
#define PARKINGINNERMAGWIDGET_H

#include <QWidget>
#include "Thread/qvideoshow.h"
#include <QTimer>
#include <QDateTime>
#include "Thread/cfilesplit.h"
#include <QMessageBox>
#include "monitorreplaywin.h"
#include "Thread/readthread.h"
#include "Thread/camerathread.h"

namespace Ui {
class ParkingInnerMagWidget;
}

class ParkingInnerMagWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ParkingInnerMagWidget(QWidget *parent = nullptr);
    ~ParkingInnerMagWidget();

    void init_connect();
    QString getPhotoPath() const;
    void setPhotoPath(const QString &value);

    bool getIsrecording() const;
    void setIsrecording(bool value);


    bool getIsclicked() const;

public slots:
    void showTime();
    void receiveFrame(Mat frame);

    void on_settingBtn_clicked();

    void on_carAndPeopleBtn_clicked();

    void on_startRecordingBtn_clicked();

    void on_autoRecordingBtn_clicked();

    void receiveInfo(int flag,char *info);

    void videoUpBackInfo(int flag,char *info);

    void showmyselfbyMonitor();
signals:
    void go2SettingWin();

private:
    Ui::ParkingInnerMagWidget *ui;
    MonitorReplayWin *monitorWin;
    QTimer *timer;
    CameraThread *camerathread;

    ReadThread *rd;

    QImage img;
    Mat frame;

    QString photoPath;

    CFileSplit *filethread;

    bool isrecording;
    bool isclicked;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
private slots:
    void on_monitorReBtn_clicked();
};

#endif // PARKINGINNERMAGWIDGET_H
