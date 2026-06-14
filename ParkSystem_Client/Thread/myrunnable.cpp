#include "myrunnable.h"

MyRunnable::MyRunnable(FILEINFO fileinfo)
{
    memcpy(&this->fileinfo,&fileinfo,sizeof(this->fileinfo));
    CTool::init_crc_table();
}

MyRunnable::~MyRunnable()
{

}


//将一个fileinfo类型的结构体发送出去给到服务器
void MyRunnable::run()
{
    qDebug()<<"myrunn";
    if(this->fileinfo.length>0)
    {
        bzero(&this->head,sizeof(HEAD));
        bzero(this->buf,sizeof(this->buf));
        this->head.bussinessLength=sizeof(this->fileinfo);
        this->head.bussinessType=FILEINFO_T;

        memcpy(this->buf,&this->head,sizeof(HEAD));
        memcpy(this->buf+sizeof(HEAD),&this->fileinfo,sizeof(FILEINFO));

        //2.计算 CRC 之前先把 crc 字段清 0
        //用 std::uint32_t 保证 4 字节无歧义
        uint32_t *pcrc = reinterpret_cast<std::uint32_t *>(buf + offsetof(HEAD, crc));
        *pcrc = 0;
        //3.CRC 计算范围：整个包头(含清零后的 crc) + 包体
        uint32_t crc = CTool::crc32(0xFFFFFFFF,reinterpret_cast<unsigned char *>(buf),sizeof(HEAD) + sizeof(FILEINFO));
        //4.把结果写回包头，网络字节序
        *pcrc = htonl(crc);
        int res=write(this->socketfd,this->buf,sizeof(HEAD)+sizeof(FILEINFO));
        if(res>0)
        {
            bzero(&this->fileinfo,sizeof(this->fileinfo));
            qDebug()<<"文件碎片发送成功";
        }
    }
}

void MyRunnable::setSocketfd(int value)
{
    socketfd = value;
}
