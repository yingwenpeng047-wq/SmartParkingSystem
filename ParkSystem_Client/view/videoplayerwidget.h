#ifndef VIDEOPLAYERWIDGET_H
#define VIDEOPLAYERWIDGET_H

#include <QWidget>
#include "Thread/qvideoshow.h"
#include <QCloseEvent>
//#include <>

namespace Ui {
class VideoPlayerWidget;
}

class VideoPlayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoPlayerWidget(QWidget *parent = nullptr);
    ~VideoPlayerWidget();

    void init_connect();

    void setVideoPath(const QString &value);


    void setCurrentFrame(int value);
public slots:
    void on_stopBtn_clicked();

    void on_horizontalSlider_sliderPressed();

    void on_horizontalSlider_sliderReleased();

    void receiveFrame(Mat frame,int index);

    void changeSpeed(QString text);
    void on_startBtn_clicked();
private:
    Ui::VideoPlayerWidget *ui;

    QVideoShow *thread;

    int totalFrame;
    int currentFrame;
    QImage img;

    QString videoPath;



    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
    void paintEvent(QPaintEvent *event);

signals:
    void updatevideo(int frame,QString jindu);
    void sendCurrentNum(int num);
};

#endif // VIDEOPLAYERWIDGET_H
