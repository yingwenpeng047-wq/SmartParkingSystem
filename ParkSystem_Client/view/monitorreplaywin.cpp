#include "monitorreplaywin.h"
#include "ui_monitorreplaywin.h"

MonitorReplayWin::MonitorReplayWin(QWidget *parent):QWidget(parent),ui(new Ui::MonitorReplayWin)
{
    ui->setupUi(this);
    this->videoplay=nullptr;
    this->currentPage=1;
    this->maxPage=1;
    this->isnext=false;
    this->rd=ReadThread::getInstance();
    this->wd=WriteThread::getInstance();
    this->init_connect();
    bzero(&this->head,sizeof(HEAD));
    bzero(this->buf,sizeof(this->buf));
}

MonitorReplayWin::~MonitorReplayWin()
{
    if(videoplay)
    {
        delete videoplay;
    }
    disconnect();  // 断开所有信号槽
    delete ui;
}

void MonitorReplayWin::init_connect()
{
    qRegisterMetaType<VIDEO>("VIDEO");
    qRegisterMetaType<QVector<VIDEO>>("QVector<VIDEO>");
    connect(this->rd,&ReadThread::send2MonitorWin,this,&MonitorReplayWin::showDay,Qt::BlockingQueuedConnection);
    connect(this->rd,&ReadThread::send2MonitorWin2,this,&MonitorReplayWin::showMonth,Qt::BlockingQueuedConnection);
    connect(this->ui->listWidget,&QListWidget::itemClicked, this, &MonitorReplayWin::onItemClicked);
    connect(this->ui->listWidget_2,&QListWidget::itemDoubleClicked,this,&MonitorReplayWin::videobroadcast);
    connect(this->rd,&ReadThread::send2MonitorWin3,this,&MonitorReplayWin::showVideo,Qt::QueuedConnection);
    connect(this->rd,&ReadThread::send2MonitorWin4,this,&MonitorReplayWin::updateVideoInfo);
}


//点击按月显示
void MonitorReplayWin::on_monthBtn_clicked()
{
    //只有是没选择这个按月显示的情况下才做出反应
    if(this->ui->monthBtn->text()=="⚪")
    {
        this->totalVec.clear();
        this->currentPage=1;
        this->maxPage=1;
        this->ui->monthBtn->setText("√");
        if(this->ui->dayBtn->text()=="√")
        {
            this->ui->dayBtn->setText("⚪");
        }
        bzero(this->buf,sizeof(this->buf));
        bzero(&this->head,sizeof(HEAD));
        //将请求发送上去，只发送头
        this->head.bussinessType=VIDEOMONTH_T;
        this->head.bussinessLength=0;
        memcpy(buf,&head,sizeof(HEAD));
        ((HEAD*)buf)->crc=0;
        unsigned int crc = CTool::crc32(0xFFFFFFFF, (unsigned char*)buf, sizeof(HEAD));
        ((HEAD*)buf)->crc = htonl(crc);
        this->wd->setData(this->buf,sizeof(this->head));
    }
}

//点击完按月显示
void MonitorReplayWin::showMonth(int number,QStringList list)
{
    qDebug()<<"month";
    //往左边的qlistwidget里面加item
    //每次显示之前都先清空所有的item项，然后再添加
    this->ui->listWidget->clear();
    for(int i=0;i<number;i++)
    {
        qDebug()<<list[i];
        QListWidgetItem *item = new QListWidgetItem(list[i]);
        this->ui->listWidget->addItem(item);
    }
}

//点击按天显示
void MonitorReplayWin::on_dayBtn_clicked()
{
    //只有是没选择这个按天显示的情况下才做出反应
    if(this->ui->dayBtn->text()=="⚪")
    {
        this->totalVec.clear();
        this->currentPage=1;
        this->maxPage=1;
        this->ui->dayBtn->setText("√");
        if(this->ui->monthBtn->text()=="√")
        {
            this->ui->monthBtn->setText("⚪");
        }
        //将请求发送上去，只发送头
        bzero(this->buf,sizeof(this->buf));
        bzero(&this->head,sizeof(HEAD));
        this->head.bussinessType=VIDEODAY_T;
        this->head.bussinessLength=0;
        memcpy(buf,&head,sizeof(HEAD));
        ((HEAD*)buf)->crc=0;
        unsigned int crc = CTool::crc32(0xFFFFFFFF, (unsigned char*)buf, sizeof(HEAD));
        ((HEAD*)buf)->crc = htonl(crc);
        this->wd->setData(this->buf,sizeof(this->head));
    }
}


