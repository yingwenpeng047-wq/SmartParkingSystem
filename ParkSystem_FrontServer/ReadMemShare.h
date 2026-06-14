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
#include "CEpollServer.h"
#include "WriteMemShare.h"
#define BLOCK_SIZE 100
using namespace std;

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
    //读取共享内存
    static void* readMsg(void* arg);

private:
    void* shmaddr;
    int index;              //要读取的是哪一个数据块，数组arr的下标索引是多少
    int arr[BLOCK_SIZE];    //记录每一个数据块的状态，用于判断是否可读，临时副本
    int semid;      //信号
    int msgid;      //消息队列
    int shmid;      //共享内存
    MSGBUF buf;
};

