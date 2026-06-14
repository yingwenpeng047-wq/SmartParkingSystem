#include "mainwidget.h"
#include "ui_mainwidget.h"

//发送上去的车牌都是不带蓝牌两个字的，表格里面也是，但是编辑框里面是带的

//数据缺失的时候的包处理
MainWidget::MainWidget(QWidget *parent):QWidget(parent),ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    this->wd=WriteThread::getInstance();
    this->timer=new QTimer();
    this->timer->start(1000);
    this->tableIndex=0;
    bzero(this->buf,sizeof(this->buf));
    bzero(&this->head,sizeof(HEAD));
    bzero(&this->inoutRecord,sizeof(INOUTRECORD));

    this->filethread=CFileSplit::getInstance();
    this->rd=ReadThread::getInstance();

    this->pr.setDetectType(PR_DETECT_CMSER | PR_DETECT_COLOR);
    this->pr.setMaxPlates(4);
    this->pr.setResultShow(false);
    this->camerathread=CameraThread::getInstance();

    this->setWidget=new SettingWidget();
    this->parkingWidget=new ParkingInnerMagWidget();
    this->carinfoCheckWin=new CarInfoCheckWidget();
    this->parkingWidget->setPhotoPath(this->setWidget->getPhotoPath());
    this->loginWindget=new LoginWidget();

    this->currentIndex=0;

    this->ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->init_connect();
}

MainWidget::~MainWidget()
{
    delete ui;
}


void MainWidget::init_connect()
{
    qRegisterMetaType<QVector<UPDATATABLE>>("QVector<UPDATATABLE>");  //
    connect(this->timer,&QTimer::timeout,this,&MainWidget::showTime);
    connect(this->setWidget,&SettingWidget::back2MainWin,this,&MainWidget::showMyself);
    connect(this->loginWindget,&LoginWidget::go2Mainwin,this,&MainWidget::showMyselfByLogin);

    connect(this->camerathread,&CameraThread::send2MainWidget,this,&MainWidget::receiveFrame,Qt::BlockingQueuedConnection);

    connect(this->setWidget,&SettingWidget::go2Loginwin,this,&MainWidget::showLogin);
    connect(this->parkingWidget,&ParkingInnerMagWidget::go2SettingWin,this,&MainWidget::showSettingByParking);
    connect(this->rd,&ReadThread::send2MainWin,this,&MainWidget::receiveFileInfo,Qt::BlockingQueuedConnection);
    connect(this->rd,&ReadThread::send2ParkingWin,this->parkingWidget,&ParkingInnerMagWidget::receiveInfo,Qt::BlockingQueuedConnection);
    connect(this->rd,&ReadThread::send2MainWin2,this,&MainWidget::updateTable,Qt::BlockingQueuedConnection);
    connect(this->rd,&ReadThread::send2MainWin3,this,&MainWidget::showOuterInfo,Qt::BlockingQueuedConnection);
    connect(this->rd,&ReadThread::send2MainWin4,this,&MainWidget::isCanGo,Qt::BlockingQueuedConnection);
    connect(this->carinfoCheckWin,&CarInfoCheckWidget::backMain,this,&MainWidget::showmyselfbyCarinfoWin);
    connect(this->loginWindget,&LoginWidget::tableshow,this,&MainWidget::showTableinit);
}


//出入场切换
void MainWidget::on_entryModeBtn_clicked()
{
    //由第一页换成第二页，同时按钮名称改为入场模式
    if(this->currentIndex==0)
    {
        this->ui->stackedWidget->setCurrentIndex(1);
        this->currentIndex=1;
        this->ui->entryModeBtn->setText("入场模式");
    }
    else if(this->currentIndex==1)
    {
        this->ui->stackedWidget->setCurrentIndex(0);
        this->currentIndex=0;
        this->ui->entryModeBtn->setText("出场模式");
    }
}


//前往设置界面
void MainWidget::on_settingBtn_clicked()
{
    this->camerathread->stop();
    this->ui->stackedWidget->setCurrentIndex(0);
    this->ui->entryModeBtn->setText("出场模式");
    this->currentIndex=0;
    this->hide();
    this->setWidget->show();
}


//前往停车场界面
void MainWidget::on_innerMagBtn_clicked()
{
    this->camerathread->changeWindow(PARKINGWINDGET);
    this->hide();
    this->parkingWidget->show();
}


