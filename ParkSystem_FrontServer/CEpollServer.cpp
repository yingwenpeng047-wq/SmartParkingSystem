#include "CEpollServer.h"

/*
epoll主要是有红黑树和就序列表两个数据结构，然后首先通过epoll_create创建一个epollfd，然后将我需要监听的fd的IO操作通过epoll_ctl都加入到epollfd里面去，当内核检测到这些fd的IO操作的时候，
内核将就绪的fd放入到就绪列表，通过epoll_wait将就序列表返回给用户，然后去遍历整个就序列表看是哪个fd发生了什么IO操作
*/

int CEpollServer::epollfd = -1;
extern unordered_map<int, TimePoint> custom_fd;

CEpollServer::CEpollServer(unsigned short port, int type, int sin_family, int protocol):CTcpServer(port, type, sin_family, protocol)
{
	//网络搭建
	CTcpServer::work();

	//创建前置服务器线程池，四个线程的线程池
	this->pool = new CFrontThreadPool(4);
	//启动共享内存收发数据
	this->writeMemShare = new WriteMemShare();

	CEpollServer::epollfd = -1;
	//创建红黑树结构，参数含义位可以识别多少个fd
	CEpollServer::epollfd = epoll_create(20);	//就绪列表数量和识别数量要一致
	bzero(this->resbuf, sizeof(this->resbuf));

	//2.将服务器网络端口socketfd 添加保存到epoll事件列表
	this->epollevent.data.fd = this->socketfd;
	this->epollevent.events = EPOLLIN | EPOLLOUT;	//文件描述符的可读事件
	epoll_ctl(CEpollServer::epollfd, EPOLL_CTL_ADD, this->socketfd, &this->epollevent);
}

CEpollServer::~CEpollServer()
{
	//关闭发送数据线程
	this->writeMemShare->endThread();

	//关闭epoll实例
	if (CEpollServer::epollfd != -1)
	{
		close(CEpollServer::epollfd);
	}

}

void CEpollServer::work()
{
	HEAD head;
	while (1)
	{
		//这里的监听20个是说20个fd，也就是说一次性最多返回20个fd的事件，一个fd可能对应多个事件
		this->epollwaitNum = epoll_wait(CEpollServer::epollfd, this->epolleventArry, 20, -1); //监听20个，-1表示永远阻塞等待

		if (this->epollwaitNum < 0)
		{
			perror("epoll_wait error");
			return;
		}
		else
		{
			int res = 0;
			//循环遍历已经发生的事件的fd，对每一个fd进行相应处理
			for (int i = 0; i < this->epollwaitNum; i++)
			{
				//如果是服务器的监听socket，处理新的连接--接收新连接
				if (this->epolleventArry[i].data.fd == this->socketfd)
				{
					//阻塞式函数，一直等待客户端连接
					this->acceptfd = accept(this->socketfd, NULL, NULL);

					FrontLog* frontLog = FrontLog::getInstance();
					frontLog->setConnection(frontLog->getConnection() + 1);
					//记录首次连接的心跳
					custom_fd[this->acceptfd] = Clock::now();

					bzero(&this->epollevent, sizeof(this->epollevent));
					this->epollevent.data.fd = this->acceptfd;
					this->epollevent.events = EPOLLIN | EPOLLET | EPOLLRDHUP; // 添加EPOLLRDHUP检测半关闭
					epoll_ctl(CEpollServer::epollfd, EPOLL_CTL_ADD, this->acceptfd, &this->epollevent);
				}
				// 处理可读事件或连接关闭事件，处理客户端连接socket---接收数据
				else if (this->epolleventArry[i].events & (EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLERR))		//事件中包含其中任意一个就行，就为真
				{
					int client_fd = this->epolleventArry[i].data.fd;

					// 检查是否是连接关闭事件
					if (this->epolleventArry[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) 
					{
						// 从epoll中移除
						bzero(&this->epollevent, sizeof(this->epollevent));
						this->epollevent.data.fd = client_fd;
						epoll_ctl(CEpollServer::epollfd, EPOLL_CTL_DEL, client_fd, &this->epollevent);

						// 关闭套接字
						close(client_fd);
						if (custom_fd.erase(client_fd) != 0)
						{
							FrontLog* frontLog = FrontLog::getInstance();
							frontLog->setConnection(frontLog->getConnection() - 1);
						}

						continue;
					}

					bzero(this->resbuf, sizeof(this->resbuf));
					bzero(&head, sizeof(HEAD));

					//读取头
					res = read(this->epolleventArry[i].data.fd, &head, sizeof(HEAD)); // 接收数据

					if (res < sizeof(HEAD))	//请求头丢包
					{
						continue;
					}

					//记录心跳---处理心跳业务
					else if (head.bussinessType == LIFE_T)
					{
						custom_fd[client_fd] = Clock::now();
						this->updateLog((protocolType)head.bussinessType);
					}
					else 
					{
						//处理其他的普通业务
						memcpy(this->resbuf, &head, sizeof(HEAD));
						res = read(this->epolleventArry[i].data.fd, this->resbuf + sizeof(HEAD), head.bussinessLength); // 接收数据

						//数据包不完整
						if (res != head.bussinessLength)
						{
							continue;
						}
						this->updateLog((protocolType)head.bussinessType);
						//提交给线程池进行解析处理
						this->pool->pushTask(this->epolleventArry[i].data.fd, this->resbuf, sizeof(HEAD) + head.bussinessLength);
						bzero(this->resbuf, sizeof(this->resbuf));
					}
				}
			}
		}
	}
}

int CEpollServer::getEpollWaitNum()
{
	return this->epollwaitNum;
}

void CEpollServer::setEpollWaitNum(int epollwaitnum)
{
	this->epollwaitNum = epollwaitnum;
}

void CEpollServer::updateLog(protocolType type)
{
	FrontLog* frontLog = FrontLog::getInstance();
	frontLog->setReceived(frontLog->getReceived() + 1);

	switch (type)
	{
		case VIDEOBACK_T:frontLog->setVideoList(frontLog->getVideoList() + 1); break;
		case VIDEOPLAY_T:frontLog->setVideoRecord(frontLog->getVideoRecord() + 1); break;
		case FILECHECK_T:frontLog->setPictureFile(frontLog->getPictureFile() + 1); break;
		case VIDEOUPDATE_T:frontLog->setVideoUpdate(frontLog->getVideoUpdate() + 1); break;
		case INRECORD_T:frontLog->setCarUpdate(frontLog->getCarUpdate() + 1); break;
		case INFOSEARCH_T:frontLog->setCarSearch(frontLog->getCarSearch() + 1); break;
	}
}

void CEpollServer::delCustom(int fd)
{
	//删除客户端连接
	epoll_ctl(CEpollServer::epollfd, EPOLL_CTL_DEL, fd, nullptr);
	//关闭客户端连接
	close(fd);

	//删除心跳记录
	if (custom_fd.erase(fd) != 0)
	{
		FrontLog* frontLog = FrontLog::getInstance();
		frontLog->setConnection(frontLog->getConnection() - 1);
	}
}
