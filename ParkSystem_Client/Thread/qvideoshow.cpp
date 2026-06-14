#include "Thread/qvideoshow.h"

QVideoShow *QVideoShow::videoShowThread=nullptr;


QVideoShow *QVideoShow::getInstance()
{
    if(QVideoShow::videoShowThread==nullptr)
    {
        QVideoShow::videoShowThread=new QVideoShow();
    }
    return QVideoShow::videoShowThread;
}

QVideoShow::QVideoShow():QThread()
{
    this->filethread=CFileSplit::getInstance();
    this->wd=WriteThread::getInstance();
    this->isRun=false;
    this->isStop=false;
    this->rate=1;
    this->whichwidget=OPENWIDGET;
    this->currentframe=0;
    bzero(this->buf,sizeof(this->buf));
    bzero(&this->head,sizeof(HEAD));
    bzero(&this->videoInfo,sizeof(VIDEO));
}




int QVideoShow::getCurrentframeVideoShow() const
{
    return this->currentframe;
}

void QVideoShow::setCurrentframeVideoShow(int value)
{
    this->currentframe = value;
    cap.set(CAP_PROP_POS_FRAMES,this->currentframe);
}


double QVideoShow::getRate() const
{
    return rate;
}

void QVideoShow::setRate(double value)
{
    rate = value;
}



int QVideoShow::getTotalframe() const
{
    return totalframe;
}

void QVideoShow::setTotalframe(int value)
{
    totalframe = value;
}

int QVideoShow::getWhichwidget() const
{
    return whichwidget;
}

void QVideoShow::setWhichwidget(int value)
{
    whichwidget = value;
}

void QVideoShow::setCurrent(int value)
{
    this->currentframe=value;
}


QVideoShow::~QVideoShow()
{
    this->cap.release();
    this->isStop = true; // 确保线程退出
    wait();              // 等待线程结束
}


bool QVideoShow::getIsStop() const
{
    return isStop;
}

void QVideoShow::setIsStop(bool value)
{
    isStop = value;
}

void QVideoShow::activate()
{
    this->isRun=true;
}

void QVideoShow::stop()
{
    this->isRun=false;
}

void QVideoShow::changeVideo(const string &fileName,int index)
{
    qDebug()<<"切换视频";
    this->stop();
    if(this->cap.isOpened())
    {
        this->cap.release();
    }
    this->whichwidget=index;
    this->cap.open(fileName);
    qDebug()<<QString::fromStdString(fileName);
    if(!this->cap.isOpened())
    {
        qDebug() << "Error: Failed to open video file:" << QString::fromStdString(fileName);
        return;
    }
    this->currentframe=0;
    double totalFrames = cap.get(CAP_PROP_FRAME_COUNT);
    this->totalframe=static_cast<int>(totalFrames);
    this->activate();
}

bool QVideoShow::getIsRun() const
{
    return isRun;
}

void QVideoShow::setIsRun(bool value)
{
    isRun = value;
}

VideoCapture &QVideoShow::getCap()
{
    return this->cap;
}


void QVideoShow::run()
{
    while(this->isStop==false)
    {
        while(this->isRun==true)
        {
            if(this->cap.read(this->frame))
            {
                this->currentframe++;
                if(this->whichwidget==OPENWIDGET)
                {
                    emit this->send2OpenWidget(this->frame);
                    msleep(40);
                    int total=(int)round(this->cap.get(CAP_PROP_FRAME_COUNT));
                    if(this->currentframe==total-1)
                    {
                        emit this->change2NextWidegt();
                    }
                }
               else if(this->whichwidget==VIDEOPLAYERWIDGET)
               {
                    Mat clone=this->frame.clone();
                    emit this->send2VideoPlayerWidget(clone,this->currentframe);
                    double delay =40/(this->rate);
                    msleep((unsigned long)delay);
                    if(this->currentframe==this->totalframe-1)
                    {
                        this->cap.set(CAP_PROP_POS_FRAMES,0);
                        this->currentframe=0;
                    }
               }
            }
        }
    }
}

