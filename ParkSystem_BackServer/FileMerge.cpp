#include "FileMerge.h"

FileMerge::FileMerge(string user_account, string fileName, int number, int totalLength,int fd)
{
	this->user_account = user_account;
	this->fileName = fileName;
	this->number = number;
	this->totalLength = totalLength;
	this->nowLength = 0;
	this->fd = fd;
}

FileMerge::~FileMerge()
{
	for (auto file : fileMap)
	{
		//释放内存
		free(file.second);
	}
}

void FileMerge::pushMap(FILEINFO& file)
{
	FILEINFO* fileInfo = new FILEINFO();
	memcpy(fileInfo, &file, sizeof(FILEINFO));

	fileMap[file.fileIndex] = fileInfo;
	this->nowLength += file.fileLength;
}

bool FileMerge::checkFile()
{
	//文件完整性标志位，默认文件完整
	bool flag = true;
	int index = 0;
	SHAREMEM* shareMem = new SHAREMEM();
	shareMem->fd = fd;
	FILEBACK fileBack;
	RESULTBACK resultBack;
	HEAD head;

	strcpy(fileBack.filename, this->fileName.c_str());
	

	cout << "收到尾包，检验文件完整性!" << endl;
	for (int i = 1; i <= this->number; i++)
	{
		//未找到该序号文件
		if (this->fileMap.count(i) == 0)
		{
			fileBack.arr[index++] = i;
			flag = false;
		}
	}

	if (flag)	//文件完整，开始拼接
	{
		bool ret = this->unionFile();

		head.bussinessType = FILECHECK_T;
		head.bussinessLength = sizeof(RESULTBACK);
		//文件保存成功
		if (ret)
		{
			resultBack.flag = 1;
			strcpy(resultBack.message, this->fileName.c_str());
			cout << "图片文件保存成功！ fileName = " << this->fileName << endl;
		}
		else  //文件保存失败
		{
			resultBack.flag = 2;
			strcpy(resultBack.message, this->fileName.c_str());
		}

		memcpy(shareMem->buf, &head, sizeof(HEAD));
		memcpy(shareMem->buf + sizeof(HEAD), &resultBack, sizeof(RESULTBACK));
		
	}
	else	//文件缺失，发送缺失返回体
	{
		head.bussinessLength = sizeof(FILEBACK);
		head.bussinessType = FILEBACK_T;

		memcpy(shareMem->buf, &head, sizeof(HEAD));
		memcpy(shareMem->buf + sizeof(HEAD), &fileBack, sizeof(FILEBACK));

		cout << "图片文件丢包返回客户端" << endl;
	}

	WriteMemShare::sendQueue.push(shareMem);

	return flag;
}

bool FileMerge::unionFile()
{
	//拼接文件路径
	string path = "./" + this->user_account + "/" + this->fileName;

	cout << "path = " << path << endl;

	//以二进制写入方式打开文件
	ofstream file(path, ios::binary | ios::out);
	if (file.is_open()) 
	{
		//文件拼接
		for (auto m : fileMap)
		{
			file.write(m.second->context, m.second->fileLength);
		}
		//关闭文件
		file.close();
		cout << "文件写入成功!" << endl;
		return true;
	}
	else {
		cerr << "文件打开失败" << std::endl;
		return false;
	}
}
