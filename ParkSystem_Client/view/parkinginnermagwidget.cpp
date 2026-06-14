#include "parkinginnermagwidget.h"
#include "ui_parkinginnermagwidget.h"

ParkingInnerMagWidget::ParkingInnerMagWidget(QWidget *parent):QWidget(parent),ui(new Ui::ParkingInnerMagWidget)
{
    ui->setupUi(this);
    this->monitorWin=new MonitorReplayWin();
    this->rd=ReadThread::getInstance();
    this->isrecording=false;
    this->isclicked=false;
    this->timer=new QTimer();
    this->timer->start(1000);
    this->camerathread=CameraThread::getInstance();
    this->filethread=CFileSplit::getInstance();
    this->init_connect();
}

ParkingInnerMagWidget::~ParkingInnerMagWidget()
{
    delete ui;
}

void ParkingInnerMagWidget::init_connect()
{
    connect(this->timer,&QTimer::timeout,this,&ParkingInnerMagWidget::showTime);
    connect(this->camerathread,&CameraThread::send2ParkingWidget,this,&ParkingInnerMagWidget::receiveFrame,Qt::BlockingQueuedConnection);
    connect(this->rd,&ReadThread::send2ParkinWin2,this,&ParkingInnerMagWidget::videoUpBackInfo,Qt::BlockingQueuedConnection);
    connect(this->monitorWin,&MonitorReplayWin::backParkingWin,this,&ParkingInnerMagWidget::showmyselfbyMonitor);
}

void ParkingInnerMagWidget::showTime()
{
    QDateTime currentDateTime =QDateTime::currentDateTime();
    QString timeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");       //显示时间
    this->ui->timeLabel->setText(timeString);
}

void ParkingInnerMagWidget::receiveFrame(Mat frame)
{
    cvtColor(frame,frame,CV_BGR2RGB);
    this->img=QImage(frame.data,frame.cols,frame.rows,QImage::Format_RGB888).scaled(this->ui->photoLabel->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
}

void ParkingInnerMagWidget::paintEvent(QPaintEvent *event)
{
    if(!this->img.isNull())
    {
        this->ui->photoLabel->setPixmap(QPixmap::fromImage(this->img));
    }
}


void ParkingInnerMagWidget::on_settingBtn_clicked()
{
    emit this->go2SettingWin();
}

void ParkingInnerMagWidget::on_carAndPeopleBtn_clicked()
{
    this->isclicked=true;
    QDateTime currentDateTime =QDateTime::currentDateTime();
    QString timeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");       //显示时间
    bool success = this->img.save(this->photoPath+"/"+timeString+".jpg");
    if(!success)
    {
        qDebug()<<"photo save fail";
        return;
    }
    this->filethread->addFile(this->photoPath+"/"+timeString+".jpg");
}

QString ParkingInnerMagWidget::getPhotoPath() const
{
    return photoPath;
}

void ParkingInnerMagWidget::setPhotoPath(const QString &value)
{
    photoPath = value;
}

void ParkingInnerMagWidget::on_startRecordingBtn_clicked()
{
    if(!camerathread->getIsRecording()) {
          // 确保自动录制已关闭
          camerathread->setIsautoRecording(false);
          camerathread->setIsRecording(true);
          ui->startRecordingBtn->setText("停车场监控手动录制--暂停录制");
      } else {
          camerathread->setIsRecording(false);
          ui->startRecordingBtn->setText("停车场监控手动录制--开始录制");
      }

}

bool ParkingInnerMagWidget::getIsrecording() const
{
    return isrecording;
}

void ParkingInnerMagWidget::setIsrecording(bool value)
{
    isrecording = value;
}

void ParkingInnerMagWidget::on_autoRecordingBtn_clicked()
{
    if(this->camerathread->getIsautoRecording()==false)
    {
        if(this->ui->autoRecordingBtn->text()=="☐")
        {
            this->camerathread->setIsRecording(false);  // 先停止手动录制
            this->camerathread->setIsautoRecording(true);
            this->ui->autoRecordingBtn->setText("√");
        }
    }
}

void ParkingInnerMagWidget::receiveInfo(int flag, char *info)
{
    if(this->isclicked==true)
    {
        if(flag==1)
        {
            QMessageBox::information(this,"停车场内部管理界面","图片上传成功",QMessageBox::Ok,QMessageBox::Ok);
            this->isclicked=false;
        }
    }
}

void ParkingInnerMagWidget::videoUpBackInfo(int flag, char *info)
{
    if(flag==1)
    {
        if(this->ui->autoRecordingBtn->text()=="√")
        {
            this->ui->autoRecordingBtn->setText("☐");
        }
        QMessageBox::information(this,"停车场内部管理界面","视频信息上传成功",QMessageBox::Ok,QMessageBox::Ok);
    }
}

void ParkingInnerMagWidget::showmyselfbyMonitor()
{
    this->camerathread->changeWindow(PARKINGWINDGET);
    this->monitorWin->hide();
    this->show();

}

bool ParkingInnerMagWidget::getIsclicked() const
{
    return isclicked;
}

void ParkingInnerMagWidget::on_monitorReBtn_clicked()
{
    this->camerathread->stop();
    this->hide();
    this->monitorWin->show();
}
