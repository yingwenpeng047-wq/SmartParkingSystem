#pragma once
#include "CTcpServer.h"
#include <sys/epoll.h>
#include "CFrontThreadPool.h"
#include "WriteMemShare.h"
#include <unordered_map>
#include "FrontLog.h"

using Clock = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<Clock>;

class CEpollServer:public CTcpServer
{
public:
	CEpollServer(unsigned short port, int type, int sin_family = AF_INET, int protocol = 0);
	~CEpollServer();

	/// <summary>
	/// 工作函数 处理客户端连接与掉线，读取客户端发来的数据并交给线程池处理
	/// </summary>
	virtual void work() override;

	int getEpollWaitNum();
	void setEpollWaitNum(int epollwaitnum);

	/// <summary>
	/// 更新实时日志显示内容
	/// </summary>
	void updateLog(protocolType type);

	/// <summary>
	/// 删除心跳超时的客户端
	/// </summary>
	static void delCustom(int fd);

	static int epollfd;		//代表epoll红黑树头节点

private:
	int epollwaitNum;		//就绪列表发生事件的fd数量
	struct epoll_event epollevent;			//期望发送xx事件的结构体
	struct epoll_event epolleventArry[20];	//已经发生事件的结构体数组
	char resbuf[2560];
	CFrontThreadPool* pool;
	WriteMemShare* writeMemShare;

};

