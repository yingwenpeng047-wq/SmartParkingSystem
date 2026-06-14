#include "writethread.h"

WriteThread *WriteThread::writeThread=nullptr;

WriteThread *WriteThread::getInstance()
{
    if(WriteThread::writeThread==nullptr)
    {
        WriteThread::writeThread=new WriteThread();
    }
    return WriteThread::writeThread;
}


WriteThread::WriteThread()
{
    this->length=0;
    memset(this->data,0,sizeof(data));
    this->isStart=false;
}

bool WriteThread::getIsStart() const
{
    return isStart;
}

void WriteThread::setIsStart(bool value)
{
    isStart = value;
}



void WriteThread::run()
{
    while(1)
    {
        if(this->length>0)
        {
            int res=write(this->socketfd,this->data,this->length);
            qDebug()<<"send res="<<res;
            bzero(this->data,sizeof(this->data));
            this->length=0;
        }
    }
}



void WriteThread::setData(void *value, int size)
{
    this->length=size;
    memcpy(this->data,value,size);
}

void WriteThread::setSocketfd(int value)
{
    this->socketfd=value;
}