//点击完按天显示
void MonitorReplayWin::showDay(int number,QStringList list)
{
    qDebug()<<"day";
    //往左边的qlistwidget里面加item
    //每次显示之前都先清空所有的item项，然后再添加
    this->ui->listWidget->clear();
    for(int i=0;i<number;i++)
    {
        qDebug()<<list[i];
        QListWidgetItem *item = new QListWidgetItem(list[i]);
        this->ui->listWidget->addItem(item);
    }
}

//点击左侧的时间
void MonitorReplayWin::onItemClicked(QListWidgetItem *item)
{
    //把这个item的内容发送给到服务器
    this->currentPage=1;
    this->maxPage=1; // 重置最大页数
    this->totalVec.clear(); // 清空之前的视频数据
    this->ui->listWidget_2->clear();

    bzero(&this->video,sizeof(this->video));
    bzero(this->buf,sizeof(this->buf));
    bzero(&this->head,sizeof(HEAD));

    this->currentTime=item->text();

    memcpy(video.date,item->text().toStdString().c_str(),sizeof(this->video.date));
    video.page=this->currentPage;

    this->head.bussinessType=VIDEOBACK_T;
    this->head.bussinessLength=sizeof(VIDEOPLAY);
    memcpy(buf,&head,sizeof(HEAD));
    memcpy(this->buf+sizeof(this->head),&this->video,sizeof(VIDEOPLAY));
    ((HEAD*)buf)->crc=0;
    unsigned int crc = CTool::crc32(0xFFFFFFFF, (unsigned char*)buf, sizeof(HEAD)+sizeof(VIDEOPLAY));
    ((HEAD*)buf)->crc = htonl(crc);
    this->wd->setData(this->buf,sizeof(this->head)+sizeof(this->video));
}


//点击完左侧时间，展示右边的视频列表
void MonitorReplayWin::showVideo(int num, const QVector<VIDEO> &tempvideo)
{
    QMutexLocker locker(&videoMutex);  // 加锁
    if(num==0)
    {
        if(this->isnext==true)
        {
            qDebug()<<"已经是最后一页数据啦";
            this->currentPage--;
            this->isnext=false;
        }
        return;
    }
    qDebug()<<"video list show";
    if(this->currentPage==1 && this->maxPage<=this->currentPage)
    {
        this->totalVec=tempvideo;
    }
    else if(this->currentPage > this->maxPage)
    {
        this->totalVec.append(tempvideo);
        this->maxPage = this->currentPage;
    }
    this->ui->listWidget_2->clear();
    int startIndex = (this->currentPage-1)*9;
    int endIndex = qMin(startIndex+9, this->totalVec.size());

    for(int i=startIndex; i<endIndex; i++)
    {
        if(this->maxPage==this->currentPage-1)
        {
            this->totalVec.push_back(tempvideo[i]);
        }
        int percent=qRound((double)this->totalVec[i].video_current / this->totalVec[i].video_frame * 100);
        QString jindu = QString("已经观看%1%").arg(percent);  // 修正百分比计算
        // 设置图标大小
        this->ui->listWidget_2->setIconSize(QSize(64*2/3, 64*2/3));
        // 设置文字样式和布局
        this->ui->listWidget_2->setStyleSheet(
            "QListWidget {"
            "    font: 12pt 'Arial';"
            "    color: #333333;"
            "}"
            "QListWidget::item {"
            "    margin: 5px;"
            "    padding: 5px;"
            "}"
        );

        QListWidgetItem *item = new QListWidgetItem(QIcon(this->totalVec[i].picture_path), jindu);
        item->setTextAlignment(Qt::AlignCenter);

        //每一个item都要塞一个自定义结构体进去，到时候点击的时候要去跳转播放
        QString video_name = QString::fromUtf8(this->totalVec[i].video_name);
        QString video_path = QString::fromUtf8(this->totalVec[i].video_path);
        QString pictrue_path = QString::fromUtf8(this->totalVec[i].picture_path);
        item->setData(Qt::UserRole,QVariant::fromValue(video_name));
        item->setData(Qt::UserRole+1,QVariant::fromValue(video_path));
        item->setData(Qt::UserRole+2,QVariant::fromValue(this->totalVec[i].video_current));
        item->setData(Qt::UserRole+3,QVariant::fromValue(this->totalVec[i].video_frame));
        item->setData(Qt::UserRole+4,QVariant::fromValue(pictrue_path));
        item->setData(Qt::UserRole+5,i); // 存储totalVec中的索引
        this->ui->listWidget_2->addItem(item);
    }
    this->ui->pageLabel->setText(QString("第%1页/共%2页").arg(this->currentPage).arg(this->maxPage));
}


