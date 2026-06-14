#include "WriteMemShare.h"

//有俩队列，一个是要发送到共享内存的队列，一个是要发给客户端的队列
queue<SHAREMEM*> WriteMemShare::sendQueue;
queue<SHAREMEM*> WriteMemShare::sendData;
WriteMemShare::WriteMemShare()
{
	this->frontLog = FrontLog::getInstance();
	pthread_t id;
	pthread_create(&id, NULL, sendMsg, this);
}

WriteMemShare::~WriteMemShare()
{

}

//初始化成员变量
void WriteMemShare::init_mem()
{
	this->flag = true;
	this->shmaddr = nullptr;
	this->index = -1;
	//消息队列创建或访问
	this->msgid = msgget((key_t)1001, IPC_CREAT | 0777);
	if (msgid < 0)
	{
		perror("消息队列创建或访问失败！");
	}

	//共享内存创建
	this->shmid = shmget((key_t)1002, sizeof(arr) + sizeof(SHAREMEM) * BLOCK_SIZE, IPC_CREAT | 0777);
	if (shmid < 0)
	{
		perror("共享内存创建或访问失败");
	}
	
	//信号量创建
	this->semid = sem_create((key_t)1003, BLOCK_SIZE);

	// 初始化所有信号量为1（表示资源可用）
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		sem_setVal(semid, i, 1);
		arr[i] = 0;
	}
}

// 信号量的初始化函数 参数：key就是创建信号  num就是信号量数组长度
int WriteMemShare::sem_create(key_t key, int num_sems)
{
	int res = semget(key, num_sems, IPC_CREAT | 0777);
	if (res < 0)
	{
		perror("semget error");
		return 0;
	}

	return res;
}

// 信号量初始化操作
// 参数semid: 信号量的id
// 参数sem_num:要设置的信号量数组的下标元素
// 参数value:要设置的信号量的值
void WriteMemShare::sem_setVal(int semid, int sem_num, int value)
{
	//准备数据
	union semun arg;
	arg.val = value;	// // 初始化 arg.val 以避免未初始化变量错误
	int res = semctl(semid, sem_num, SETVAL, arg);

	if (res < 0)
	{
		perror("semctl error");
	}
}

// 信号量P操作 -1   参数semid：对应信号量id      参数sem_index：你要做-1操作的信号量数组的下标元素
void WriteMemShare::sem_p(int semid, int sem_index)
{
	struct sembuf buf = { sem_index,-1,SEM_UNDO };
	int res = semop(semid, &buf, 1);
	if (res < 0)
	{
		perror("semop error");
	}
}

// 信号量V操作 +1
void WriteMemShare::sem_v(int semid, int sem_index)
{
	struct sembuf buf = { sem_index,1,SEM_UNDO };	//// 修复信号量 V 操作的值为 +1

	int res = semop(semid, &buf, 1);
	if (res < 0)
	{
		perror("semop error");
	}
}
//发送消息
void* WriteMemShare::sendMsg(void* arg)
{
	WriteMemShare* wirteMem = (WriteMemShare*)arg;
	wirteMem->init_mem();
	//线程执行期间获取自身id
	pthread_t id = pthread_self();
	//系统记录此id，并在线程无后续操作时自动回收  并不是在此处回收
	pthread_detach(id);

	while (wirteMem->flag)
	{
		if (!wirteMem->sendQueue.empty())
		{
			wirteMem->sendToMemshare();
		}
		if (!wirteMem->sendData.empty())
		{
			wirteMem->sendToClient();
		}
	}
}

void WriteMemShare::sendToClient()
{
	//处理发送事件
	if (!WriteMemShare::sendData.empty())
	{
		HEAD head;
		
		SHAREMEM* send = WriteMemShare::sendData.front();
		memcpy(&head, send->buf, sizeof(HEAD));

		int res = write(send->fd, send->buf, sizeof(HEAD) + head.bussinessLength);
		WriteMemShare::sendData.pop();
		delete send;

		this->frontLog = FrontLog::getInstance();
		this->frontLog->setSent(frontLog->getSent() + 1);
	}
}

void WriteMemShare::sendToMemshare()
{
	//读索引区，判断哪个位置为0可写
	this->shmaddr = shmat(this->shmid, nullptr, 0);
	memcpy(this->arr, this->shmaddr, sizeof(arr));

	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		if (this->arr[i] == 0)
		{
			this->index = i;
			break;
		}
	}
	
	//验证index是否找到可写的位置下标
	if (this->index == -1)
	{
		shmdt(this->shmaddr);
	}
	//取出数据
	SHAREMEM* data = this->popQueue();
	//信号量加锁
	this->sem_p(this->semid, this->index);
	//操作数据区
	memcpy(this->shmaddr + sizeof(this->arr) + sizeof(SHAREMEM) * this->index, data, sizeof(SHAREMEM));
	//操作索引区
	this->arr[this->index] = 1;
	memcpy(this->shmaddr + sizeof(int) * this->index, &(this->arr[this->index]), sizeof(int));
	shmdt(this->shmaddr);
	//信号量解锁
	this->sem_v(this->semid, this->index);

	//释放资源
	delete data;

	//发消息队列通知后置
	this->buf.mtype = 1; // 设置消息类型为1
	sprintf(this->buf.mtext, "%d", this->index);

	if (msgsnd(this->msgid, &(this->buf), sizeof(this->buf.mtext), 0) == -1)
	{
		perror("msgsnd error");
	}

	//5.清空所有数据
	bzero(&(this->buf), sizeof(MSGBUF));
	bzero(&(this->arr[this->index]), sizeof(int));
	this->index = -1; // 重置索引
}

void WriteMemShare::pushQueue()
{
	
}

SHAREMEM* WriteMemShare::popQueue()
{
	//取出队头任务
	SHAREMEM* data = this->sendQueue.front();
	//删除队头任务
	this->sendQueue.pop();

	return data;
}

void WriteMemShare::endThread()
{
	this->flag = false;
}
