#include "CBackThreadPool.h"

CBackThreadPool::CBackThreadPool(const int num)
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

	cout << "后置线程池创建成功" << endl;
}

CBackThreadPool::~CBackThreadPool()
{
}

void CBackThreadPool::lock()
{
	pthread_mutex_lock(&(this->mutex));
}

void CBackThreadPool::unlock()
{
	pthread_mutex_unlock(&(this->mutex));
}

void CBackThreadPool::wait()
{
	//注意第二个参数必须是上锁之后的线程互斥量
	pthread_cond_wait(&(this->cond), &(this->mutex));
}

void CBackThreadPool::wakeup()
{
	pthread_cond_signal(&(this->cond));
}

void CBackThreadPool::addRecvQueue(SHAREMEM *recvMem)
{
	CBaseTask* task = new CDataTask(recvMem);
	this->taskQueue.push(task);

	//释放资源
	delete(recvMem);
	//唤醒线程
	this->wakeup();
}

CBaseTask* CBackThreadPool::popTask()
{
	//从队头取任务
	CBaseTask* task = this->taskQueue.front();
	//从队列删除队头任务
	this->taskQueue.pop();
	return task;
}

bool CBackThreadPool::queueIsEmpty()
{
	return this->taskQueue.empty();
}

void CBackThreadPool::moveToIdle(pthread_t id)
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

void CBackThreadPool::moveToBusy(pthread_t id)
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

void CBackThreadPool::addThread()
{
	if (this->busyList.size() < this->threadMaxNum)
	{
		//如果没有空闲线程，并且忙碌线程数小于最大线程数，则创建新线程
		pthread_t id;
		pthread_create(&id, NULL, thread_function, this);
		this->idleList.push_back(id);
	}
	else
	{
		//如果没有空闲线程，并且忙碌线程数已经达到最大线程数，则不创建新线程
		cout << "当前忙碌线程数已达到最大值，无法创建新线程处理任务" << endl;
	}
}

void CBackThreadPool::delThread()
{
	//如果空闲线程数大于最小线程数，则删除空闲线程
	while (this->idleList.size() > this->threadMinNum - 1)
	{
		pthread_t idleThread = this->idleList.front();
		this->idleList.pop_front();
		pthread_cancel(idleThread); //取消线程
		cout << "已删除空闲线程" << endl;
	}
}

void* CBackThreadPool::thread_function(void* arg)
{
	CBackThreadPool* pool = (CBackThreadPool*)arg;
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

		//所有线程均忙碌，调用新增线程函数
		//if (pool->idleList.size() == 0)
		//{
		//	pool->addThread();
		//}

		pool->unlock();

		//work不能放在锁范围内，需要多线程并发执行
		task->work();
		delete task;
		pool->lock();

		//所有线程均为空闲线程，判断是否清理多余线程
		//if (pool->busyList.size() == 1)
		//{
		//	//注意当前线程不可删除，所以先保留threadMinNum-1个空闲线程 再把当前线程转为空闲线程
		//	pool->delThread();
		//}

		pool->moveToIdle(id);
		pool->unlock();
	}
}
