#include "openwidget.h"
#include "ui_openwidget.h"

openWidget::openWidget(QWidget *parent):QWidget(parent),ui(new Ui::openWidget)
{
    ui->setupUi(this);

    this->mainWidget=new MainWidget();
    this->settingWidget=this->mainWidget->getSetWidget();

    this->videoShowThread=QVideoShow::getInstance();
    this->init_connect();
    if(!this->videoShowThread->getCap().open("../resourceVideo/20250724_104552.mp4"))
    {
        qDebug()<<"open error";
    }
    this->videoShowThread->activate();
    this->videoShowThread->start();
}

openWidget::~openWidget()
{
    delete ui;
}

void openWidget::init_connect()
{
    qRegisterMetaType<cv::Mat>("cv::Mat");  // 必须注册Mat类型
    connect(this->videoShowThread,&QVideoShow::send2OpenWidget,this,&openWidget::showImage,Qt::BlockingQueuedConnection);
    connect(this->videoShowThread,&QVideoShow::change2NextWidegt,this,&openWidget::change2MainWidget,Qt::BlockingQueuedConnection);
}



void openWidget::showImage(Mat frame)
{
    cvtColor(frame,frame,CV_BGR2RGB);
    this->image=QImage(frame.data,frame.cols,frame.rows,QImage::Format_RGB888).scaled(this->ui->videoLabel->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
}

void openWidget::change2MainWidget()
{
    this->videoShowThread->stop();
    this->hide();
    this->videoShowThread->getCap().release();
    this->settingWidget->show();
}

void openWidget::paintEvent(QPaintEvent *event)
{
    //要判断图片是否为空
    if(!this->image.isNull())
    {
        this->ui->videoLabel->setPixmap(QPixmap::fromImage(this->image));
    }
}
