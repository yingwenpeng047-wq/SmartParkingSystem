#include "CDataTask.h"

FileLogRecord* CDataTask::fileLog = new FileLogRecord();
extern unordered_map<int, User*> userMap;
map<string, FileMerge*> CDataTask::fileMap;

CDataTask::CDataTask(SHAREMEM *recvMem)
{
	memcpy(&this->shareMem, recvMem, sizeof(SHAREMEM));
}

CDataTask::~CDataTask()
{

}

void CDataTask::setShareMem(SHAREMEM* shareMem)
{
	memcpy(&(this->shareMem), shareMem, sizeof(SHAREMEM));
}

SHAREMEM& CDataTask::getShareMem()
{
	return this->shareMem;
}

void CDataTask::work()
{
	//取出队头
	HEAD head;
	memcpy(&head, this->shareMem.buf, sizeof(HEAD));

	switch (head.bussinessType)
	{
		//登录业务
		case USER_T:doLogin(); break;
		//注册业务
		case REGISTER_T:doRegister(); break;
		//车辆出入库记录查询业务
		case INFOSEARCH_T:doSearchRecord(); break;
		//文件碎片存储业务
		case FILEINFO_T:doFileInfo(); break;
		//文件碎片拼接业务
		case FILECHECK_T:doEmerge(); break;
		//车辆出入库记录插入业务
		case INRECORD_T:doInsertRecord(); break;
		//视频信息插入业务
		case VIDEOPLAY_T:doInsertVideo(); break;
		//日期查询业务
		case VIDEODAY_T:doQueryDate(VIDEODAY_T); break;
		//按月查询业务
		case VIDEOMONTH_T:doQueryDate(VIDEOMONTH_T); break;
		//视频列表查询
		case VIDEOBACK_T:doQueryVideo(); break;
		//视频播放更新
		case VIDEOUPDATE_T:doVideoUpdate(); break;
		//心跳超时数据删除
		case LIFE_T:userMap.erase(head.bussinessLength); cout << "客户端下线 fd = " << head.bussinessLength << endl; break;

	}
}

void CDataTask::doLogin()
{
	cout << "执行登录业务" << endl;

	USER user;
	//取出请求体
	memcpy(&user, this->shareMem.buf + sizeof(HEAD), sizeof(USER));
	//获取数据库实例
	OperatorFactory* of = OperatorFactory::getInstance();
	//获取用户操作类实例
	UserOperator* uo = (UserOperator*)(of->createRepository(OperatorFactory::USER));
	//获取日志操作类实例
	LogOperator* log = (LogOperator*)(of->createRepository(OperatorFactory::LOG));
	//获取出入库操作类实例
	RecordOperator* ro = (RecordOperator*)(of->createRepository(OperatorFactory::CARRECORD));
	//请求包日志写入
	CDataTask::fileLog->logLogin(user.account, user.pwd, false);

	//执行用户登录
	string user_name = uo->loginUser(user,this->shareMem.fd);

	SHAREMEM* sendData = new SHAREMEM();
	bzero(sendData, sizeof(SHAREMEM));

	//创建返回头
	HEAD head;
	bzero(&head, sizeof(HEAD));


	//创建返回体
	RESULTBACK result;
	bzero(&result, sizeof(RESULTBACK));
	TABLES table;
	bzero(&table, sizeof(TABLES));

	if (user_name.empty())
	{
		result.flag = 2;
		user_name = "登录失败！账号或密码错误！";
		head.bussinessLength = sizeof(RESULTBACK);
		head.bussinessType = USER_T;
		memcpy(sendData->buf, &head, sizeof(HEAD));
		memcpy(result.message, user_name.c_str(), user_name.length());
		memcpy(sendData->buf + sizeof(HEAD), &result, sizeof(RESULTBACK));
	}
	else
	{
		head.bussinessType = UPDATETABLE_T;
		table = ro->queryInRecord(userMap[this->shareMem.fd]->getUserID());
		head.bussinessLength = sizeof(TABLES);
		memcpy(sendData->buf, &head, sizeof(HEAD));
		memcpy(sendData->buf + sizeof(HEAD), &table, sizeof(TABLES));
		CDataTask::fileLog->logLogin(user.account, user.pwd, true);
		//写入数据库日志
		log->insertLog(userMap[this->shareMem.fd]->getUserID(), "登录");
	}

	
	//写入文件日志信息
	CDataTask::fileLog->resultBack(!user_name.empty(), "登录", user_name, user.account);

	sendData->fd = this->shareMem.fd;

	WriteMemShare::sendQueue.push(sendData);
}

