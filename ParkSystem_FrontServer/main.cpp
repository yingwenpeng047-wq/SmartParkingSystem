#include <iostream>
#include "ReadMemShare.h"
#include "FrontLog.h"
#include "HeartBeatThread.h"
#include <unordered_map>
using namespace std;

unordered_map<int, TimePoint> custom_fd;

int main()
{
	//心跳监听线程
	HeartBeatThread* heartThread = new HeartBeatThread();

	//实时日志线程
	FrontLog* frontLog = FrontLog::getInstance();

	ReadMemShare* readMemShare = new ReadMemShare();
	CEpollServer* tcp = new CEpollServer(10001, SOCK_STREAM);
	tcp->work();

	return 0;
}
