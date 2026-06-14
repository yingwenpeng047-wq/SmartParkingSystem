#ifndef OPENWIDGET_H
#define OPENWIDGET_H

#include <QWidget>
#include "Thread/qvideoshow.h"
#include "mainwidget.h"

#include <QMetaType>
Q_DECLARE_METATYPE(Mat)


namespace Ui
{
    class openWidget;
}


class openWidget : public QWidget
{
    Q_OBJECT

public:
    explicit openWidget(QWidget *parent = nullptr);
    ~openWidget();
    void init_connect();

public slots:
    void showImage(Mat frame);
    void change2MainWidget();
private:
    Ui::openWidget *ui;
    QVideoShow *videoShowThread;
    QImage image;

    SettingWidget *settingWidget;
    MainWidget *mainWidget;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // OPENWIDGET_H