void CDataTask::doRegister()
{
	cout << "执行注册业务" << endl;
	//获取数据库实例
	OperatorFactory* of = OperatorFactory::getInstance();
	//获取用户操作类实例
	UserOperator* uo = (UserOperator*)(of->createRepository(OperatorFactory::USER));

	REGISTER registe;
	//取出请求体
	memcpy(&registe, this->shareMem.buf + sizeof(HEAD), sizeof(REGISTER));
	//获取手机号
	string user_account = registe.account;
	
	//创建请求头和返回体
	HEAD head;
	RESULTBACK result;

	head.bussinessType = REGISTER_T;
	head.bussinessLength = sizeof(RESULTBACK);

	//创建待发送数据
	SHAREMEM* sendData = new SHAREMEM();

	bzero(&result, sizeof(RESULTBACK));
	bzero(sendData, sizeof(SHAREMEM));

	//请求头拷贝
	memcpy(sendData->buf, &head, sizeof(HEAD));
	//客户端文件描述符赋值
	sendData->fd = this->shareMem.fd;

	//查询手机号是否被注册过
	bool ret = uo->queryUser(user_account);

	//查询到手机号已被注册
	if (ret == true)
	{
		result.flag = 2;	//1-成功 2-失败
		strcpy(result.message, "手机号已被注册！");
		CDataTask::fileLog->resultBack(false, "注册", result.message,user_account);
	}
	//用户名为空，判断执行发送短信
	else if (strlen(registe.name) == 0)
	{
		cout << "请求发送验证码" << endl;

		result.flag = 1;	//1-成功 2-失败
		SendCode* sendCode = new SendCode(registe.account);
		string code = sendCode->doWork();

		CDataTask::fileLog->sendCode(true, user_account, code);

		//拷贝验证码
		strcpy(result.message, code.c_str());

		if (code.empty())
		{
			result.flag = 2;	//1-成功 2-失败
			strcpy(result.message, "短信发送失败！服务器服务不可用！");
			CDataTask::fileLog->resultBack(false, "注册", result.message,"-1");
		}
	}
	else   //用户名不为空，执行注册逻辑
	{
		cout << "收到注册请求，执行注册逻辑" << endl;
		
		bool ret = uo->registerUser(registe.account, registe.name);

		if (ret == true)
		{
			result.flag = 1;	//1-成功 2-失败
			strcpy(result.message, "注册成功，初始密码为123456！");
			cout << "注册成功！" << endl;
			CDataTask::fileLog->resultBack(true, "注册", result.message,registe.account);
			//生成用户文件夹
			char dirPath[50] = { 0 };

			sprintf(dirPath, "./%s", registe.account);
			// 创建文件夹，权限设置为 0755（用户可读写执行，组和其他用户可读可写）
			int status = mkdir(dirPath, 0766);
		}
		else
		{
			result.flag = 2;	//1-成功 2-失败
			strcpy(result.message, "注册失败，手机号已被注册！");
			CDataTask::fileLog->resultBack(false, "注册", result.message,"-1");
		}
	}

	//返回体拷贝
	memcpy(sendData->buf + sizeof(HEAD), &result, sizeof(RESULTBACK));

	WriteMemShare::sendQueue.push(sendData);
}

