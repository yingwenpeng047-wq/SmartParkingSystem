#ifndef MYCODE_H
#define MYCODE_H

#include <QString>
#include <QLabel>
#include <QWidget>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>


class MyCode:public QLabel
{
public:
    explicit MyCode(QWidget *parent=nullptr);
    //处理并获取随机验证码
    QString &getCode();
    void refreshCode();
private:
    QString code;
protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
};

#endif // MYCODE_H
