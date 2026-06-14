#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
#include <string>
#include <QDateTime>
#include "cfilesplit.h"
#include <QImage>

#define MAINWIDGET 0
#define PARKINGWINDGET 1

#include <QDebug>


class CameraThread:public QThread
{
    Q_OBJECT
public:
    ~CameraThread();
    static CameraThread *getInstance();

    VideoCapture &getCap();

    void setFileName(const string &value);

    bool getIsRun() const;
    void setIsRun(bool value);

    bool getIsStop() const;
    void setIsStop(bool value);

    void activate();
    void stop();
    void changeWindow(int index);


    int getWhichwidget() const;
    void setWhichwidget(int value);

    void setCurrent(int value);

    void setIsRecording(bool value);


    bool getIsautoRecording() const;
    void setIsautoRecording(bool value);

    QString getVideoPath() const;
    void setVideoPath(const QString &value);

     bool getIsRecording() const;

     void setPhotoPath(const QString &value);

private:
     CameraThread();
     static CameraThread *videoShowThread;

    WriteThread *wd;

    bool isRun;
    bool isStop;

    VideoCapture cap;
    QString videoPath;
    QString photoPath;
    Mat frame;

    bool isRecording;

    bool isautoRecording;

    VideoWriter videoWriter;    //视频保存对象

    int recordNum;              //录制的总帧数

    int whichwidget;
    double fps;

    CascadeClassifier car_cascade;

    vector<Rect> faces;

    Mat gray;

    CFileSplit *filethread;

    HEAD head;
    VIDEO videoInfo;
    char buf[500];

    QString current_date;
    QString prefix;
    QString fileName;
    Mat clone;
signals:
    void send2MainWidget(Mat frame);
    void send2ParkingWidget(Mat frame);
protected:
    void run();
};

#endif // QVIDEOSHOW_H
