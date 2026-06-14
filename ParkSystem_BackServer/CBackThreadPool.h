#pragma once
#include <iostream>
#include <pthread.h>
#include <list>
#include <algorithm>
#include <queue>
#include "protocol.h"
#include "CDataTask.h"

using namespace std;

#define MIN_THREAD_NUN 2
#define MAX_THREAD_NUM 8

//后置服务器线程池
class CBackThreadPool
{
public:
	CBackThreadPool(const int num = MIN_THREAD_NUN);
	~CBackThreadPool();

	//对应线程互斥量 进行相关操作的函数 二次封装线程加锁和解锁函数
	void lock();
	void unlock();

	void wait();	//休息
	void wakeup();	//唤醒

	void addRecvQueue(SHAREMEM* recvMem);
	CBaseTask* popTask();						//取(移除)任务函数
	bool queueIsEmpty();						//判断任务是否为空

	void moveToIdle(pthread_t id);	//从忙碌列表到空闲列表
	void moveToBusy(pthread_t id);	//从空闲列表到忙碌列表

	void addThread();	//添加新线程
	void delThread();	//删除线程

	static void* thread_function(void* arg);


private:
	int threadMinNum;	//线程最小数量
	int threadMaxNum;	//线程最大数量

	//任务队列
	queue<CBaseTask*> taskQueue;

	list<pthread_t> idleList;	//空闲线程队列
	list<pthread_t>	busyList;	//忙碌线程队列

	pthread_mutex_t mutex;	//线程互斥量
	pthread_cond_t cond;	//线程的条件变量 用于让线程出现阻塞等待和唤醒

};

