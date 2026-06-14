#pragma once
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include "WriteMemShare.h"
#include "protocol.h"
#include <unordered_map>
#include "CEpollServer.h"
using namespace std;

using Clock = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<Clock>;


//心跳线程
class HeartBeatThread
{
public:
	HeartBeatThread();
	~HeartBeatThread();

	/// <summary>
	/// 检查客户端心跳时间间隔，超出规定间隔进行下线处理
	/// </summary>
	/// <param name="userMap">保存客户端的map容器</param>
	static void* doCheck(void* arg);
};

