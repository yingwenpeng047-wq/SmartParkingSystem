#include "camerathread.h"

CameraThread *CameraThread::videoShowThread=nullptr;


CameraThread *CameraThread::getInstance()
{
    if(CameraThread::videoShowThread==nullptr)
    {
        CameraThread::videoShowThread=new CameraThread();
    }
    return CameraThread::videoShowThread;
}

CameraThread::CameraThread():QThread()
{
    this->filethread=CFileSplit::getInstance();
    this->wd=WriteThread::getInstance();
    this->isRun=false;
    this->isStop=false;
    this->isRecording=false;
    this->isautoRecording=false;
    this->recordNum=0;
    this->whichwidget=MAINWIDGET;
    this->fps=25;
    this->cap.open(0);

    bzero(this->buf,sizeof(this->buf));
    bzero(&this->head,sizeof(HEAD));
    bzero(&this->videoInfo,sizeof(VIDEO));
    if (!car_cascade.load("../xml/cars.xml"))
    {
        cout<<"cars_cascade.load error"<<endl;
    }
}

void CameraThread::setPhotoPath(const QString &value)
{
    photoPath = value;
}

bool CameraThread::getIsRecording() const
{
    return isRecording;
}

QString CameraThread::getVideoPath() const
{
    return videoPath;
}

void CameraThread::setVideoPath(const QString &value)
{
    videoPath = value;
}

bool CameraThread::getIsautoRecording() const
{
    return isautoRecording;
}

void CameraThread::setIsautoRecording(bool value)
{
    isautoRecording = value;
}


void CameraThread::setIsRecording(bool value)
{
    //QMutexLocker locker(&mutex);
    this->isRecording = value;
    if(this->isRecording==false && this->videoWriter.isOpened())
    {
        cvtColor(clone,clone,CV_BGR2RGB);
        QImage img=QImage(clone.data,clone.cols,clone.rows,QImage::Format_RGB888);
        bool success=img.save(this->photoPath + "/" + prefix + current_date+"_first.jpg");
        if(!success)
        {
            qDebug()<<"photo save fail";
            return;
        }
        //视频信息要通过写线程给上去
       //总帧数
       this->videoInfo.video_frame=this->recordNum;
       //上次视频播放的帧数
       this->videoInfo.video_current=0;        //???
       //视频名称
       QString videoname=prefix + current_date + ".avi";
       memcpy(this->videoInfo.video_name,videoname.toStdString().c_str(),sizeof(this->videoInfo.video_name));
       //视频路径
       memcpy(this->videoInfo.video_path,fileName.toStdString().c_str(),sizeof(this->videoInfo.video_path));
       //首帧路径
       QString fisrtphotoPath=this->photoPath + "/" + prefix + current_date+"_first.jpg";
       memcpy(this->videoInfo.picture_path,fisrtphotoPath.toStdString().c_str(),sizeof(this->videoInfo.picture_path));
       this->head.bussinessType=VIDEOPLAY_T;
       this->head.bussinessLength=sizeof(VIDEO);
       memcpy(buf,&head,sizeof(HEAD));
       memcpy(buf+sizeof(HEAD),&this->videoInfo,sizeof(VIDEO));

       ((HEAD*)buf)->crc=0;
       unsigned int crc = CTool::crc32(0xFFFFFFFF, (unsigned char*)buf, sizeof(HEAD) + sizeof(VIDEO));
       ((HEAD*)buf)->crc = htonl(crc);
       this->wd->setData(this->buf,sizeof(this->head)+sizeof(this->videoInfo));

        qDebug()<<"手动结束";
        this->videoWriter.release();
        this->recordNum=0;
    }
}


int CameraThread::getWhichwidget() const
{
    return whichwidget;
}

void CameraThread::setWhichwidget(int value)
{
    whichwidget = value;
}



CameraThread::~CameraThread()
{
    this->cap.release();
    this->isStop = true; // 确保线程退出
    wait();              // 等待线程结束
}


bool CameraThread::getIsStop() const
{
    return isStop;
}

void CameraThread::setIsStop(bool value)
{
    isStop = value;
}

void CameraThread::activate()
{
    this->isRun=true;
}

void CameraThread::stop()
{
    this->isRun=false;
}

void CameraThread::changeWindow(int index)
{
    this->stop();
    this->whichwidget=index;
    this->activate();
}

bool CameraThread::getIsRun() const
{
    return isRun;
}

void CameraThread::setIsRun(bool value)
{
    isRun = value;
}

VideoCapture &CameraThread::getCap()
{
    return this->cap;
}


