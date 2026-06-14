#ifndef MONITORREPLAYWIN_H
#define MONITORREPLAYWIN_H

#include <QWidget>
#include "Thread/readthread.h"
#include "Thread/writethread.h"
#include <QListWidgetItem>
#include "videoplayerwidget.h"
#include <QMutex>
#include <QMetaObject>
#include <QMutexLocker>

//#include <METATYPE>





namespace Ui {
class MonitorReplayWin;
}

class MonitorReplayWin : public QWidget
{
    Q_OBJECT

public:
    explicit MonitorReplayWin(QWidget *parent = nullptr);
    ~MonitorReplayWin();


    void init_connect();


    void showDay(int number,QStringList list);
    void showMonth(int number,QStringList list);

private slots:
    void on_monthBtn_clicked();

    void on_dayBtn_clicked();

    void onItemClicked(QListWidgetItem *item);

    void showVideo(int num,const QVector<VIDEO> &tempvideo);
    void on_lastPageBtn_clicked();

    void on_nextPageBtn_clicked();

    void videobroadcast(QListWidgetItem *item);

    void sendVideoInfo(int frame,QString jindu);

    void updateVideoInfo(int flag);
    void on_pushButton_clicked();

private:
    Ui::MonitorReplayWin *ui;

    VideoPlayerWidget *videoplay;

    HEAD head;

    WriteThread *wd;
    ReadThread *rd;

    VIDEOPLAY video;

    VIDEO newVideoInfo;

    char buf[100];
    int currentPage;
    int maxPage;

    QVector<VIDEO>totalVec;

    QString currentTime;


    QString videoName;

    int currentindex;

    bool isnext;

   // QListWidgetItem *item;

    QString currentVideoPath;

    QString jindu;

    QMutex videoMutex;
signals:
    //void videosss();
    void backParkingWin();
};

#endif // MONITORREPLAYWIN_H
