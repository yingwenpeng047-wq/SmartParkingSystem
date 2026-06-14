#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>
#include "loginwidget.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include "socketconnect/socketconnect.h"
#include "Thread/cfilesplit.h"
#include "Thread/qvideoshow.h"
#include "Thread/camerathread.h"


namespace Ui {
    class SettingWidget;
}

class SettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingWidget(QWidget *parent = nullptr);
    ~SettingWidget();


    QString getPhotoPath() const;
    void setPhotoPath(const QString &value);

    QString getVideoPath() const;
    void setVideoPath(const QString &value);

private slots:
    void on_photoSaveBtn_clicked();

    void on_videoSaveBtn_clicked();

    void on_enterBtn_clicked();

    void on_backBtn_clicked();

    //void showSettingWidget();
private:
    Ui::SettingWidget *ui;
    QVideoShow *videoThread;
    CameraThread *camerathread;

    SocketConnect *sc;
    QString photoPath;
    QString videoPath;
    CFileSplit *thread;
    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);

signals:
    void back2MainWin();
    void go2Loginwin();
};

#endif // SETTINGWIDGET_H
