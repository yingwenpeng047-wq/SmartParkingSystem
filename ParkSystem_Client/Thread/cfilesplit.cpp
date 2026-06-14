#include "cfilesplit.h"

CFileSplit *CFileSplit::cfileSplit=nullptr;

CFileSplit *CFileSplit::getInstance()
{
    if(CFileSplit::cfileSplit==nullptr)
    {
        CFileSplit::cfileSplit=new CFileSplit();
    }
    return CFileSplit::cfileSplit;
}


CFileSplit::CFileSplit():QThread()
{
    bzero(this->buf,sizeof(this->buf));
    memset(&this->tempFileInfo,0,sizeof(this->tempFileInfo));

    this->sc=SocketConnect::getInstance();
    this->pool=QThreadPool::globalInstance();
    this->pool->setMaxThreadCount(6);
    this->writethread=WriteThread::getInstance();
}



QString CFileSplit::getUser_account() const
{
    return user_account;
}

void CFileSplit::setUser_account(const QString &value)
{
    user_account = value;
}



CFileSplit::~CFileSplit()
{

}

void CFileSplit::addFile(const QString &filePath)
{
    this->manyFile.enqueue(filePath);
}


void CFileSplit::run()
{    
    while(1)
    {
        //有文件,开始拆分
        if(this->manyFile.size()>0)
        {
            memset(&this->tempFileInfo, 0, sizeof(this->tempFileInfo));
            int currentIndex=0;         //碎片序号
            int resbuf=0;
            //---------1出队,获取要拆分的文件路径-------
            this->path=this->manyFile.dequeue();
            qDebug()<<"path="<<this->path;
            int readfd = open(this->path.toStdString().c_str(), O_RDONLY,0777);	//只读方式打开图片
            if (readfd < 0)
            {
                perror("打开失败");
                return;
            }
            //-----------2文件账号--------
            memcpy(this->tempFileInfo.user_account,this->user_account.toStdString().c_str(),sizeof(this->tempFileInfo.user_account));
            qDebug()<<"this->tempFileInfo.user_account="<<this->tempFileInfo.user_account;
            //---------------3文件名称----------------
            QString fileName=this->path.split("/").last();
            memcpy(this->tempFileInfo.fileName,fileName.toStdString().c_str(),sizeof(this->tempFileInfo.fileName));
            qDebug()<<"this->tempFileInfo.fileName="<<this->tempFileInfo.fileName;
            qDebug()<<"4444----";
            //---------------4获取文件总字节数------------------
            this->tempFileInfo.length=lseek(readfd,0,SEEK_END);
            //获取文件拆分总数
            if(this->tempFileInfo.length % sizeof(this->buf)==0)
            {
                this->tempFileInfo.number=this->tempFileInfo.length/sizeof(this->buf);
            }
            else{
                this->tempFileInfo.number=this->tempFileInfo.length/sizeof(this->buf)+1;
            }

            qDebug()<<"this->tempFileInfo.length="<<this->tempFileInfo.length;
            lseek(readfd,0,SEEK_SET);
            //读取到内容了
            //read函数：读取哪个文件的内容，读取到哪里去，读取多大的字节数，返回值是实际读取到的多少个字节
            while ((resbuf = read(readfd, buf, sizeof(buf))) !=0) {
                qDebug()<<"resbuf="<<resbuf;
                //当前碎片序号
                this->tempFileInfo.fileIndex=++currentIndex;
                //当前碎片总长度
                this->tempFileInfo.fileLength=resbuf;
                //文件二进制内容
                memcpy(this->tempFileInfo.context,this->buf,this->tempFileInfo.fileLength);

                qDebug()<<"this->tempFileInfo.context="<<this->tempFileInfo.context;

                qDebug()<<"this->tempFileInfo.fileLength="<<this->tempFileInfo.fileLength;

                //进队
                this->fileinfo.push_back(this->tempFileInfo);

                //清空buf和暂时存放fileinfo的结构体
                bzero(buf, sizeof(buf));

                bzero(this->tempFileInfo.context, sizeof(this->tempFileInfo.context));
            }
            //逐步读取碎片队列里的东西，然后加入到线程池中
            int size=this->fileinfo.size();
            for(int i=0;i<size;i++)
            {
                MyRunnable *task=new MyRunnable(this->fileinfo.dequeue());
                //MyRunnable *task=new MyRunnable(this->fileinfo[i]);
                //设置服务器套接字
                task->setSocketfd(this->sc->getSocketfd());
                task->setAutoDelete(true);
                this->pool->start(task);
                QThread::msleep(300);
            }
            close(readfd);
            bzero(&this->tempFileInfo, sizeof(this->tempFileInfo));
            bzero(this->buf,sizeof(this->buf));
            //等待所有任务完成，所有包都发出去了
            this->pool->waitForDone();
            QThread::msleep(1000);
            //再次利用写线程发尾包
            //准备尾包数据
            bzero(&this->filecheck, sizeof(this->filecheck));
            memcpy(this->filecheck.user_account,this->user_account.toStdString().c_str(),sizeof(this->filecheck.user_account));
            strcpy(this->filecheck.fileName, fileName.toStdString().c_str());
            memcpy(this->filecheck.cpath,this->path.toStdString().c_str(),sizeof(this->filecheck.cpath));

            //利用写线程发送尾包
            this->head.bussinessType=FILECHECK_T;
            this->head.bussinessLength=sizeof(FILECHECK);

            memcpy(buf,&head,sizeof(HEAD));
            memcpy(buf+sizeof(HEAD),&this->filecheck,sizeof(this->filecheck));


            uint32_t *pcrc = reinterpret_cast<std::uint32_t *>(buf + offsetof(HEAD, crc));
            *pcrc = 0;
            //3.CRC 计算范围：整个包头(含清零后的 crc) + 包体
            uint32_t crc = CTool::crc32(0xFFFFFFFF,reinterpret_cast<unsigned char *>(buf),sizeof(HEAD) + sizeof(FILECHECK));
            //4.把结果写回包头，网络字节序
            *pcrc = htonl(crc);
            this->writethread->setData(buf,sizeof(this->head)+sizeof(this->filecheck));
            //清空buf
            bzero(buf, sizeof(buf));
        }
    }
}