void CDataTask::doSearchRecord()
{
	cout << "执行查询车辆出入库记录业务" << endl;
	//获取数据库实例
	OperatorFactory* of = OperatorFactory::getInstance();
	//获取查询操作类实例
	RecordOperator* ro = (RecordOperator*)(of->createRepository(OperatorFactory::CARRECORD));
	//获取日志操作类实例
	LogOperator* lo = (LogOperator*)(of->createRepository(OperatorFactory::LOG));


	//取出请求体
	INFOSEARCH infoSearch;
	memcpy(&infoSearch, this->shareMem.buf + sizeof(HEAD), sizeof(INFOSEARCH));

	//查询车辆信息
	INFOBACK InfoBack = ro->queryRecord(infoSearch.number, infoSearch.in_time, infoSearch.out_time,infoSearch.page,userMap[this->shareMem.fd]->getUserID());
	//写入数据库日志
	lo->insertLog(userMap[this->shareMem.fd]->getUserID(), "查询车辆出入库记录");

	//创建返回信息结构体
	SHAREMEM* sendData = new SHAREMEM();
	bzero(sendData, sizeof(SHAREMEM));
	HEAD head;
	
	head.bussinessLength = sizeof(CARINFO);
	head.bussinessType = INFOBACK_T;

	sendData->fd = this->shareMem.fd;
	memcpy(sendData->buf, &head, sizeof(HEAD));
	memcpy(sendData->buf + sizeof(HEAD), &InfoBack, sizeof(INFOBACK));

	//写入文件日志
	CDataTask::fileLog->queryRecord(userMap[this->shareMem.fd]->getUserAccount(), infoSearch.number, infoSearch.in_time, infoSearch.out_time);
	//加入待发送消息队列
	WriteMemShare::sendQueue.push(sendData);
}

void CDataTask::doFileInfo()
{
	FILEINFO fileInfo;

	bzero(&fileInfo, sizeof(FILEINFO));
	cout << "执行文件碎片接收" << endl;
	//取出请求体
	memcpy(&fileInfo, this->shareMem.buf + sizeof(HEAD), sizeof(FILEINFO));

	//该文件类实例已存在
	if (this->fileMap.count(fileInfo.fileName) > 0)
	{
		fileMap[fileInfo.fileName]->pushMap(fileInfo);
	}
	else	//该文件类不存在 新建文件类
	{
		FileMerge* fileMerge = new FileMerge(fileInfo.user_account, fileInfo.fileName, fileInfo.number, fileInfo.length,this->shareMem.fd);
		fileMerge->pushMap(fileInfo);
		fileMap[fileInfo.fileName] = fileMerge;
	}
}

void CDataTask::doEmerge()
{
	//延时500ms 确保已接收到所有文件碎片
	usleep(500 * 1000);
	FILECHECK fileCheck;

	bzero(&fileCheck, sizeof(FILECHECK));
	//取出请求体
	memcpy(&fileCheck, this->shareMem.buf + sizeof(HEAD), sizeof(FILECHECK));

	cout << "fileCheck.fileName = " << fileCheck.fileName << endl;

	if (fileMap.count(fileCheck.fileName) == 0)
	{
		cout << "未找到该文件！" << endl;
		cout << "fileMap.size() = " << fileMap.size() << endl;
		return;
	}

	bool ret = fileMap[fileCheck.fileName]->checkFile();

	if (ret)
	{
		//文件拼接已完成 清理内存
		delete fileMap[fileCheck.fileName];
		fileMap.erase(fileCheck.fileName);
		CDataTask::fileLog->resultBack(true, "图片上传", fileCheck.fileName,userMap[this->shareMem.fd]->getUserAccount());
		string operate = fileCheck.fileName;

		//插入图片表数据
		OperatorFactory* of = OperatorFactory::getInstance();
		PictureOperator* po = (PictureOperator*)of->createRepository(OperatorFactory::PICTURECORED);
		LogOperator* log = (LogOperator*)of->createRepository(OperatorFactory::LOG);
		cout << "shareMem.fd = " << this->shareMem.fd << endl;
		cout << "userId = " << userMap[this->shareMem.fd]->getUserID() << endl;
		
		po->insertPicture(fileCheck.fileName, fileCheck.cpath, this->shareMem.fd);

		//插入数据库日志
		log->insertLog(userMap[this->shareMem.fd]->getUserID(), "上传图片：" + operate);


		//插入文件日志
		string logstr = "图片名称:";
		logstr += fileCheck.fileName;
		CDataTask::fileLog->resultBack(true, "图片上传", logstr, userMap[this->shareMem.fd]->getUserAccount());

		
	}
	else
	{
		CDataTask::fileLog->resultBack(false, "图片上传", "返回图片丢包序号！", userMap[this->shareMem.fd]->getUserAccount());
	}
}

