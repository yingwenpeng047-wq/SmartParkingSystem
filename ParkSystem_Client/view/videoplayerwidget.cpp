#include "videoplayerwidget.h"
#include "ui_videoplayerwidget.h"


//总的容器里面的当前帧数量没有改变

VideoPlayerWidget::VideoPlayerWidget(QWidget *parent) :QWidget(parent),ui(new Ui::VideoPlayerWidget)
{
    ui->setupUi(this);
    this->ui->horizontalSlider->setMinimum(0);
    this->ui->horizontalSlider->setSingleStep(1);
    this->thread=QVideoShow::getInstance();

    this->init_connect();
}

VideoPlayerWidget::~VideoPlayerWidget()
{
    this->thread->stop(); // 确保线程停止
    delete ui;
}

void VideoPlayerWidget::init_connect()
{
    connect(this->ui->rateBox,&QComboBox::currentTextChanged,this,&VideoPlayerWidget::changeSpeed);
    connect(this->thread,&QVideoShow::send2VideoPlayerWidget,this,&VideoPlayerWidget::receiveFrame);
}


void VideoPlayerWidget::setCurrentFrame(int value)
{
    this->currentFrame = value;
}





void VideoPlayerWidget::on_stopBtn_clicked()
{
    if(this->ui->stopBtn->text()=="暂停播放")
    {
        this->thread->stop();
        this->ui->stopBtn->setText("继续播放");
    }
    else if(this->ui->stopBtn->text()=="继续播放"){
        this->thread->activate();
        this->ui->stopBtn->setText("暂停播放");
    }
}


void VideoPlayerWidget::on_horizontalSlider_sliderPressed()
{
    //线程暂停
    this->thread->stop();
}

void VideoPlayerWidget::on_horizontalSlider_sliderReleased()
{
    //改变帧数，并且重新启动线程
    this->thread->setCurrentframeVideoShow(this->ui->horizontalSlider->value());
    this->thread->activate();
}

void VideoPlayerWidget::receiveFrame(Mat frame, int index)
{
    this->currentFrame=index;
    //将传过来的mat对象转化成qimage显示出
    cvtColor(frame,frame,CV_BGR2RGB);
    this->img=QImage(frame.data,frame.cols,frame.rows,QImage::Format_RGB888).scaled(this->ui->photoLabel->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    //滑轮也要修改
    this->ui->horizontalSlider->setValue(index);
    //当前时长也要显示


    update();
}

void VideoPlayerWidget::changeSpeed(QString text)
{
    //获取到具体倍数
    int index = text.indexOf("倍速");
    if(index <= 0)return;          //没有找到"倍速"或前面没有内容
    //提取"倍速"前面的部分
    QString numStr = text.left(index);

    this->thread->stop();
    this->thread->setRate(numStr.toDouble());
    this->thread->activate();
}



void VideoPlayerWidget::setVideoPath(const QString &value)
{
    videoPath = value;
    if (this->thread->isRunning()) {
        this->thread->stop();
    }
}


void VideoPlayerWidget::paintEvent(QPaintEvent *event)
{
    //要判断图片是否为空
    if(!this->img.isNull())
      {
          this->ui->photoLabel->setPixmap(QPixmap::fromImage(this->img));
    }
}


void VideoPlayerWidget::on_startBtn_clicked()
{
    this->thread->changeVideo(this->videoPath.toStdString(),VIDEOPLAYERWIDGET);
    this->totalFrame=this->thread->getTotalframe();
    //设置总范围和当前的帧数
    this->ui->horizontalSlider->setMaximum(this->totalFrame);
    this->ui->horizontalSlider->setRange(0,this->totalFrame);
    this->ui->horizontalSlider->setValue(this->currentFrame);
    this->thread->setCurrentframeVideoShow(this->currentFrame);

}


void VideoPlayerWidget::closeEvent(QCloseEvent *event)
{
    //线程暂停
    this->thread->stop();
    if (this->totalFrame > 0) {
           int percent = qRound((double)this->currentFrame / this->totalFrame * 100);
           QString jindu = QString("已经观看%1%").arg(percent);
               //视频播放完应该要给服务器发一个新的视频信息
           emit this->updatevideo(this->currentFrame, jindu);
       }
    //接受关闭事件
    event->accept();

}