//点击视频列表，播放视频
void MonitorReplayWin::videobroadcast(QListWidgetItem *item)
{
    if (!item) {
            qDebug() << "Item is null!";
            return;
        }
    //QMutexLocker locker(&videoMutex);  // 加锁
    qDebug()<<"video bo fang";
    this->videoName = QString(item->data(Qt::UserRole).toString()); // 深拷贝
    this->currentVideoPath = item->data(Qt::UserRole+1).toString();
    int indexInTotalVec = item->data(Qt::UserRole+5).toInt();
    // 创建新窗口
    if(this->videoplay==nullptr)
    {
        videoplay = new VideoPlayerWidget();
        connect(this->videoplay,&VideoPlayerWidget::updatevideo,this,&MonitorReplayWin::sendVideoInfo);
    }
    if(this->videoplay!=nullptr && this->videoplay->isVisible())
    {
        this->videoplay->close();
    }
    //设置播放路径
    this->videoplay->setVideoPath(this->currentVideoPath);
    //this->videoplay->setCurrentFrame(item->data(Qt::UserRole+2).toInt());
    this->videoplay->setCurrentFrame(this->totalVec[indexInTotalVec].video_current);
    this->videoplay->show();

}

void MonitorReplayWin::sendVideoInfo(int frame,QString jindu)
{
    //QMutexLocker locker(&videoMutex);  // 加锁
    this->currentindex=frame;
    qDebug()<<"this->currentIndex"<<this->currentindex;
    this->jindu=jindu;

    // 更新totalVec中的进度
    for(int i=0; i<this->totalVec.size(); i++)
    {
        if(QString::fromUtf8(this->totalVec[i].video_name) == this->videoName)
        {
            this->totalVec[i].video_current = this->currentindex;
            break;
        }
    }

    //发送新的视频信息给到服务器
    bzero(&this->newVideoInfo,sizeof(VIDEO));
    bzero(this->buf,sizeof(this->buf));

    this->newVideoInfo.video_current=this->currentindex;
    memcpy(this->newVideoInfo.video_name,this->videoName.toStdString().c_str(),sizeof(this->newVideoInfo.video_name));

    this->head.bussinessType=VIDEOUPDATE_T;
    this->head.bussinessLength=sizeof(VIDEO);


    memcpy(buf,&head,sizeof(HEAD));
    memcpy(this->buf+sizeof(this->head),&this->newVideoInfo,sizeof(VIDEO));
    ((HEAD*)buf)->crc=0;
    unsigned int crc = CTool::crc32(0xFFFFFFFF, (unsigned char*)buf, sizeof(HEAD)+sizeof(VIDEO));
    ((HEAD*)buf)->crc = htonl(crc);
    this->wd->setData(this->buf,sizeof(this->head)+sizeof(this->newVideoInfo));
}