void CDataTask::doInsertRecord()
{
	cout << "执行记录插入业务" << endl;

	HEAD head;
	RESULTBACK result;
	INOUTRECORD inoutRecord;
	SHAREMEM* sendData = new SHAREMEM();
	bool flag = false;

	
	head.bussinessLength = sizeof(RESULTBACK);

	//获取请求体内容
	bzero(&inoutRecord, sizeof(INOUTRECORD));
	memcpy(&inoutRecord, this->shareMem.buf + sizeof(HEAD), sizeof(INOUTRECORD));

	//获取数据库实例
	OperatorFactory* of = OperatorFactory::getInstance();
	PictureOperator* po = (PictureOperator*)of->createRepository(OperatorFactory::PICTURECORED);
	LogOperator* log = (LogOperator*)of->createRepository(OperatorFactory::LOG);
	RecordOperator* ro = (RecordOperator*)of->createRepository(OperatorFactory::CARRECORD);

	cout << "获取数据库实例完成" << endl;

	if (inoutRecord.type == 1)	//入场记录插入
	{
		head.bussinessType = INRECORD_T;
		//查询图片id;
		int img_id = po->queryPicture(inoutRecord.filename);
		cout << "img_id = " << img_id << endl;
		flag = ro->insertRecord(inoutRecord.number, inoutRecord.time, inoutRecord.locate, img_id);
		//插入数据库日志
		log->insertLog(userMap[shareMem.fd]->getUserID(), "上传车辆入场记录");
		cout << "上传车辆入场记录完成" << endl;
		//插入文件日志
		CDataTask::fileLog->InsertRecord(inoutRecord, userMap[this->shareMem.fd]->getUserAccount());

		if (flag)	//数据库操作执行成功
		{
			result.flag = 1;	

			string message = inoutRecord.number;
			message += ",";
			message += inoutRecord.time;
			message += ",";
			message += inoutRecord.locate;
			strcpy(result.message, message.c_str());
			CDataTask::fileLog->resultBack(true, "入库记录插入", message, userMap[this->shareMem.fd]->getUserAccount());
		}
		else	//数据库操作执行失败
		{
			result.flag = 2;
			strcpy(result.message, "记录上传失败");
			CDataTask::fileLog->resultBack(false, "入库记录插入", result.message, userMap[this->shareMem.fd]->getUserAccount());
		}
	}
	else	//出场记录更新或查询
	{
		if (strlen(inoutRecord.filename) == 0)		//图片名称为空，执行入场时间查询
		{
			head.bussinessType = OUTRECORD_T;
			string time = ro->queryTime(inoutRecord.number);

			if (time.empty())
			{
				result.flag = 2;
				strcpy(result.message, "未查询到车辆入场记录");
				CDataTask::fileLog->resultBack(false, "车辆入场时间查询", result.message, userMap[this->shareMem.fd]->getUserAccount());
			}
			else
			{
				result.flag = 1;
				strcpy(result.message, time.c_str());
				//插入数据库日志
				log->insertLog(userMap[shareMem.fd]->getUserID(), "查询车辆入场时间");
				CDataTask::fileLog->resultBack(true, "车辆入场时间查询", result.message, userMap[this->shareMem.fd]->getUserAccount());
			}
		}
		else		//执行出场记录更新
		{
			head.bussinessType = RESULTBACK_T;
			//查询图片id;
			int img_id = po->queryPicture(inoutRecord.filename);
			cout << "inoutRecord.fileName = " << inoutRecord.filename << endl;
			flag = ro->updateRecord(inoutRecord.number, inoutRecord.time, inoutRecord.locate, img_id, inoutRecord.cost);
			//插入数据库日志
			log->insertLog(userMap[shareMem.fd]->getUserID(), "上传车辆出场记录");
			//插入文件日志
			CDataTask::fileLog->InsertRecord(inoutRecord, userMap[this->shareMem.fd]->getUserAccount());

			if (flag)
			{
				result.flag = 1;	//数据库操作执行成功

				string message = inoutRecord.number;
				message += ",";
				message += inoutRecord.time;
				message += ",";
				message += inoutRecord.locate;
				strcpy(result.message, message.c_str());
			}
			else
			{
				result.flag = 2;
				strcpy(result.message, "记录上传失败");
			}
		}
	}

	sendData->fd = this->shareMem.fd;
	memcpy(sendData->buf, &head, sizeof(HEAD));
	memcpy(sendData->buf + sizeof(HEAD), &result, sizeof(RESULTBACK));

	WriteMemShare::sendQueue.push(sendData);
}

