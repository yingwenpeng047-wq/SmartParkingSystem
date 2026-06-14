#pragma once
#include <map>
#include <iostream>
#include <fstream>
#include <string.h>
#include <malloc.h>
#include "protocol.h"
#include "WriteMemShare.h"
using namespace std;

//文件拼接类
class FileMerge
{
public:
	FileMerge(string user_account,string fileName,int number,int length,int fd);
	~FileMerge();
	//向map中添加文件碎片
	void pushMap(FILEINFO& file);

	//验证文件完整性
	bool checkFile();

	//整合文件碎片为图片
	bool unionFile();

private:
	map<int, FILEINFO*> fileMap;	//文件二进制内容map类  文件序号-文件碎片
	string user_account;			//文件归属用户账号
	string fileName;				//原文件名称
	int number;						//原文件碎片总个数
	int totalLength;				//原文件总字节数
	int nowLength;					//已接收到的文件总字节数
	int fd;							//客户端网络通道描述符
};

