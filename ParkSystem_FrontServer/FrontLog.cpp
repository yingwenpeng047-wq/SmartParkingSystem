#include "FrontLog.h"

FrontLog* FrontLog::instance = nullptr;

FrontLog::FrontLog()
{
    this->connections = 0;
    this->videoList = 0;
    this->videoUpdate = 0;
    this->videoRecord = 0;
    this->pictureFile = 0;
    this->received = 0;
    this->sent = 0;
    this->carSearch = 0;
    this->carUpdate = 0;

    pthread_t id;
    pthread_create(&id, NULL, showLog, this);

}

FrontLog::~FrontLog()
{
}

void* FrontLog::showLog(void *arg)
{
    FrontLog* frontLog = (FrontLog*)arg;

    //线程执行期间获取自身id
    pthread_t id = pthread_self();
    //系统记录此id，并在线程无后续操作时自动回收
    pthread_detach(id);

    char buffer[20];  // 时间缓冲区

    // 打印静态面板
    printf("\n=============================== 系统监控面板 ================================\n\n");
    printf("有效连接数：%d\n", frontLog->connections);
    printf("接收数据包：%d      |      发送数据包：%d\n\n", frontLog->received, frontLog->sent);
    printf("获取视频列表：%d    |      上传播放记录：%d      |      更新播放记录：%d\n", frontLog->videoList, frontLog->videoRecord, frontLog->videoUpdate);
    printf("上传图片数量：%d    |      车辆记录更新：%d      |      车辆记录查询：%d\n\n", frontLog->pictureFile, frontLog->carUpdate, frontLog->carSearch);
    printf("当前系统时间：%s\n", buffer);
    printf("=============================================================================\n");

    while (1) {
        // 更新时间
        time_t now = time(nullptr);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H-%M-%S", localtime(&now));


        printf(MOVE_UP(8));  

        // 更新有效连接数（第2行）
        printf(MOVE_TO_START CLEAR_LINE);
        printf("有效连接数：%d\n", frontLog->connections);

        // 更新接收/发送数据包（第3行）
        printf(MOVE_TO_START CLEAR_LINE);
        printf("接收数据包：%d      |      发送数据包：%d\n\n", frontLog->received, frontLog->sent);

        // 更新视频相关数据（第4行）
        printf(MOVE_TO_START CLEAR_LINE);
        printf("获取视频列表：%d    |      上传播放记录：%d      |      更新播放记录：%d\n", frontLog->videoList, frontLog->videoRecord, frontLog->videoUpdate);

        // 更新图片和车辆数据（第5行）
        printf(MOVE_TO_START CLEAR_LINE);
        printf("上传图片数量：%d    |      车辆记录更新：%d      |      车辆记录查询：%d\n\n", frontLog->pictureFile, frontLog->carUpdate, frontLog->carSearch);

        // 更新系统时间（第6行）
        printf(MOVE_TO_START CLEAR_LINE);
        printf("当前系统时间：%s\n", buffer);

        // 移动光标到底部分隔线行
        printf(MOVE_DOWN(1));

        fflush(stdout);
        sleep(1);
    }
}

FrontLog* FrontLog::getInstance()
{
    if (FrontLog::instance == nullptr)
    {
        instance = new FrontLog();
    }

    return instance;
}

int FrontLog::getConnection()
{
    return this->connections;
}

void FrontLog::setConnection(int connections)
{
    if (connections < 0)
    {
        this->connections = 0;
    }
    else
    {
        this->connections = connections;
    }
}

int FrontLog::getVideoList()
{
    return this->videoList;
}

void FrontLog::setVideoList(int videoList)
{
    this->videoList = videoList;
}

int FrontLog::getVideoRecord()
{
    return this->videoRecord;
}

void FrontLog::setVideoRecord(int videoRecord)
{
    this->videoRecord = videoRecord;
}

int FrontLog::getPictureFile()
{
    return this->pictureFile;
}

void FrontLog::setPictureFile(int pictureFile)
{
    this->pictureFile = pictureFile;
}

int FrontLog::getVideoUpdate()
{
    return this->videoUpdate;
}

void FrontLog::setVideoUpdate(int videoUpdate)
{
    this->videoUpdate = videoUpdate;
}

int FrontLog::getReceived()
{
    return this->received;
}

void FrontLog::setReceived(int received)
{
    this->received = received;
}

int FrontLog::getSent()
{
    return this->sent;
}

void FrontLog::setSent(int sent)
{
    this->sent = sent;
}

int FrontLog::getCarUpdate()
{
    return this->carUpdate;
}

void FrontLog::setCarUpdate(int carUpdate)
{
    this->carUpdate = carUpdate;
}

int FrontLog::getCarSearch()
{
    return this->carSearch;
}

void FrontLog::setCarSearch(int carSearch)
{
    this->carSearch = carSearch;
}