void MonitorReplayWin::updateVideoInfo(int flag)
{
    QMutexLocker locker(&videoMutex);  // 加锁
    if (flag == 1)
    {
        if (this->totalVec.isEmpty()) {  // 明确判空
                   qDebug() << "totalVec is empty!";
                   return;
               }
           // 更新当前页的显示
           int startIndex = (this->currentPage-1)*9;
           int endIndex = qMin(startIndex+9, this->totalVec.size());

           for (int i = startIndex; i < endIndex; ++i)
           {
               if (QString::fromUtf8(this->totalVec[i].video_name) == this->videoName)
               {
                   int percent=qRound((double)this->totalVec[i].video_current / this->totalVec[i].video_frame * 100);
                   QString jindu = QString("已经观看%1%").arg(percent);

                   // 更新列表中的显示
                   for(int j=0; j<this->ui->listWidget_2->count(); j++)
                   {
                       QListWidgetItem *item = this->ui->listWidget_2->item(j);
                       if(item->data(Qt::UserRole).toString() == this->videoName)
                       {
                           item->setText(jindu);
                           item->setData(Qt::UserRole+2, this->currentindex);
                           break;
                       }
                   }
                   break;
               }
           }
    }
}


//上一页
void MonitorReplayWin::on_lastPageBtn_clicked()
{
    if(this->currentPage==1)
    {
        qDebug()<<"已经是第一页了";
        return;
    }
    this->currentPage--;
    //清空原有的数据
    this->ui->listWidget_2->clear();
    int j=0;
    for(int i=(this->currentPage-1)*9;i<this->currentPage*9;i++)
    {
        int percent=qRound((double)this->totalVec[i].video_current / this->totalVec[i].video_frame * 100);
        QString jindu = QString("已经观看%1%").arg(percent);  // 修正百分比计算
        this->ui->listWidget_2->setIconSize(QSize(64*2/3, 64*2/3));
        // 设置文字样式和布局
        this->ui->listWidget_2->setStyleSheet(
            "QListWidget {"
            "    font: 12pt 'Arial';"
            "    color: #333333;"
            "}"
            "QListWidget::item {"
            "    margin: 5px;"
            "    padding: 5px;"
            "}"
        );
        QListWidgetItem *item = new QListWidgetItem(QIcon(this->totalVec[i].picture_path), jindu);
        item->setTextAlignment(Qt::AlignCenter);
        //每一个item都要塞一个自定义结构体进去，到时候点击的时候要去跳转播放
        QString video_name = QString::fromUtf8(this->totalVec[i].video_name);
        QString video_path = QString::fromUtf8(this->totalVec[i].video_path);
        QString pictrue_path = QString::fromUtf8(this->totalVec[i].picture_path);
        item->setData(Qt::UserRole,QVariant::fromValue(video_name));
        item->setData(Qt::UserRole+1,QVariant::fromValue(video_path));
        item->setData(Qt::UserRole+2,QVariant::fromValue(this->totalVec[i].video_current));
        item->setData(Qt::UserRole+3,QVariant::fromValue(this->totalVec[i].video_frame));
        item->setData(Qt::UserRole+4,QVariant::fromValue(pictrue_path));
        this->ui->listWidget_2->addItem(item);
    }
}



void MonitorReplayWin::on_nextPageBtn_clicked()
{
     this->isnext=true;
     this->currentPage++;

     bzero(&this->video,sizeof(this->video));
     bzero(this->buf,sizeof(this->buf));
     bzero(&this->head,sizeof(HEAD));

     memcpy(video.date,this->currentTime.toStdString().c_str(),sizeof(this->video.date));
     video.page=this->currentPage;

     this->head.bussinessType=VIDEOBACK_T;
     this->head.bussinessLength=sizeof(VIDEOPLAY);
     memcpy(buf,&head,sizeof(HEAD));
     memcpy(this->buf+sizeof(this->head),&this->video,sizeof(VIDEOPLAY));
     ((HEAD*)buf)->crc=0;
     unsigned int crc = CTool::crc32(0xFFFFFFFF, (unsigned char*)buf, sizeof(HEAD)+sizeof(VIDEOPLAY));
     ((HEAD*)buf)->crc = htonl(crc);
     this->wd->setData(this->buf,sizeof(this->head)+sizeof(this->video));
}


void MonitorReplayWin::on_pushButton_clicked()
{
    emit this->backParkingWin();
}
