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
#include <pthread.h>
#include <mutex>
#include "FrontLog.h"
#define BLOCK_SIZE 100
using namespace std;


//共享内存写入类
class WriteMemShare
{
public:
    WriteMemShare();
    ~WriteMemShare();
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
    //发送消息
    static void* sendMsg(void* arg);
    //向共享内存发送消息
    void sendToMemshare();
    //向客户端发送消息
    void sendToClient();
    //添加发送消息
    void pushQueue();
    //删除已发送的消息
    SHAREMEM* popQueue();
    void endThread();
    //待发送到共享内存的消息队列
    static queue<SHAREMEM*> sendQueue;
    //待发送到客户端的消息队列
    static queue<SHAREMEM*>sendData;
private:

    void* shmaddr;
    int index;
    int arr[BLOCK_SIZE];
    int semid;      //信号
    int msgid;      //消息队列
    int shmid;      //共享内存
    bool flag;      //控制线程结束
    MSGBUF buf;
    FrontLog* frontLog;

};

