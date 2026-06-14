#include "ReadMemShare.h"


ReadMemShare::ReadMemShare()
{
	pthread_t id;
	//创建读共享内存的线程
	pthread_create(&id, NULL, readMsg, this);
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
	this->msgid = msgget((key_t)1004, IPC_CREAT | 0777);
	if (msgid < 0)
	{
		perror("消息队列创建或访问失败！");
	}

	//共享内存创建，一个是索引区，一个是数据区
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

int ReadMemShare::sem_create(key_t key, int num_sems)
{
	int res = semget(key, num_sems, IPC_CREAT | 0777);
	if (res < 0)
	{
		perror("semget error");
		return 0;
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
	struct sembuf buf = { sem_index,1,SEM_UNDO };	//// 修复信号量 V 操作的值为 +1

	int res = semop(semid, &buf, 1);
	if (res < 0)
	{
		perror("semop error");
	}
}


//读取共享内存是在消息队列发送信息过后，才知道要读取哪一块的区域
void* ReadMemShare::readMsg(void *arg)
{
	ReadMemShare* readMem = (ReadMemShare*)arg;
	readMem->init_mem();
	//线程执行期间获取自身id
	pthread_t id = pthread_self();
	//系统记录此id，并在线程无后续操作时自动回收  并不是在此处回收
	pthread_detach(id);

	while (1)
	{
		//接收消息队列的消息,要读取哪个数据块
		if (msgrcv(readMem->msgid, &(readMem->buf), sizeof(buf.mtext), 1, 0) == -1)
		{
			perror("msgrcv error");
		}

		//要读取的是哪一个数据块，通过消息队列传输过来
		readMem->index = atoi(readMem->buf.mtext);

		//读操作  先操作索引区，创建共享内存块		
		readMem->shmaddr = shmat(readMem->shmid, NULL, 0);
		memcpy(&(readMem->arr[readMem->index]), readMem->shmaddr + sizeof(int) * readMem->index, sizeof(int));

		//读操作 操作数据区
		if (readMem->arr[readMem->index] == 1)
		{
			//信号量加锁，这个信号量数组的index位置上锁
			readMem->sem_p(readMem->semid, readMem->index);

			SHAREMEM* recvMem = new SHAREMEM();

			memcpy(recvMem, readMem->shmaddr + sizeof(readMem->arr) + sizeof(SHAREMEM) * readMem->index, sizeof(SHAREMEM));

			HEAD head;
			memcpy(&head, recvMem->buf, sizeof(HEAD));


			//添加到任务队列 交给发送线程发送
			WriteMemShare::sendData.push(recvMem);

			//数据区清空
			memset(readMem->shmaddr + sizeof(readMem->arr) + sizeof(SHAREMEM) * readMem->index, 0, sizeof(SHAREMEM));

			//索引区置0
			readMem->arr[readMem->index] = 0;
			memcpy(readMem->shmaddr + sizeof(int) * readMem->index, &(readMem->arr[readMem->index]), sizeof(int));

			shmdt(readMem->shmaddr);
			//信号量解锁
			readMem->sem_v(readMem->semid, readMem->index);

			//4.重置缓冲区数据
			bzero(readMem->buf.mtext, sizeof(buf.mtext)); // 清空消息缓冲区
			bzero(readMem->arr, sizeof(readMem->arr)); // 清空索引数组
			readMem->index = -1; // 重置索引
		}
	}
}
