#include "FileLogRecord.h"

FileLogRecord::FileLogRecord()
{
    time_t now = std::time(nullptr);

    char buffer[11];  // 格式: YYYY_MM_DD + 终止符
    std::strftime(buffer, sizeof(buffer), "%Y_%m_%d", std::localtime(&now));

    //生成日志文件夹
    char dirPath[50] = { "./log" };
    // 创建文件夹，权限设置为 0755（用户可读写执行，组和其他用户可读执行）
    int status = mkdir(dirPath, 0755);

    //追加方式打开或创建日志文件
    this->logFilePath = "./log/" + string(buffer) + ".log";
    this->logFile.open(this->logFilePath, ios::app | ios::out);
}

FileLogRecord::~FileLogRecord()
{
    if (logFile.is_open()) 
    {
        logFile.close();
    }
}

string FileLogRecord::getCurrentTime()
{
    time_t now = time(nullptr);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return string(buffer);
}

void FileLogRecord::logLogin(const string& username, const string& password, bool isSend)
{
    if (!logFile.is_open())
        return;

    string type = isSend ? "发送" : "接收";
    logFile << "时间：" << getCurrentTime() << "\n"
        << "功能：登录\n"
        << "类型：" << type << "\n"
        << "发送个人结构体数据包：\n"
        << "账号：" << username << "\n"
        << "密码:" << password << "\n\n";
    logFile.flush(); // 确保数据写入磁盘
}

void FileLogRecord::resultBack(bool flag, string function, string message, string user_account)
{
    string result = flag ? "成功" : "失败";
    logFile << "时间：" << getCurrentTime() << "\n"
        << "账号：" << user_account << "\n"
        << "功能：" << function << "\n"
        << "类型：发送" << "\n"
        << "发送返回结构体数据包：\n"
        << "功能执行结果：" << result << "\n"
        << "携带信息:" << message << "\n\n";
    logFile.flush(); // 确保数据写入磁盘
}

void FileLogRecord::sendCode(bool flag, string phone, string code)
{
    logFile << "时间：" << getCurrentTime() << "\n"
        << "功能：发送验证码" << "\n"
        << "类型：发送" << "\n"
        << "手机号：" << phone << "\n"
        << "发送返回结构体数据包：\n"
        << "功能执行结果：成功\n"
        << "携带验证码:" << code << "\n\n";
    logFile.flush(); // 确保数据写入磁盘
}

void FileLogRecord::queryRecord(string user_account, string number, string in_time, string out_time)
{
    logFile << "时间：" << getCurrentTime() << "\n"
        << "功能：查询车辆出入库记录" << "\n"
        << "类型：发送" << "\n"
        << "账号：" << user_account << "\n"
        << "发送车辆出入库记录数据包：\n"
        << "功能执行结果：成功\n"
        << "查询车牌:" << number << "\n"
        << "入库时间:" << in_time << "\n"
        << "出库时间:" << out_time << "\n\n";
    logFile.flush(); // 确保数据写入磁盘
}

void FileLogRecord::VideoInsert(VIDEO& videoInfo, string user_account)
{
    logFile << "时间：" << getCurrentTime() << "\n"
        << "功能：视频信息上传" << "\n"
        << "类型：接收" << "\n"
        << "账号：" << user_account << "\n"
        << "包含信息：\n"
        << "视频名称:" << videoInfo.video_name << "\n"
        << "视频总帧数:" << videoInfo.video_frame << "\n"
        << "视频路径:" << videoInfo.video_path << "\n\n";
    logFile.flush(); // 确保数据写入磁盘
}

void FileLogRecord::InsertRecord(INOUTRECORD record,string user_account)
{
    string function;
    record.type == 1 ? function = "上传入库记录" : function = "更新出库记录";

    

    logFile << "时间：" << getCurrentTime() << "\n"
        << "功能：" << function << "\n"
        << "类型：接收" << "\n"
        << "账号：" << user_account << "\n"
        << "包含信息：\n"
        << "车牌号:" << record.number << "\n"
        << "时间:" << record.time << "\n"
        << "地点:" << record.locate << "\n";

    if (record.type == 2)
    {
        logFile << "金额：" << record.cost << "\n\n";
    }
    else
    {
        logFile << "\n";
    }

    logFile.flush(); // 确保数据写入磁盘
}

void FileLogRecord::queryVideo(VIDEOPLAY videoPlay, string user_account)
{
    logFile << "时间：" << getCurrentTime() << "\n"
        << "功能：视频列表查询" << "\n"
        << "类型：接收" << "\n"
        << "账号：" << user_account << "\n"
        << "包含信息：\n"
        << "查询日期：" << videoPlay.date << "\n"
        << "查询页码:" << videoPlay.page << "\n\n";

    logFile.flush(); // 确保数据写入磁盘
}
