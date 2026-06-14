#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sys/stat.h>
#include <fcntl.h>
#include "protocol.h"
using namespace std;

class FileLogRecord
{
public:
    FileLogRecord();
    ~FileLogRecord();

    /// <summary>
    /// 获取本地时间
    /// </summary>
    /// <returns>返回本地时间 %Y-%m-%d %H:%M:%S </returns>
    string getCurrentTime();

    /// <summary>
    /// 记录登录请求日志
    /// </summary>
    /// <param name="userId">用户id</param>
    /// <param name="username">用户名</param>
    /// <param name="password">密码</param>
    /// <param name="isSend">true-发送 false-接收</param>
    void logLogin(const string& username, const string& password, bool isSend);

    /// <summary>
    /// 记录返回日志
    /// </summary>
    /// <param name="flag">返回结果类型 成功-true 失败-false</param>
    /// <param name="function">执行功能</param>
    /// <param name="message">携带信息</param>
    void resultBack(bool flag, string function, string message,string user_account);

    /// <summary>
    /// 验证码发送日志
    /// </summary>
    /// <param name="flag"></param>
    /// <param name="phone"></param>
    /// <param name="number"></param>
    void sendCode(bool flag, string phone, string number);

    /// <summary>
    /// 车辆出入库查询日志
    /// </summary>
    /// <param name="user_account">用户账号</param>
    /// <param name="number">车牌号码</param>
    /// <param name="in_time">出库时间</param>
    /// <param name="out_time">入库时间</param>
    void queryRecord(string user_account,string number,string in_time,string out_time);

    /// <summary>
    /// 视频插入请求日志
    /// </summary>
    /// <param name="videoInfo">视频信息结构体</param>
    /// <param name="user_account">用户账号</param>
    void VideoInsert(VIDEO& videoInfo,string user_account);

    /// <summary>
    /// 车辆出入库记录更新日志
    /// </summary>
    /// <param name="record">出入库记录结构体</param>
    /// <param name="user_account">用户账号</param>
    void InsertRecord(INOUTRECORD record, string user_account);

    /// <summary>
    /// 视频列表查询业务日志
    /// </summary>
    /// <param name="videoPlay">视频查询请求体</param>
    /// <param name="user_account">用户账号</param>
    void queryVideo(VIDEOPLAY videoPlay, string user_account);

private:
    string logFilePath;
    ofstream logFile;
};

