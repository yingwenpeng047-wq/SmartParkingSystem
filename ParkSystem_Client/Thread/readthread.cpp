#include "readthread.h"


ReadThread *ReadThread::readThread=nullptr;


ReadThread *ReadThread::getInstance()
{
    if(ReadThread::readThread==nullptr)
    {
        ReadThread::readThread=new ReadThread();
    }
    return ReadThread::readThread;
}


ReadThread::ReadThread()
{
    this->isStart=false;
}

bool ReadThread::getIsStart() const
{
    return isStart;
}

void ReadThread::setIsStart(bool value)
{
    isStart = value;
}


ReadThread::~ReadThread()
{

}


void ReadThread::setSocketfd(int value)
{
    this->socketfd=value;
}



int ReadThread::getSocketfd() const
{
    return socketfd;
}

//定长包头+不定长包体，包头里面有请求体类型，请求体长度，还有个CRC校验码，先读取头，就知道了体的类型和体的长度，就知道后续要读取多大的字节长度，还有CRC校验码辅佐验证数据完整性
void ReadThread::run()
{
    int res=0;
    while(1)
    {
        res=read(this->socketfd,this->buf,sizeof(HEAD));
        memcpy(&this->head,this->buf,sizeof(HEAD));
        qDebug()<<"this->head.bussinessType="<<this->head.bussinessType;
        bzero(this->buf,sizeof(this->buf));
        if(this->head.bussinessType==USER_T)
        {
            //CRC校验
            unsigned int recv_crc=ntohl(this->head.crc);
            qDebug()<<"recv_crc= "<<recv_crc;

            res=read(this->socketfd,this->buf,sizeof(RESULTBACK));
            qDebug()<<"readRes="<<res;
            unsigned char fullBuf[sizeof(HEAD)+head.bussinessLength];
            memcpy(fullBuf,&head,sizeof(HEAD));
            memcpy(fullBuf+sizeof(HEAD),buf,head.bussinessLength);
            auto pcrc=reinterpret_cast<std::uint32_t*>(fullBuf+offsetof(HEAD,crc));
            *pcrc=0;
            unsigned int calc_crc=CTool::crc32(0xFFFFFFFF,fullBuf,sizeof(HEAD)+head.bussinessLength);
            if(calc_crc!=recv_crc)
            {
                qDebug()<<"CRC错误，丢弃注册包";
                continue;
            }
            else{
                qDebug()<<"CRC匹配成功";
                memcpy(&this->resultback,this->buf,sizeof(RESULTBACK));
                qDebug()<<"登录flag="<<resultback.flag;
                emit this->send2LoginWin(resultback.flag,resultback.message);
            }
        }
        else if(this->head.bussinessType==REGISTER_T)
        {
            //CRC校验
            unsigned int recv_crc=ntohl(this->head.crc);
            qDebug()<<"recv_crc= "<<recv_crc;

            res=read(this->socketfd,this->buf,sizeof(RESULTBACK));
            qDebug()<<"readRes="<<res;
            unsigned char fullBuf[sizeof(HEAD)+head.bussinessLength];
            memcpy(fullBuf,&head,sizeof(HEAD));
            memcpy(fullBuf+sizeof(HEAD),buf,head.bussinessLength);
            auto pcrc=reinterpret_cast<std::uint32_t*>(fullBuf+offsetof(HEAD,crc));
            *pcrc=0;
            unsigned int calc_crc=CTool::crc32(0xFFFFFFFF,fullBuf,sizeof(HEAD)+head.bussinessLength);
            if(calc_crc!=recv_crc)
            {
                qDebug()<<"CRC错误，丢弃注册包";
                continue;
            }
            else{
                qDebug()<<"CRC匹配成功";
                memcpy(&this->resultback,this->buf,sizeof(RESULTBACK));
                qDebug()<<"注册flag="<<resultback.flag;
                qDebug()<<"注册code="<<resultback.message;
                emit this->Send2ReginWin(resultback.flag,resultback.message);
            }
        }
        //发完尾包，若是落包没发出去，就返回这个
        else if(this->head.bussinessType==FILEBACK_T)
        {
            //CRC校验
            unsigned int recv_crc=ntohl(this->head.crc);
            qDebug()<<"recv_crc= "<<recv_crc;

            res=read(this->socketfd,this->buf,sizeof(FILEBACK));
            qDebug()<<"readRes="<<res;
            unsigned char fullBuf[sizeof(HEAD)+head.bussinessLength];
            memcpy(fullBuf,&head,sizeof(HEAD));
            memcpy(fullBuf+sizeof(HEAD),buf,head.bussinessLength);
            auto pcrc=reinterpret_cast<std::uint32_t*>(fullBuf+offsetof(HEAD,crc));
            *pcrc=0;
            unsigned int calc_crc=CTool::crc32(0xFFFFFFFF,fullBuf,sizeof(HEAD)+head.bussinessLength);
            if(calc_crc!=recv_crc)
            {
                qDebug()<<"CRC错误，丢弃文件返回包";
                continue;
            }
            else{
                int non_zero_count = 0;
                qDebug()<<"CRC匹配成功";
                memcpy(&this->fileback,this->buf,sizeof(FILEBACK));
                for(int i=0;i<sizeof(this->fileback.arr);i++)
                {
                   if (this->fileback.arr[i] != 0)
                   {
                        non_zero_count++;
                   }
                }
                qDebug()<<"丢包non_zero_count="<<non_zero_count;
            }
        }
        //包发送成功,不管是在停车界面还是出入场界面截的图
        else if(this->head.bussinessType==FILECHECK_T)
        {
            //CRC校验
            unsigned int recv_crc=ntohl(this->head.crc);
            qDebug()<<"recv_crc= "<<recv_crc;

            res=read(this->socketfd,this->buf,sizeof(RESULTBACK));
            qDebug()<<"readRes="<<res;
            unsigned char fullBuf[sizeof(HEAD)+head.bussinessLength];
            memcpy(fullBuf,&head,sizeof(HEAD));
            memcpy(fullBuf+sizeof(HEAD),buf,head.bussinessLength);
            auto pcrc=reinterpret_cast<std::uint32_t*>(fullBuf+offsetof(HEAD,crc));
            *pcrc=0;
            unsigned int calc_crc=CTool::crc32(0xFFFFFFFF,fullBuf,sizeof(HEAD)+head.bussinessLength);
            if(calc_crc!=recv_crc)
            {
                qDebug()<<"CRC错误，丢弃上传图片包";
                continue;
            }
            else{
                qDebug()<<"CRC匹配成功";
                memcpy(&this->resultback,this->buf,sizeof(RESULTBACK));
                qDebug()<<"文件上传成功flag="<<resultback.flag;
                emit this->send2MainWin(resultback.flag,resultback.message);
                emit this->send2ParkingWin(resultback.flag,resultback.message);
            }
        }
        //入场记录是否成功
        else if(this->head.bussinessType==INRECORD_T)
        {
            //CRC校验
            unsigned int recv_crc=ntohl(this->head.crc);
            qDebug()<<"recv_crc= "<<recv_crc;

            res=read(this->socketfd,this->buf,sizeof(RESULTBACK));
            qDebug()<<"readRes="<<res;
            unsigned char fullBuf[sizeof(HEAD)+head.bussinessLength];
            memcpy(fullBuf,&head,sizeof(HEAD));
            memcpy(fullBuf+sizeof(HEAD),buf,head.bussinessLength);
            auto pcrc=reinterpret_cast<std::uint32_t*>(fullBuf+offsetof(HEAD,crc));
            *pcrc=0;
            unsigned int calc_crc=CTool::crc32(0xFFFFFFFF,fullBuf,sizeof(HEAD)+head.bussinessLength);
            if(calc_crc!=recv_crc)
            {
                qDebug()<<"CRC错误，丢弃上传图片包";
                continue;
            }
            else{
                qDebug()<<"CRC匹配成功";
                memcpy(&this->resultback,this->buf,sizeof(RESULTBACK));
                qDebug()<<"文件上传成功flag="<<resultback.flag;
                emit this->send2MainWin2(resultback.flag,resultback.message);
            }
        }
        //出场车牌的上传返回体flag==2,
        else if(this->head.bussinessType==OUTRECORD_T)
        {
            //CRC校验
            unsigned int recv_crc=ntohl(this->head.crc);
            qDebug()<<"recv_crc= "<<recv_crc;

            res=read(this->socketfd,this->buf,sizeof(RESULTBACK));
            qDebug()<<"readRes="<<res;
            unsigned char fullBuf[sizeof(HEAD)+head.bussinessLength];
            memcpy(fullBuf,&head,sizeof(HEAD));
            memcpy(fullBuf+sizeof(HEAD),buf,head.bussinessLength);
            auto pcrc=reinterpret_cast<std::uint32_t*>(fullBuf+offsetof(HEAD,crc));
            *pcrc=0;
            unsigned int calc_crc=CTool::crc32(0xFFFFFFFF,fullBuf,sizeof(HEAD)+head.bussinessLength);
            if(calc_crc!=recv_crc)
            {
                qDebug()<<"CRC错误，丢弃上传图片包";
                continue;
            }
            else{
                qDebug()<<"CRC匹配成功";
                memcpy(&this->resultback,this->buf,sizeof(RESULTBACK));
                qDebug()<<"出库车牌信息上传成功flag="<<resultback.flag;
                emit this->send2MainWin3(resultback.flag,resultback.message);
            }
        }
        else if(this->head.bussinessType==RESULTBACK_T)
        {
            //CRC校验
            unsigned int recv_crc=ntohl(this->head.crc);
            qDebug()<<"recv_crc= "<<recv_crc;

            res=read(this->socketfd,this->buf,sizeof(RESULTBACK));
            qDebug()<<"readRes="<<res;
            unsigned char fullBuf[sizeof(HEAD)+head.bussinessLength];
            memcpy(fullBuf,&head,sizeof(HEAD));
            memcpy(fullBuf+sizeof(HEAD),buf,head.bussinessLength);
            auto pcrc=reinterpret_cast<std::uint32_t*>(fullBuf+offsetof(HEAD,crc));
            *pcrc=0;
            unsigned int calc_crc=CTool::crc32(0xFFFFFFFF,fullBuf,sizeof(HEAD)+head.bussinessLength);
            if(calc_crc!=recv_crc)
            {
                qDebug()<<"CRC错误，丢弃上传图片包";
                continue;
            }
            else{
                qDebug()<<"CRC匹配成功";
                memcpy(&this->resultback,this->buf,sizeof(RESULTBACK));
                qDebug()<<"放行请求成功flag="<<resultback.flag;
                emit this->send2MainWin4(resultback.flag,resultback.message);
            }
        }
        else if(this->head.bussinessType==VIDEOPLAY_T)
        {
            //CRC校验
            unsigned int recv_crc=ntohl(this->head.crc);
            qDebug()<<"recv_crc= "<<recv_crc;

            res=read(this->socketfd,this->buf,sizeof(RESULTBACK));
            qDebug()<<"readRes="<<res;
            unsigned char fullBuf[sizeof(HEAD)+head.bussinessLength];
            memcpy(fullBuf,&head,sizeof(HEAD));
            memcpy(fullBuf+sizeof(HEAD),buf,head.bussinessLength);
            auto pcrc=reinterpret_cast<std::uint32_t*>(fullBuf+offsetof(HEAD,crc));
            *pcrc=0;
            unsigned int calc_crc=CTool::crc32(0xFFFFFFFF,fullBuf,sizeof(HEAD)+head.bussinessLength);
            if(calc_crc!=recv_crc)
            {
                qDebug()<<"CRC错误，丢弃上传图片包";
                continue;
            }
            else{
                qDebug()<<"CRC匹配成功";
                memcpy(&this->resultback,this->buf,sizeof(RESULTBACK));
                qDebug()<<"录制视频上传成功flag="<<resultback.flag;
                emit this->send2ParkinWin2(resultback.flag,resultback.message);
            }
        }
        //车辆信息查询
        else if(this->head.bussinessType==INFOBACK_T)
        {
            //CRC校验
            unsigned int recv_crc=ntohl(this->head.crc);
            qDebug()<<"recv_crc= "<<recv_crc;

            res=read(this->socketfd,this->buf,sizeof(INFOBACK));
            qDebug()<<"readRes="<<res;
            unsigned char fullBuf[sizeof(HEAD)+head.bussinessLength];
            memcpy(fullBuf,&head,sizeof(HEAD));
            memcpy(fullBuf+sizeof(HEAD),buf,head.bussinessLength);
            auto pcrc=reinterpret_cast<std::uint32_t*>(fullBuf+offsetof(HEAD,crc));
            *pcrc=0;
            unsigned int calc_crc=CTool::crc32(0xFFFFFFFF,fullBuf,sizeof(HEAD)+head.bussinessLength);
            if(calc_crc!=recv_crc)
            {
                qDebug()<<"CRC错误，丢弃上传图片包";
                continue;
            }
            else{
                qDebug()<<"CRC匹配成功";
                memcpy(&this->carinfoback,this->buf,sizeof(INFOBACK));
                qDebug()<<"车辆查询请求上传成功number="<<carinfoback.number;
                QVector<CARINFO>tempcarinfo;
                for(int i=0;i<carinfoback.number;i++)
                {
                    tempcarinfo.push_back(carinfoback.picture_back[i]);
//                    qDebug()<<"number="<<carinfoback.picture_back[i].number;
//                    qDebug()<<"in_time="<<carinfoback.picture_back[i].in_time;
//                    qDebug()<<"out_time="<<carinfoback.picture_back[i].out_time;
//                    qDebug()<<"in_picture_path="<<carinfoback.picture_back[i].in_picture_path;
//                    qDebug()<<"out_picture_path="<<carinfoback.picture_back[i].out_picture_path;
//                    qDebug()<<"cost="<<carinfoback.picture_back[i].cost;
                }
                emit this->send2CarinfoWin(carinfoback.number,tempcarinfo);
            }
        }
        //按天查询
        else if(this->head.bussinessType==VIDEODAY_T)
        {
            //CRC校验
            unsigned int recv_crc=ntohl(this->head.crc);
            qDebug()<<"recv_crc= "<<recv_crc;

            res=read(this->socketfd,this->buf,sizeof(VIDEODATE));
            qDebug()<<"readRes="<<res;
            unsigned char fullBuf[sizeof(HEAD)+head.bussinessLength];
            memcpy(fullBuf,&head,sizeof(HEAD));
            memcpy(fullBuf+sizeof(HEAD),buf,head.bussinessLength);
            auto pcrc=reinterpret_cast<std::uint32_t*>(fullBuf+offsetof(HEAD,crc));
            *pcrc=0;
            unsigned int calc_crc=CTool::crc32(0xFFFFFFFF,fullBuf,sizeof(HEAD)+head.bussinessLength);
            if(calc_crc!=recv_crc)
            {
                qDebug()<<"CRC错误，丢弃按天查询";
                continue;
            }
            else{
                qDebug()<<"CRC匹配成功";
                memcpy(&this->dayinfoback,this->buf,sizeof(VIDEODATE));
                qDebug()<<"按天查询请求上传成功number="<<dayinfoback.number;
                QStringList timeList; // 使用局部变量
                for(int i=0;i<dayinfoback.number;i++)
                {
                    timeList.append(QString::fromUtf8(dayinfoback.video_time[i]));
                    //qDebug()<<timeList[i];
                }
                qDebug()<<"timeList.size="<<timeList.size();
                emit this->send2MonitorWin(dayinfoback.number,timeList);
                //qDebug()<<"111";
            }
        }
        //按月查询
        else if(this->head.bussinessType==VIDEOMONTH_T)
        {
            //CRC校验
            unsigned int recv_crc=ntohl(this->head.crc);
            qDebug()<<"recv_crc= "<<recv_crc;

            res=read(this->socketfd,this->buf,sizeof(VIDEODATE));
            qDebug()<<"readRes="<<res;
            unsigned char fullBuf[sizeof(HEAD)+head.bussinessLength];
            memcpy(fullBuf,&head,sizeof(HEAD));
            memcpy(fullBuf+sizeof(HEAD),buf,head.bussinessLength);
            auto pcrc=reinterpret_cast<std::uint32_t*>(fullBuf+offsetof(HEAD,crc));
            *pcrc=0;
            unsigned int calc_crc=CTool::crc32(0xFFFFFFFF,fullBuf,sizeof(HEAD)+head.bussinessLength);
            if(calc_crc!=recv_crc)
            {
                qDebug()<<"CRC错误，丢弃按月查询";
                continue;
            }
            else{
                qDebug()<<"CRC匹配成功";
                memcpy(&this->dayinfoback,this->buf,sizeof(VIDEODATE));
                qDebug()<<"按月查询请求上传成功number="<<dayinfoback.number;
                QStringList timeList; // 使用局部变量
                for(int i=0;i<dayinfoback.number;i++)
                {
                    timeList << QString::fromLocal8Bit(dayinfoback.video_time[i]);
                    //qDebug()<<timeList[i];
                }
                qDebug()<<"timeList.size="<<timeList.size();
                emit this->send2MonitorWin2(dayinfoback.number,timeList);
            }
        }
        //视频信息返回
        else if(this->head.bussinessType==VIDEOBACK_T)
        {
            //CRC校验
            unsigned int recv_crc=ntohl(this->head.crc);
            qDebug()<<"recv_crc= "<<recv_crc;

            res=read(this->socketfd,this->buf,sizeof(VIDEOBACK));
            qDebug()<<"readRes="<<res;
            unsigned char fullBuf[sizeof(HEAD)+head.bussinessLength];
            memcpy(fullBuf,&head,sizeof(HEAD));
            memcpy(fullBuf+sizeof(HEAD),buf,head.bussinessLength);
            auto pcrc=reinterpret_cast<std::uint32_t*>(fullBuf+offsetof(HEAD,crc));
            *pcrc=0;
            unsigned int calc_crc=CTool::crc32(0xFFFFFFFF,fullBuf,sizeof(HEAD)+head.bussinessLength);
            if(calc_crc!=recv_crc)
            {
                qDebug()<<"CRC错误，丢弃视频信息返回";
                continue;
            }
            else{
                qDebug()<<"CRC匹配成功";
                memcpy(&this->videoback,this->buf,sizeof(VIDEOBACK));
                qDebug()<<"视频信息返回请求上传成功number="<<videoback.number;
                QVector<VIDEO> tempvideo; // 使用局部变量
                for(int i=0;i<videoback.number;i++)
                {
                    tempvideo.push_back(videoback.video_back[i]);
                }
                qDebug()<<"tempvideo.size="<<tempvideo.size();
                QVector<VIDEO> safeCopy = tempvideo;  // 触发深拷贝
                emit this->send2MonitorWin3(videoback.number,safeCopy);
            }
        }
        //入场表格数据结构体返回
        else if(this->head.bussinessType==UPDATETABLE_T)
        {
            //CRC校验
            unsigned int recv_crc=ntohl(this->head.crc);
            qDebug()<<"recv_crc= "<<recv_crc;

            res=read(this->socketfd,this->buf,sizeof(VIDEOBACK));
            qDebug()<<"readRes="<<res;
            unsigned char fullBuf[sizeof(HEAD)+head.bussinessLength];
            memcpy(fullBuf,&head,sizeof(HEAD));
            memcpy(fullBuf+sizeof(HEAD),buf,head.bussinessLength);
            auto pcrc=reinterpret_cast<std::uint32_t*>(fullBuf+offsetof(HEAD,crc));
            *pcrc=0;
            unsigned int calc_crc=CTool::crc32(0xFFFFFFFF,fullBuf,sizeof(HEAD)+head.bussinessLength);
            if(calc_crc!=recv_crc)
            {
                qDebug()<<"CRC错误，丢弃入场表格结构体返回返回";
                continue;
            }
            else{
                qDebug()<<"CRC匹配成功";
                memcpy(&this->tableback,this->buf,sizeof(TABLES));
                qDebug()<<"入场表格结构体请求上传成功number="<<tableback.number;
                QVector<UPDATATABLE> temptable; // 使用局部变量
                for(int i=0;i<tableback.number;i++)
                {
                    temptable.push_back(tableback.table[i]);
                    //qDebug()<<videoback.video_back[i].video_path;
                }

                qDebug()<<"temptable.size="<<temptable.size();
                emit this->send2LoginWin5(tableback.number,temptable);
            }
        }
        //视频信息更新的返回体
        else if(this->head.bussinessType==VIDEOUPDATE_T)
        {
            //CRC校验
            unsigned int recv_crc=ntohl(this->head.crc);
            qDebug()<<"recv_crc= "<<recv_crc;

            res=read(this->socketfd,this->buf,sizeof(RESULTBACK));
            qDebug()<<"readRes="<<res;
            unsigned char fullBuf[sizeof(HEAD)+head.bussinessLength];
            memcpy(fullBuf,&head,sizeof(HEAD));
            memcpy(fullBuf+sizeof(HEAD),buf,head.bussinessLength);
            auto pcrc=reinterpret_cast<std::uint32_t*>(fullBuf+offsetof(HEAD,crc));
            *pcrc=0;
            unsigned int calc_crc=CTool::crc32(0xFFFFFFFF,fullBuf,sizeof(HEAD)+head.bussinessLength);
            if(calc_crc!=recv_crc)
            {
                qDebug()<<"CRC错误，丢弃视频信息更新返回";
                continue;
            }
            else{
                qDebug()<<"CRC匹配成功";
                memcpy(&this->resultback,this->buf,sizeof(RESULTBACK));
                qDebug()<<"视频信息更新请求上传成功flag="<<resultback.flag;
                emit this->send2MonitorWin4(resultback.flag);
            }
        }
    }
}
