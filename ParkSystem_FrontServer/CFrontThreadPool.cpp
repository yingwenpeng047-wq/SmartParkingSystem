#include "CFrontThreadPool.h"

CFrontThreadPool::CFrontThreadPool(const int num)
{
	if (num > 0 && num * 4 <= MAX_THREAD_NUM)
	{
		this->threadMinNum = num;
		this->threadMaxNum = MAX_THREAD_NUM;
	}
	else 
	{
		this->threadMinNum = MAX_THREAD_NUM;
		this->threadMaxNum = MAX_THREAD_NUM;
	}

	//线程互斥量初始化
	pthread_mutex_init(&this->mutex, NULL);

	//线程条件变量初始化
	pthread_cond_init(&this->cond, NULL);

	for (int i = 0; i < this->threadMinNum; i++)
	{
		pthread_t id;
		pthread_create(&id, NULL, thread_function, this);
		//创建好的线程加入空闲列表
		this->idleList.push_back(id);
	}
	//执行初始化CRC校验
	CBaseTask::init_crc_table();
}

CFrontThreadPool::~CFrontThreadPool()
{
}

//对应线程互斥量 进行相关操作的函数 二次封装线程加锁和解锁函数
void CFrontThreadPool::lock()
{
	pthread_mutex_lock(&(this->mutex));
}

void CFrontThreadPool::unlock()
{
	pthread_mutex_unlock(&(this->mutex));
}

//对应线程条件变量  进行相关操作的函数
void CFrontThreadPool::wait()
{
	//注意第二个参数必须是上锁之后的线程互斥量
	pthread_cond_wait(&(this->cond), &(this->mutex));
}

//唤醒线程函数
void CFrontThreadPool::wakeup()
{
	pthread_cond_signal(&(this->cond));
}

//添加任务函数
void CFrontThreadPool::pushTask(int fd,char* buf,int size)
{
	//一个是哪个文件描述符也就是哪个fd发来的消息，发来了什么消息，多大的消息
	CBaseTask* task = new CSendTask(buf,fd,size);
	//添加任务
	this->taskQueue.push(task);
	this->wakeup();
}

CBaseTask* CFrontThreadPool::popTask()
{
	//从队头取任务
	CBaseTask* task = this->taskQueue.front();
	//从队列删除队头任务
	this->taskQueue.pop();
	return task;
}

//判断任务队列是否为空函数
bool CFrontThreadPool::queueIsEmpty()
{
	return this->taskQueue.empty();
}

void CFrontThreadPool::moveToIdle(pthread_t id)
{
	list<pthread_t>::iterator iter;
	//通过传入的线程id在忙碌链表查找id对应的线程
	iter = find(this->busyList.begin(), this->busyList.end(), id);

	if (iter != busyList.end())
	{
		//从忙碌链表移除，添加空闲链表
		this->busyList.erase(iter);
		this->idleList.push_back(*iter);
	}
}

void CFrontThreadPool::moveToBusy(pthread_t id)
{
	list<pthread_t>::iterator iter;
	//通过传入的线程id在空闲链表查找id对应的线程
	iter = find(this->idleList.begin(), this->idleList.end(), id);

	if (iter != idleList.end())
	{
		//从空闲链表移除，添加忙碌链表
		this->idleList.erase(iter);
		this->busyList.push_back(*iter);
	}
}

void CFrontThreadPool::addThread()
{
	if (this->busyList.size() < this->threadMaxNum)
	{
		//如果没有空闲线程，并且忙碌线程数小于最大线程数，则创建新线程
		pthread_t id;
		pthread_create(&id, NULL, thread_function, this);
		this->idleList.push_back(id);
	}
}

void CFrontThreadPool::delThread()
{
	//如果空闲线程数大于最小线程数，则删除空闲线程
	while (this->idleList.size() > this->threadMinNum - 1)
	{
		pthread_t idleThread = this->idleList.front();
		this->idleList.pop_front();
		pthread_cancel(idleThread); //取消线程
	}
}

void* CFrontThreadPool::thread_function(void* arg)
{
	CFrontThreadPool* pool = (CFrontThreadPool*)arg;
	//线程执行期间获取自身id
	pthread_t id = pthread_self();
	//系统记录此id，并在线程无后续操作时自动回收  并不是在此处回收
	pthread_detach(id);

	while (1)
	{
		//让线程进入空闲链表之后处于阻塞，因为此时任务为空
		pool->lock();

		while (pool->queueIsEmpty())
		{
			pool->wait();
		}
		pool->moveToBusy(id);
		//取出任务
		CBaseTask* task = pool->popTask();

		pool->unlock();
		//work不能放在锁范围内，需要多线程并发执行
		task->work();

		pool->lock();
		pool->moveToIdle(id);
		pool->unlock();
	}
}