//前往车辆信息查询界面
void MainWidget::on_carSearchBtn_clicked()
{
    this->camerathread->stop();
    this->hide();
    this->carinfoCheckWin->show();
}


void MainWidget::on_recognizeBtn1_clicked()
{
    QDateTime currentDateTime =QDateTime::currentDateTime();
    QString timeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");       //显示时间
    this->ui->timeLabel->setText(timeString);
    this->inTime=timeString;
    Mat car_number;
    int result=this->pr.plateRecognize(this->frame,this->plateVec);
    qDebug()<<"result="<<result;
    if(result==0)
    {
        this->plate=this->plateVec.at(0);                     //car info
        this->plateMat=this->plate.getPlateMat();             //car number info
        cvtColor(plateMat,car_number,CV_BGR2RGB);
        cvtColor(car_number,car_number,CV_BGR2RGB);

        QImage q_image=QImage(car_number.data,car_number.cols,car_number.rows,QImage::Format_RGB888);
        this->inImg=q_image.copy();
        //photo
        this->ui->carLabel->setPixmap(QPixmap::fromImage(q_image));
        this->ui->carLabel->setScaledContents(true);

        //num
        string str_car_num=this->plateVec[0].getPlateStr();
        QString q_str=QString::fromLocal8Bit(str_car_num.c_str());

        this->ui->carNumEdit->clear();
        this->ui->carNumEdit->setText(q_str);
    }
    this->plateVec.clear();
}

void MainWidget::showTime()
{
    QDateTime currentDateTime =QDateTime::currentDateTime();
    QString timeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");       //显示时间
    this->ui->currentTimeNum->setText(timeString);
}

void MainWidget::showMyself()
{
    this->setWidget->hide();
    this->show();
}

void MainWidget::showMyselfByLogin()
{
    //qDebug()<<"slot";
    this->loginWindget->hide();
    this->show();
}

SettingWidget *MainWidget::getSetWidget() const
{
    return setWidget;
}

void MainWidget::setSetWidget(SettingWidget *value)
{
    setWidget = value;
}

QString MainWidget::formatTimeDifference(const QString &timeStr1, const QString &timeStr2)
{
    // 解析时间字符串为 QDateTime
    QDateTime dt1 = QDateTime::fromString(timeStr1, "yyyy-MM-dd hh:mm:ss");
    QDateTime dt2 = QDateTime::fromString(timeStr2, "yyyy-MM-dd hh:mm:ss");
   if (!dt1.isValid() || !dt2.isValid()) {
       return "Invalid time format";
   }
   // 计算时间差（毫秒）
   qint64 diffMs = dt1.msecsTo(dt2);
   if (diffMs < 0) {
       diffMs = -diffMs; // 取绝对值
   }
   // 转换为秒
   qint64 diffSec = diffMs / 1000;
   // 计算小时、分钟、秒
   qint64 hours = diffSec / 3600;
   qint64 minutes = (diffSec % 3600) / 60;
   qint64 seconds = diffSec % 60;
   // 格式化输出
   return QString("%1小时%2分钟%3秒").arg(hours).arg(minutes).arg(seconds);
}


int MainWidget::checkMinutesAndSeconds(const QString &timeStr)
{
    QStringList parts = timeStr.split("小时");
   if (parts.size() < 2) return 0; // 格式错误，默认返回 0

   QString remaining = parts[1]; // "52分钟13秒"
   parts = remaining.split("分钟");
   if (parts.size() < 2) return 0;

   // 提取分钟（"52"）和秒（"13秒" -> 去掉"秒"）
   bool ok;
   int minutes = parts[0].toInt(&ok);
   if (!ok) return 0;

   QString secondsStr = parts[1].split("秒")[0]; // "13"
   int seconds = secondsStr.toInt(&ok);
   if (!ok) return 0;

   // 判断分钟或秒是否 > 0
   return (minutes > 0 || seconds > 0) ? 1 : 0;
}

