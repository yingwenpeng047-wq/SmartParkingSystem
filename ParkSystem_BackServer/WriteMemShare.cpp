#include "WriteMemShare.h"

queue<SHAREMEM*> WriteMemShare::sendQueue;
WriteMemShare::WriteMemShare()
{
	pthread_t id;
	pthread_create(&id, NULL, sendMsg, this);
	cout << "写入线程创建成功！" << endl;
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
	this->msgid = msgget((key_t)1004, IPC_CREAT | 0777);
	if (msgid < 0)
	{
		perror("消息队列创建或访问失败！");
	}

	//共享内存创建
	this->shmid = shmget((key_t)1005, sizeof(arr) + sizeof(SHAREMEM) * BLOCK_SIZE, IPC_CREAT | 0777);
	if (shmid < 0)
	{
		perror("共享内存创建或访问失败");
	}
	
	//信号创建
	this->semid = sem_create((key_t)1006, BLOCK_SIZE);
	if (semid <= 0)
	{
		perror("信号量创建失败");
	}
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
	else
	{
		cout << "信号量创建成功！" << endl;
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
		if (wirteMem->sendQueue.empty())
		{
			continue;
		}

		//读索引区，判断哪个位置为0可写
		wirteMem->shmaddr = shmat(wirteMem->shmid, nullptr, 0);
		memcpy(wirteMem->arr, wirteMem->shmaddr, sizeof(arr));

		for (int i = 0; i < BLOCK_SIZE; i++)
		{
			if (wirteMem->arr[i] == 0)
			{
				wirteMem->index = i;
				break;
			}
		}

		//验证index是否找到可写的位置下标
		if (wirteMem->index == -1)
		{
			shmdt(wirteMem->shmaddr);
			sleep(1);
			continue;
		}
		//取出数据
		SHAREMEM* data = wirteMem->popQueue();

		// 计算CRC:包头(不含CRC) + 包体
		((HEAD*)(data->buf))->crc = 0;
		unsigned int crc = CBaseTask::crc32(0xFFFFFFFF, (unsigned char*)(data->buf), sizeof(HEAD) + ((HEAD*)(data->buf))->bussinessLength);
		((HEAD*)(data->buf))->crc = htonl(crc);


		//信号量加锁
		wirteMem->sem_p(wirteMem->semid, wirteMem->index);
		//操作数据区
		memcpy(wirteMem->shmaddr + sizeof(wirteMem->arr) + sizeof(SHAREMEM) * wirteMem->index, data, sizeof(SHAREMEM));
		//操作索引区
		wirteMem->arr[wirteMem->index] = 1;
		memcpy(wirteMem->shmaddr + sizeof(int) * wirteMem->index, &(wirteMem->arr[wirteMem->index]), sizeof(int));
		shmdt(wirteMem->shmaddr);
		//信号量解锁
		wirteMem->sem_v(wirteMem->semid, wirteMem->index);

		//释放资源
		delete data;

		//发消息队列通知后置
		wirteMem->buf.mtype = 1; // 设置消息类型为1
		sprintf(wirteMem->buf.mtext, "%d", wirteMem->index);

		if (msgsnd(wirteMem->msgid, &(wirteMem->buf), sizeof(wirteMem->buf.mtext), 0) == -1)
		{
			perror("msgsnd error");
		}
		else
		{
			cout << "消息队列发送消息成功 index = " << wirteMem->index << endl;
		}

		//5.清空所有数据
		bzero(&(wirteMem->buf), sizeof(MSGBUF));
		bzero(&(wirteMem->arr[wirteMem->index]), sizeof(int));
		wirteMem->index = -1; // 重置索引
	}
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
