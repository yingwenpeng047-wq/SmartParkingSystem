#include "mycode.h"

//在 Qt 里，一个控件的父对象是在其基类的构造函数中被设定的。
MyCode::MyCode(QWidget *parent):QLabel(parent)
{
    this->refreshCode();            //刷新验证码
}


QString &MyCode::getCode()
{
   return this->code;
}


void MyCode::refreshCode()
{
    QString codeNumber="";
    for(int i = 0;i < 4;++i)
    {
        int num = qrand()%3;
        if(num == 0)
        {
            //数字
            codeNumber += QString::number(qrand()%10);
        }
        else if(num == 1)
        {
            //大写字母
            int temp = 'A';
            codeNumber += static_cast<QChar>(temp + qrand()%26);
        } else if(num == 2)
        {
            //小写字母
            int temp = 'a';
            codeNumber += static_cast<QChar>(temp + qrand()%26);
        }
   }
    this->code=codeNumber;
}


void MyCode::mousePressEvent(QMouseEvent *event)
{
    event->setAccepted(true);
    if (this->rect().contains(event->pos()))
    {
        this->refreshCode();        //刷新验证码
        update();                   //触发单次重绘
    }
}


void MyCode::paintEvent(QPaintEvent *event)
{
    event->setAccepted(true);
    QPainter painter(this);
    QPen pen;

    //1.画点
    for(int i = 0; i < 100; ++i)
    {
        painter.setPen(QColor(qrand()%256, qrand()%256, qrand()%256));
        painter.drawPoint(qrand()%width(), qrand()%height());                                 //相对坐标
    }
    // 2. 画干扰线
    for(int i = 0; i < 10; ++i)
    {
        painter.drawLine(qrand()%width(), qrand()%height(),qrand()%width(), qrand()%height()); //相对坐标
    }
    // 3. 绘制验证码文字（居中显示）
    QFont font("楷体", 20, QFont::Bold);
    painter.setFont(font);
    painter.setPen(Qt::red);

    int textWidth = 30 * 4;                 // 4个字符总宽度
    int startX = (width() - textWidth) / 2; // 水平居中

    for(int i = 0; i < 4; ++i)
    {
        painter.drawText(5+startX + 25*i, height()/2-20,30,40,Qt::AlignCenter, QString(this->code[i]));
    }
}