void CDataTask::doInsertVideo()
{
	cout << "执行视频信息记录插入业务" << endl;

	VIDEO videoData;
	HEAD head;
	RESULTBACK resultBack;
	SHAREMEM* sendData = new SHAREMEM();
	bzero(sendData, sizeof(SHAREMEM));

	//获取数据库实例
	OperatorFactory* of = OperatorFactory::getInstance();
	VideoOperator* vo = (VideoOperator*)of->createRepository(OperatorFactory::VIDEORECORD);
	LogOperator* log = (LogOperator*)of->createRepository(OperatorFactory::LOG);

	//请求头赋值
	head.bussinessLength = sizeof(VIDEO);
	head.bussinessType = VIDEOPLAY_T;
	sendData->fd = this->shareMem.fd;

	//取出请求体
	memcpy(&videoData, this->shareMem.buf + sizeof(HEAD), sizeof(VIDEO));

	//插入视频信息
	bool flag = vo->insertVideo(videoData, userMap[this->shareMem.fd]->getUserID());

	if (flag)
	{
		resultBack.flag = 1;
		strcpy(resultBack.message, "视频记录插入成功！");
		//写入日志
		string operate = videoData.video_name;
		log->insertLog(userMap[this->shareMem.fd]->getUserID(), "上传视频记录：" + operate);
		CDataTask::fileLog->resultBack(true, "上传视频记录", resultBack.message, userMap[this->shareMem.fd]->getUserAccount());
	}
	else
	{
		resultBack.flag = 2;
		strcpy(resultBack.message, "视频记录插入失败！");
		//写入文件日志
		CDataTask::fileLog->resultBack(false, "上传视频记录", resultBack.message, userMap[this->shareMem.fd]->getUserAccount());
	}

	
	memcpy(sendData->buf, &head, sizeof(HEAD));
	memcpy(sendData->buf + sizeof(HEAD), &resultBack, sizeof(RESULTBACK));

	//添加到发送队列
	WriteMemShare::sendQueue.push(sendData);
}

void CDataTask::doQueryDate(protocolType type)
{
	cout << "执行视频列表日期查询业务" << endl;
	
	OperatorFactory* of = OperatorFactory::getInstance();
	VideoOperator* vo = (VideoOperator*)of->createRepository(OperatorFactory::VIDEORECORD);
	LogOperator* lo = (LogOperator*)of->createRepository(OperatorFactory::LOG);
	HEAD head;
	SHAREMEM* sendData = new SHAREMEM();
	VIDEODATE videoDate;

	head.bussinessLength = sizeof(VIDEODATE);
	head.bussinessType = type;
	bzero(sendData, sizeof(SHAREMEM));


	//按天查询
	if (type == VIDEODAY_T)
	{
		videoDate = vo->queryDay(userMap[this->shareMem.fd]->getUserID());
		lo->insertLog(userMap[this->shareMem.fd]->getUserID(), "按天查询视频日期");
		string message = "返回" + videoDate.number;
		message += "条日期信息";
		CDataTask::fileLog->resultBack(true, "按天查询视频日期", message, userMap[this->shareMem.fd]->getUserAccount());
	}
	else	//按月查询
	{
		videoDate = vo->queryMonth(userMap[this->shareMem.fd]->getUserID());
		lo->insertLog(userMap[this->shareMem.fd]->getUserID(), "按月查询视频日期");
		string message = "返回" + videoDate.number;
		message += "条日期信息";
		CDataTask::fileLog->resultBack(true, "按月查询视频日期", message, userMap[this->shareMem.fd]->getUserAccount());
	}

	sendData->fd = this->shareMem.fd;
	memcpy(sendData->buf, &head, sizeof(head));
	memcpy(sendData->buf + sizeof(HEAD), &videoDate, sizeof(VIDEODATE));

	WriteMemShare::sendQueue.push(sendData);
}

