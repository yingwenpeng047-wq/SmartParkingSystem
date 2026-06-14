#pragma once
#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <cstring>
#include <queue>
#include <string>
#include "protocol.h"
#include <stdlib.h>
#include <pthread.h>
#include "CBackThreadPool.h"
#define BLOCK_SIZE 100
using namespace std;

////信息结构体
//typedef struct messagebuf {
//    long mtype;      /* message type, must be > 0 */
//    char mtext[20]; /* message data */
//}MSGBUF;
//
//union semun {
//    int val;                    /* Value for SETVAL */
//    struct semid_ds* buf;       /* Buffer for IPC_STAT, IPC_SET */
//    unsigned short* array;      /* Array for GETALL, SETALL */
//    struct seminfo* _buf;       /* Buffer for IPC_INFO */
//};

//共享内存读取类
class ReadMemShare
{
public:
    ReadMemShare();
    ~ReadMemShare();

    //初始化成员变量
    void init_mem();
    //信号量创建
    int sem_create(key_t key, int num_sems);
    //信号量初始化值
    void sem_setVal(int semid, int sem_num, int value);
    //信号量P操作 -1
    void sem_p(int semid, int sem_index);
    //信号量V操作 +1
    void sem_v(int semid, int sem_index);

   void readMsg();

private:
    void* shmaddr;
    int index;
    int arr[BLOCK_SIZE];
    int semid;      //信号
    int msgid;      //消息队列
    int shmid;      //共享内存
    MSGBUF buf;
    CBackThreadPool* threadPool;
};