void MainWidget::updateTable(int flag,char *info)
{
    if(this->currentIndex==0)
    {
        if(flag==1)
        {
            QMessageBox::information(this,"停车场入场界面","入场成功",QMessageBox::Ok,QMessageBox::Ok);
            //将内容插入到表格中
            this->tableIndex++;
            QString carnum=this->ui->carNumEdit->text().split(":").last();
            QString time=this->inTime;
            int row = this->ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(row);
            // 设置单元格内容
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(tableIndex)));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(carnum));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(time));
            ui->tableWidget->setItem(row, 3, new QTableWidgetItem("西北门"));
        }
    }
   else if(this->currentIndex==1)
    {
        if(flag==1)
        {
            QMessageBox::information(this,"停车场出场界面","出场成功",QMessageBox::Ok,QMessageBox::Ok);
        }
        if(flag==2)
        {
            QMessageBox::information(this,"停车场出场界面","没有查询到该车入场",QMessageBox::Ok,QMessageBox::Ok);
        }
    }
}



void MainWidget::receiveFrame(Mat frame)
{
    this->frame=frame.clone();
    cvtColor(this->frame, this->frame, CV_BGR2RGB);
    if(this->currentIndex==0)
    {
        this->img = QImage(this->frame.data,this->frame.cols,this->frame.rows,QImage::Format_RGB888);
        this->img = this->img.scaled(this->ui->carPhoto->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    }
    else if(this->currentIndex==1)
    {
        this->img2 = QImage(this->frame.data,this->frame.cols,this->frame.rows,QImage::Format_RGB888);
        this->img2 = this->img2.scaled(this->ui->photoLabel2->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    }
    update();
}

void MainWidget::showLogin()
{
    this->setWidget->hide();
    this->loginWindget->show();
}

void MainWidget::paintEvent(QPaintEvent *event)
{
    if(this->currentIndex==0)
    {
        if(!this->img.isNull())
        {
            this->ui->carPhoto->setPixmap(QPixmap::fromImage(this->img));
        }
    }
    else if(this->currentIndex==1)
    {
        if(!this->img2.isNull())
        {
            this->ui->photoLabel2->setPixmap(QPixmap::fromImage(this->img2));
        }
    }
}



void MainWidget::on_recognizeCarBtn2_clicked()
{
    Mat car_number;
    int result=this->pr.plateRecognize(this->frame,this->plateVec);
    qDebug()<<"result="<<result;
    if(result==0)
    {
        this->plate=this->plateVec.at(0);                     //car info
        this->plateMat=this->plate.getPlateMat();             //car number info
        cvtColor(plateMat,car_number,CV_BGR2RGB);
        cvtColor(car_number,car_number,CV_BGR2RGB);

        QImage q_image=QImage(car_number.data,car_number.cols,car_number.rows,QImage::Format_RGB888);
        this->outImg=q_image.copy();
        //photo
        this->ui->carLabel_2->setPixmap(QPixmap::fromImage(q_image));
        this->ui->carLabel_2->setScaledContents(true);

        //num
        string str_car_num=this->plateVec[0].getPlateStr();
        QString q_str=QString::fromLocal8Bit(str_car_num.c_str());

        this->ui->carNum2Edit->clear();
        this->ui->carNum2Edit->setText(q_str);
    }
    this->plateVec.clear();
}


void MainWidget::showSettingByParking()
{
    this->camerathread->stop();
    this->parkingWidget->hide();
    this->setWidget->show();
}

void MainWidget::receiveFileInfo(int flag, char *info)
{
    if(this->parkingWidget->getIsclicked()==false)
    {
        if(this->currentIndex==0)
        {
            if(flag==1)
            {
                QMessageBox::information(this,"停车场入场界面","图片上传成功",QMessageBox::Ok,QMessageBox::Ok);
                //发送入场记录给服务器
                this->inoutRecord.cost=0;
                this->inoutRecord.type=1;
                //发送上去的车牌是不带蓝牌俩个字的
                QString carnum=this->ui->carNumEdit->text().split(":").last();
                memcpy(this->inoutRecord.number,carnum.toStdString().c_str(),sizeof(this->inoutRecord.number));
                qDebug()<<"this->inoutRecord.number"<<this->inoutRecord.number;
                memcpy(this->inoutRecord.time,this->ui->timeLabel->text().toStdString().c_str(),sizeof(this->inoutRecord.time));
                memcpy(this->inoutRecord.locate,"西北方",sizeof(this->inoutRecord.locate));
                memcpy(this->inoutRecord.filename,this->photoName.toStdString().c_str(),sizeof(this->inoutRecord.filename));

                this->head.bussinessType=INRECORD_T;
                this->head.bussinessLength=sizeof(INOUTRECORD);

                memcpy(buf,&head,sizeof(HEAD));
                memcpy(buf+sizeof(HEAD),&this->inoutRecord,sizeof(INOUTRECORD));

                ((HEAD*)buf)->crc=0;
                unsigned int crc = CTool::crc32(0xFFFFFFFF, (unsigned char*)buf, sizeof(HEAD) + sizeof(INOUTRECORD));
                ((HEAD*)buf)->crc = htonl(crc);
                this->wd->setData(this->buf,sizeof(this->head)+sizeof(this->inoutRecord));
           }
        }
        else if(this->currentIndex==1)
        {
            if(flag==1)
            {
                QMessageBox::information(this,"停车场出场界面","图片上传成功",QMessageBox::Ok,QMessageBox::Ok);
           }
        }
    }
}

//出库图片的上传
void MainWidget::on_uploadPhotoBtn_clicked()
{
    if (this->outImg.isNull()) {
           qDebug() << "错误：outImg 是空的！";
           return;
       }
    QString file=this->setWidget->getPhotoPath()+"/"+this->outTime+this->ui->carNum2Edit->text().split(":").last()+".jpg";
    bool success = this->outImg.save(file);
    if(!success)
    {
        qDebug()<<"photo save fail";
        return;
    }
    this->photoName=file.split("/").last();
    this->filethread->addFile(file);
}


void MainWidget::showOuterInfo(int flag, char *info)
{
    //出场时间，入场时间，停车时间，收费的填写
    if(flag==1)
    {
        QDateTime currentDateTime =QDateTime::currentDateTime();
        QString timeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");       //显示时间
        this->outTime=timeString;
        //出场时间
        this->ui->timeLabel2->setText(timeString);
        //入场时间
        this->ui->timeLabel3->setText(info);
        //停车时长
        this->ui->parkingTimeLabel->setText(formatTimeDifference(timeString,info));
        //计算费用
        if(this->ui->monthBtn->text()=="√")
        {
            this->ui->costLabel->setText("0元");
        }
        else if(this->ui->monthBtn->text()=="⚪")
        {
            int cost=0;
            QString hourStr = this->ui->parkingTimeLabel->text().split("小时").first();
            int hours = hourStr.toInt(); // 停车小时数
            // 检查分钟或秒是否 > 0（调用 checkMinutesAndSeconds）
            int hasExtraTime = checkMinutesAndSeconds(this->ui->parkingTimeLabel->text());
            // 计算费用
            if(hasExtraTime)
            {
                cost=(hours+1)*5;
            }
            else{
                cost=hours*5;
            }
            this->ui->costLabel->setText(QString("%1").arg(cost));
        }
    }
    else if(flag==2)
    {
        QMessageBox::information(this,"停车场出场界面","未查询到该车辆的入库信息",QMessageBox::Ok,QMessageBox::Ok);
    }
}



void MainWidget::on_canGoBtn_clicked()
{
    //将出库的记录结构体发送给服务器
    bzero(&this->inoutRecord,sizeof(this->inoutRecord));
    this->inoutRecord.cost=this->ui->costLabel->text().split("元").first().toInt();
    this->inoutRecord.type=2;
    QString carnum=this->ui->carNum2Edit->text().split(":").last();
    memcpy(this->inoutRecord.number,carnum.toStdString().c_str(),sizeof(this->inoutRecord.number));
    qDebug()<<"this->inoutRecord.number"<<this->inoutRecord.number;
    memcpy(this->inoutRecord.time,this->ui->timeLabel2->text().toStdString().c_str(),sizeof(this->inoutRecord.time));
    memcpy(this->inoutRecord.locate,"西北方",sizeof(this->inoutRecord.locate));
    memcpy(this->inoutRecord.filename,this->photoName.toStdString().c_str(),sizeof(this->inoutRecord.filename));

    this->head.bussinessType=INRECORD_T;
    this->head.bussinessLength=sizeof(INOUTRECORD);

    memcpy(buf,&head,sizeof(HEAD));
    memcpy(buf+sizeof(HEAD),&this->inoutRecord,sizeof(INOUTRECORD));

    ((HEAD*)buf)->crc=0;
    unsigned int crc = CTool::crc32(0xFFFFFFFF, (unsigned char*)buf, sizeof(HEAD) + sizeof(INOUTRECORD));
    ((HEAD*)buf)->crc = htonl(crc);
    this->wd->setData(this->buf,sizeof(this->head)+sizeof(this->inoutRecord));
}



void MainWidget::on_monthBtn_clicked()
{
    if(this->ui->monthBtn->text()=="⚪")
    {
        this->ui->monthBtn->setText("√");
    }
    else if(this->ui->monthBtn->text()=="√")
    {
        this->ui->monthBtn->setText("⚪");
    }
}

void MainWidget::isCanGo(int flag, char *info)
{
    if(flag==1)
    {
        QMessageBox::information(this,"停车场出场界面","放行成功",QMessageBox::Ok,QMessageBox::Ok);
        qDebug()<<info;
    }
    else{
        QMessageBox::information(this,"停车场出场界面","放行失败",QMessageBox::Ok,QMessageBox::Ok);
    }
}

void MainWidget::on_modifyCarBtn_clicked()
{
    //上传车牌号码,利用出入场结构体
    bzero(&this->inoutRecord,sizeof(this->inoutRecord));
    this->inoutRecord.type=2;
    QString carnum=this->ui->carNum2Edit->text().split(":").last();
    memcpy(this->inoutRecord.number,carnum.toStdString().c_str(),sizeof(this->inoutRecord.number));
    qDebug()<<"this->inoutRecord.number"<<this->inoutRecord.number;
    this->head.bussinessType=INRECORD_T;
    this->head.bussinessLength=sizeof(INOUTRECORD);

    memcpy(buf,&head,sizeof(HEAD));
    memcpy(buf+sizeof(HEAD),&this->inoutRecord,sizeof(INOUTRECORD));

    ((HEAD*)buf)->crc=0;
    unsigned int crc = CTool::crc32(0xFFFFFFFF, (unsigned char*)buf, sizeof(HEAD) + sizeof(INOUTRECORD));
    ((HEAD*)buf)->crc = htonl(crc);
    this->wd->setData(this->buf,sizeof(this->head)+sizeof(this->inoutRecord));
}

//入库图片上传
void MainWidget::on_modifyBtn1_clicked()
{
    //保存的图片路径也不带蓝牌两个字
    QString file=this->setWidget->getPhotoPath()+"/"+this->inTime+this->ui->carNumEdit->text().split(":").last()+".jpg";
    bool success = this->inImg.save(file);
    if(!success)
    {
        qDebug()<<"photo save fail";
        return;
    }
    this->photoName=file.split("/").last();
    this->filethread->addFile(file);
}

void MainWidget::showmyselfbyCarinfoWin()
{
    this->camerathread->changeWindow(MAINWIDGET);
    this->carinfoCheckWin->hide();
    this->show();
}


void MainWidget::showTableinit(int num, const QVector<UPDATATABLE> &temptable)
{
    if(num==0)
    {
        qDebug()<<"无入场记录";
        return;
    }
    else{
        for(int i=0;i<num;i++)
        {
            //将内容插入到表格中，车牌不带蓝牌两个字
            this->tableIndex++;
            int row = this->ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(row);
            // 设置单元格内容
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(tableIndex)));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(temptable[i].plate));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(temptable[i].time));
            ui->tableWidget->setItem(row, 3, new QTableWidgetItem(temptable[i].locate));
        }
    }
}

void MainWidget::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    if(!index.isValid())
    {
        return;
    }
    int row=index.row();
    QTableWidgetItem *item1=ui->tableWidget->item(row,1);
    QTableWidgetItem *item2=ui->tableWidget->item(row,2);
    QTableWidgetItem *item3=ui->tableWidget->item(row,3);

    if(item1 && item2 && item3)
    {
        QString carNum=item1->text();
        QString timeStr=item2->text();
        QString place=item3->text();

        this->ui->carNumEdit->clear();
        this->ui->carNumEdit->setText(carNum);

        QString picture_path=this->setWidget->getPhotoPath()+"/"+timeStr+carNum+".jpg";

        this->ui->carLabel->setPixmap(QPixmap(picture_path));
        this->ui->carLabel->setScaledContents(true);

        this->ui->timeLabel->clear();
        this->ui->timeLabel->setText(timeStr);
    }
    else{
        this->ui->carLabel->clear();
        this->ui->carNumEdit->clear();
        this->ui->timeLabel->clear();
    }
}