void CDataTask::doQueryVideo()
{
	cout << "执行视频列表查询业务" << endl;

	VIDEOPLAY videoPlay;
	memcpy(&videoPlay, this->shareMem.buf + sizeof(HEAD), sizeof(VIDEOPLAY));
	
	OperatorFactory* of = OperatorFactory::getInstance();
	VideoOperator* vo = (VideoOperator*)of->createRepository(OperatorFactory::VIDEORECORD);
	LogOperator* lo = (LogOperator*)of->createRepository(OperatorFactory::LOG);
	HEAD head;
	SHAREMEM* sendData = new SHAREMEM();
	VIDEOBACK videoBack;

	head.bussinessLength = sizeof(VIDEOBACK);
	head.bussinessType = VIDEOBACK_T;

	videoBack =  vo->queryVideo(videoPlay, userMap[this->shareMem.fd]->getUserID());

	bzero(sendData, sizeof(SHAREMEM));
	sendData->fd = this->shareMem.fd;
	memcpy(sendData->buf, &head, sizeof(HEAD));
	memcpy(sendData->buf + sizeof(HEAD), &videoBack, sizeof(VIDEOBACK));

	//写入数据库日志
	lo->insertLog(userMap[this->shareMem.fd]->getUserID(), "查询视频列表");
	//写入文件日志
	CDataTask::fileLog->queryVideo(videoPlay, userMap[this->shareMem.fd]->getUserAccount());

	WriteMemShare::sendQueue.push(sendData);
}

void CDataTask::doVideoUpdate()
{
	cout << "执行视频播放位置更新操作" << endl;

	VIDEO videoData;
	OperatorFactory* of = OperatorFactory::getInstance();
	VideoOperator* vo = (VideoOperator*)of->createRepository(OperatorFactory::VIDEORECORD);
	LogOperator* log = (LogOperator*)of->createRepository(OperatorFactory::LOG);
	HEAD head;
	SHAREMEM* sendData = new SHAREMEM();
	RESULTBACK resultBack;

	head.bussinessLength = sizeof(RESULTBACK);
	head.bussinessType = VIDEOUPDATE_T;

	memcpy(&videoData, this->shareMem.buf + sizeof(HEAD), sizeof(VIDEO));

	bool ret = vo->updateVideo(videoData, userMap[this->shareMem.fd]->getUserID());

	if (ret)
	{
		resultBack.flag = 1;
		strcpy(resultBack.message, "视频播放记录更新成功");
		log->insertLog(userMap[this->shareMem.fd]->getUserID(), "更新视频播放位置");
		CDataTask::fileLog->resultBack(true, "更新视频播放位置", "视频播放记录更新成功", userMap[this->shareMem.fd]->getUserAccount());
	}
	else
	{
		resultBack.flag = 2;
		strcpy(resultBack.message, "视频播放记录更新失败");
		CDataTask::fileLog->resultBack(false, "更新视频播放位置", "视频播放记录更新失败", userMap[this->shareMem.fd]->getUserAccount());
	}

	bzero(sendData, sizeof(SHAREMEM));
	sendData->fd = this->shareMem.fd;
	memcpy(sendData->buf, &head, sizeof(HEAD));
	memcpy(sendData->buf + sizeof(HEAD), &resultBack, sizeof(RESULTBACK));

	WriteMemShare::sendQueue.push(sendData);
}