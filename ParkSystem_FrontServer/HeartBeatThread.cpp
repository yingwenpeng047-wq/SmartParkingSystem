#include "HeartBeatThread.h"

extern unordered_map<int, TimePoint> custom_fd;

HeartBeatThread::HeartBeatThread()
{
    pthread_t id;
    pthread_create(&id, NULL, doCheck, this);
}

HeartBeatThread::~HeartBeatThread()
{
}

void* HeartBeatThread::doCheck(void* arg)
{
    HeartBeatThread* heartThread = (HeartBeatThread*)arg;
    while (1) 
    {
        TimePoint time = Clock::now();
        int second = 0;
        for (auto m : custom_fd)
        {
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(time - m.second);
            second = duration.count();

            //心跳超时
            if (second > 300)
            {
                HEAD head;
                head.bussinessType = LIFE_T;
                head.bussinessLength = m.first;

                SHAREMEM* shareMem = new SHAREMEM();
                shareMem->fd = m.first;
                memcpy(shareMem->buf, &head, sizeof(HEAD));

                CEpollServer::delCustom(m.first);

                //通知后置进行数据删除
                WriteMemShare::sendQueue.push(shareMem);
            }
        }

        // 等待4.5分钟
        for (int i = 0; i < 270; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}
