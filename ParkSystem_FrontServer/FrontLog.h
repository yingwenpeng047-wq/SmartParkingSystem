#pragma once
#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <pthread.h>
using namespace std;

// ANSI 转义序列宏定义
#define MOVE_UP(n)    "\033[" #n "A"  // 上移n行
#define MOVE_DOWN(n)  "\033[" #n "B"  // 下移n行
#define CLEAR_LINE    "\033[2K"       // 清除当前行
#define MOVE_TO_START "\r"            // 移到行首

//前置服务器实时日志类
class FrontLog
{
public:
	
	static FrontLog* instance;

	~FrontLog();

	//显示日志状态函数
	static void* showLog(void* arg);
	//单例模式
	static FrontLog* getInstance();

	//Get Set方法
	int getConnection();
	void setConnection(int connections);

	int getVideoList();
	void setVideoList(int videoList);

	int getVideoRecord();
	void setVideoRecord(int videoRecord);

	int getPictureFile();
	void setPictureFile(int pictureFile);

	int getVideoUpdate();
	void setVideoUpdate(int videoUpdate);

	int getReceived();
	void setReceived(int received);

	int getSent();
	void setSent(int sent);

	int getCarUpdate();
	void setCarUpdate(int carUpdate);

	int getCarSearch();
	void setCarSearch(int carSearch);

private:
	FrontLog();
	int connections;	//有效连接数
	int videoList;		//获取视频列表次数
	int videoRecord;	//上传播放记录次数
	int pictureFile;	//上传图片文件次数
	int videoUpdate;	//更新播放记录次数
	int received;		//接收数据包
	int sent;			//发送数据包
	int carUpdate;		//车辆记录更新
	int carSearch;		//车辆记录查询
};

