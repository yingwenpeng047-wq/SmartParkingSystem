#ifndef QVIDEOSHOW_H
#define QVIDEOSHOW_H
#include <QThread>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
#include <string>
#include <QDateTime>
#include "cfilesplit.h"
#include <QImage>


#define OPENWIDGET 1
#define VIDEOPLAYERWIDGET 2

#include <QDebug>

class QVideoShow:public QThread
{
    Q_OBJECT
public:
    ~QVideoShow();
    static QVideoShow *getInstance();

    VideoCapture &getCap();

    void setFileName(const string &value);

    bool getIsRun() const;
    void setIsRun(bool value);

    bool getIsStop() const;
    void setIsStop(bool value);

    void activate();
    void stop();
    void changeVideo(const string &fileName,int index);


    int getWhichwidget() const;
    void setWhichwidget(int value);

    void setCurrent(int value);

    int getTotalframe() const;
    void setTotalframe(int value);

    double getRate() const;
    void setRate(double value);

    int getCurrentframeVideoShow() const;
    void setCurrentframeVideoShow(int value);

private:
     QVideoShow();
     static QVideoShow *videoShowThread;

    WriteThread *wd;

    bool isRun;
    bool isStop;

    VideoCapture cap;

    Mat frame;

    int currentframe;

    int totalframe;

    int whichwidget;

    double rate;

    CFileSplit *filethread;

    HEAD head;
    VIDEO videoInfo;
    char buf[500];
    Mat clone;
signals:
    void send2OpenWidget(Mat frame);
    void send2VideoPlayerWidget(Mat frame,int index);
    void change2NextWidegt();
protected:
    void run();
};

#endif // QVIDEOSHOW_H