void CameraThread::run()
{
    while(this->isStop==false)
    {
        if(this->isRun==true)
        {
            if(this->cap.read(this->frame))
            {
                if(this->whichwidget==MAINWIDGET)
                {
                    cvtColor(this->frame, gray, COLOR_BGR2GRAY) ;
                    equalizeHist(gray, gray);
                    //this->car_cascade.detectMultiScale(gray, faces,1.2,8,cv::CASCADE_DO_ROUGH_SEARCH,cv::Size(80,80));
                    this->car_cascade.detectMultiScale(gray, faces);
                    //绘制矩形框
                    for (size_t i = 0; i < faces.size(); i++)
                    {
                        rectangle(this->frame, faces[i], Scalar(255, 0, 0), 2);
                    }
                   //发送帧到 UI（非阻塞方式）
                   emit this->send2MainWidget(this->frame);
                }
                else if(this->whichwidget==PARKINGWINDGET)
                {
                    cvtColor(this->frame, gray, COLOR_BGR2GRAY) ;
                    equalizeHist(gray, gray);

                    this->car_cascade.detectMultiScale(gray, faces);
                    //绘制矩形框
                    for (size_t i = 0; i < faces.size(); i++)
                    {
                        rectangle(this->frame, faces[i], Scalar(255, 0, 0), 2);
                    }
                    if(this->isRecording || this->isautoRecording)
                    {
                        if(this->recordNum == 0)
                        {
                            QDateTime current_date_time = QDateTime::currentDateTime();
                            this->current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss");
                            this->prefix = this->isRecording ? "manual_" : "auto_";
                            this->fileName = this->videoPath + "/" + prefix + current_date + ".avi";

                            if(!this->videoWriter.open(fileName.toStdString(),VideoWriter::fourcc('M', 'J', 'P', 'G'),this->fps,Size(this->frame.cols, this->frame.rows)))
                            {
                                qDebug() << "录制路径失败";
                                this->isRecording = false;
                                this->isautoRecording = false;
                                return;
                            }
                            this->clone=this->frame.clone();
                       }
                       if(this->videoWriter.isOpened())
                       {
                           // 检查自动录制是否达到300帧
                           if(this->isautoRecording && this->recordNum >= 300)
                           {
                               cvtColor(clone,clone,CV_BGR2RGB);
                               QImage img=QImage(clone.data,clone.cols,clone.rows,QImage::Format_RGB888);
                               bool success=img.save(this->photoPath + "/" + prefix + current_date+"_first.jpg");
                               qDebug()<<this->photoPath + "/" + prefix + current_date+"_first.jpg";
                               if(!success)
                               {
                                   qDebug()<<"photo save fail";
                                   return;
                               }
                               //视频信息要通过写线程给上去
                              //总帧数
                              this->videoInfo.video_frame=this->recordNum;
                              //上次视频播放的帧数
                              this->videoInfo.video_current=0;        //???
                              //视频名称
                              QString videoname=prefix + current_date + ".avi";
                              memcpy(this->videoInfo.video_name,videoname.toStdString().c_str(),sizeof(this->videoInfo.video_name));
                              //视频路径
                              memcpy(this->videoInfo.video_path,fileName.toStdString().c_str(),sizeof(this->videoInfo.video_path));
                              //首帧路径
                              QString fisrtphotoPath=this->photoPath + "/" + prefix + current_date+"_first.jpg";
                              memcpy(this->videoInfo.picture_path,fisrtphotoPath.toStdString().c_str(),sizeof(this->videoInfo.picture_path));
                              this->head.bussinessType=VIDEOPLAY_T;
                              this->head.bussinessLength=sizeof(VIDEO);
                              memcpy(buf,&head,sizeof(HEAD));
                              memcpy(buf+sizeof(HEAD),&this->videoInfo,sizeof(VIDEO));

                              ((HEAD*)buf)->crc=0;
                              unsigned int crc = CTool::crc32(0xFFFFFFFF, (unsigned char*)buf, sizeof(HEAD) + sizeof(VIDEO));
                              ((HEAD*)buf)->crc = htonl(crc);
                              this->wd->setData(this->buf,sizeof(this->head)+sizeof(this->videoInfo));
                               qDebug() << "自动录制满300帧";
                               this->videoWriter.release();
                               this->recordNum = 0;
                               this->isautoRecording = false;
                           }
                           //手动录制没结束或者自动录制没满
                           else
                           {
                               qDebug()<<"录制中,已录制帧数:"<<++this->recordNum;
                               this->videoWriter<<frame;
                           }
                       }
                   }
                   // 发送帧到 UI（非阻塞方式）
                   emit this->send2ParkingWidget(this->frame);
                }
            }
        }
    }
}


