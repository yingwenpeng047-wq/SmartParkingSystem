#pragma once
#include "CBaseTask.h"
#include "protocol.h"
#include <string.h>
#include <iostream>
#include "OperatorFactory.h"
#include "DBConnection.h"
#include "SendCode.h"
#include "FileMerge.h"
#include <sys/stat.h>
#include "FileLogRecord.h"
#include <fcntl.h>
using namespace std;

class CDataTask:public CBaseTask
{
public:
	CDataTask(SHAREMEM *recvMem);
	~CDataTask();

	//设置待解析的数据信息
	void setShareMem(SHAREMEM* shareMem);
	//获取待解析的数据信息
	SHAREMEM& getShareMem();

	//执行任务函数
	virtual void work() override;

	////执行CRC验证
	//void createCRC();

	/// <summary>
	/// 登录业务操作 用于向数据库查找用户信息
	/// 从shareMem解析出请求体信息，当中包含用户信息
	/// 登录成功则向日志表插入数据
	/// </summary>
	void doLogin();

	/// <summary>
	/// 注册业务操作 通过检查验证码字段是否为空决定是否调用短信发送接口
	/// 验证码为空，通过手机号调用短信接口
	/// 验证码不为空，校验验证码是否可进行注册
	/// </summary>
	void doRegister();

	/// <summary>
	/// 车辆出入场记录查询 解析出请求体，通过车牌号、入库时间、出库时间
	/// 查询对应记录，并写入数据库日志，向前置服务器返回数据
	/// </summary>
	void doSearchRecord();

	/// <summary>
	/// 根据客户端发来的文件碎片，判断是否要新建文件，还是其他已有的文件的碎片
	/// 将文件碎片写入文件类中，等待客户端文件确认发送完成进行拼接
	/// </summary>
	void doFileInfo();

	/// <summary>
	/// 客户端确认文件发送完成，后置服务器进行文件完整性校验，并返回丢包序号或
	/// 文件保存成功与失败的信息
	/// </summary>
	void doEmerge();

	/// <summary>
	/// 执行车辆出入库记录插入到数据库
	/// 入场记录插入基本都可以成功，出场记录插入可能失败
	/// 失败情况为无入场记录，向服务器发送RESULTBACK结构体说明情况
	/// </summary>
	void doInsertRecord();

	/// <summary>
	/// 视频记录插入，当客户端录制好视频之后，上传的视频记录进入数据库插入
	/// </summary>
	void doInsertVideo();

	/// <summary>
	/// 视频列表日期查询，根据客户端上传的不同头类型进行查询
	/// </summary>
	void doQueryDate(protocolType type);

	/// <summary>
	/// 执行视频列表查询业务
	/// </summary>
	void doQueryVideo();

	/// <summary>
	/// 执行视频播放记录更新，更新视频播放位置帧数
	/// </summary>
	void doVideoUpdate();

	static FileLogRecord* fileLog;
private:
	SHAREMEM shareMem;
	static map<string, FileMerge*> fileMap;

};

