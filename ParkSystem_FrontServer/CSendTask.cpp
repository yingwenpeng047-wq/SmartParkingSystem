#include "CSendTask.h"

CSendTask::CSendTask(char* buf,int fd,int size):CBaseTask()
{
	bzero(this->buf, sizeof(this->buf));
    memcpy(this->buf, buf, size);
	this->fd = fd;	//保存网络通道描述符
}

CSendTask::~CSendTask()
{
}

void CSendTask::work()
{
	memcpy(&(this->head), this->buf, sizeof(HEAD));
	// CRC 校验：包头(不含crc) + 包体
	unsigned int recv_crc = ntohl(this->head.crc);
	unsigned char fullBuf[sizeof(HEAD) + this->head.bussinessLength];
	memcpy(fullBuf, &this->head, sizeof(HEAD));										// 包头（crc 已清零）
	memcpy(fullBuf + sizeof(HEAD), buf + sizeof(HEAD), this->head.bussinessLength); // 包体

	//把 fullBuf 中 HEAD 结构体的 crc 字段临时清零
	*((unsigned int*)(fullBuf + offsetof(HEAD, crc))) = 0;
	unsigned int calc_crc = CBaseTask::crc32(0xFFFFFFFF, fullBuf, sizeof(HEAD) + this->head.bussinessLength);

	//CRC验证
	if (calc_crc != recv_crc)	
	{
		return;
	}
	else    //将信息准备发送到后置服务器
	{
		SHAREMEM* sharemem = new SHAREMEM();
		memcpy(sharemem->buf, fullBuf, sizeof(fullBuf));
		sharemem->fd = this->fd;
		WriteMemShare::sendQueue.push(sharemem);
	}
}

HEAD CSendTask::getHead()
{
    return this->head;
}

void CSendTask::setHead(HEAD* head)
{
    memcpy(&(this->head), head, sizeof(HEAD));
}