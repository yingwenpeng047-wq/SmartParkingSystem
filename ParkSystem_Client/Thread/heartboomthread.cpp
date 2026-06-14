#include "heartboomthread.h"

HeartBoomThread *HeartBoomThread::heartboomThread=nullptr;

HeartBoomThread *HeartBoomThread::getInstance()
{
    if(HeartBoomThread::heartboomThread==nullptr)
    {
        HeartBoomThread::heartboomThread=new HeartBoomThread();
    }
    return HeartBoomThread::heartboomThread;
}


HeartBoomThread::HeartBoomThread()
{
    this->isRun=false;
    this->isStop=false;
    this->wd=WriteThread::getInstance();

    bzero(&this->head,sizeof(this->head));
    bzero(this->buf,sizeof(this->buf));
}

void HeartBoomThread::setSocketfd(int value)
{
    socketfd = value;
}

void HeartBoomThread::setIsStop(bool value)
{
    isStop = value;
}

void HeartBoomThread::activate()
{
    this->isRun=true;
}

void HeartBoomThread::setIsRun(bool value)
{
    isRun = value;
}



HeartBoomThread::~HeartBoomThread()
{

}


void HeartBoomThread::run()
{
    while(this->isStop==false)
    {
        while(this->isRun==true)
        {
            //发送一个头出去   
            bzero(&this->head,sizeof(HEAD));
            bzero(this->buf,sizeof(this->buf));
            this->head.bussinessType=LIFE_T;
            this->head.bussinessLength=0;
            memcpy(this->buf,&this->head,sizeof(HEAD));

            ((HEAD*)buf)->crc=0;
            unsigned int crc = CTool::crc32(0xFFFFFFFF, (unsigned char*)buf, sizeof(HEAD));
            ((HEAD*)buf)->crc = htonl(crc);
            int res=write(this->socketfd,this->buf,sizeof(HEAD));
            if(res>0)
            {
                qDebug()<<"心跳头发送成功";
            }
            this->isRun=false;
        }
    }
}
