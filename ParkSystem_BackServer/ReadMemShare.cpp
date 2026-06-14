#include "ReadMemShare.h"


ReadMemShare::ReadMemShare()
{
	//初始化共享内存
	this->init_mem();
	//启动线程池，处理接收到的任务
	this->threadPool = new CBackThreadPool(4);
}

ReadMemShare::~ReadMemShare()
{

}
//共享内存初始化
void ReadMemShare::init_mem()
{
	this->shmaddr = nullptr;
	this->index = -1;
	//消息队列创建或访问
	this->msgid = msgget((key_t)1001, IPC_CREAT | 0777);
	if (msgid < 0)
	{
		perror("消息队列创建或访问失败！");
	}
	else
	{
		cout << "消息队列创建或访问成功" << endl;
	}

	//共享内存创建
	this->shmid = shmget((key_t)1002, sizeof(arr) + sizeof(SHAREMEM) * BLOCK_SIZE, IPC_CREAT | 0777);
	if (shmid < 0)
	{
		perror("共享内存创建或访问失败");
	}
	else
	{
		cout << "共享内存创建成功！" << endl;
	}

	//信号创建
	this->semid = sem_create((key_t)1003, BLOCK_SIZE);
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

int ReadMemShare::sem_create(key_t key, int num_sems)
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

void ReadMemShare::sem_setVal(int semid, int sem_num, int value)
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

void ReadMemShare::sem_p(int semid, int sem_index)
{
	struct sembuf buf = { sem_index,-1,SEM_UNDO };
	int res = semop(semid, &buf, 1);
	if (res < 0)
	{
		perror("semop error");
	}
}

void ReadMemShare::sem_v(int semid, int sem_index)
{
	struct sembuf buf = { sem_index,1,SEM_UNDO };	// 修复信号量 V 操作的值为 +1

	int res = semop(semid, &buf, 1);
	if (res < 0)
	{
		perror("semop error");
	}
}

void ReadMemShare::readMsg()
{

	while (1)
	{
		//接收消息队列的消息
		if (msgrcv(this->msgid, &(this->buf), sizeof(buf.mtext), 1, 0) == -1)
		{
			perror("msgrcv error");
		}
		else
		{
			cout << "接收到前置服务器发送来的消息" << endl;
		}

		this->index = atoi(this->buf.mtext);


		//读操作  先操作索引区
		this->shmaddr = shmat(this->shmid, NULL, 0);
		memcpy(&(this->arr[this->index]), this->shmaddr + sizeof(int) * this->index, sizeof(int));

		//读操作 操作数据区
		if (this->arr[this->index] == 1)
		{
			//信号量加锁
			this->sem_p(this->semid, this->index);

			SHAREMEM* recvMem = new SHAREMEM();

			memcpy(recvMem, this->shmaddr + sizeof(this->arr) + sizeof(SHAREMEM) * this->index, sizeof(SHAREMEM));

			//添加到任务队列
			this->threadPool->addRecvQueue(recvMem);

			//数据区清空
			memset(this->shmaddr + sizeof(this->arr) + sizeof(SHAREMEM) * this->index, 0, sizeof(SHAREMEM));

			//索引区置0
			this->arr[this->index] = 0;
			memcpy(this->shmaddr + sizeof(int) * this->index, &(this->arr[this->index]), sizeof(int));

			shmdt(this->shmaddr);
			//信号量解锁
			this->sem_v(this->semid, this->index);

			//重置缓冲区数据
			bzero(this->buf.mtext, sizeof(buf.mtext)); // 清空消息缓冲区
			bzero(this->arr, sizeof(this->arr)); // 清空索引数组
			this->index = -1; // 重置索引
		}
	}
}
