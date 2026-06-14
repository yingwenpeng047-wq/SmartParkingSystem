#pragma once
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <ctime>
using namespace std;

#define SA struct sockaddr
#define MAXLINE 4096
#define MAXSUB  2000
#define MAXPARAM 2048
#define LISTENQ 1024

extern int h_errno;

//短信验证码发送类
class SendCode
{
public:
	//构造函数 传入手机号码
	SendCode(char* phone_num);
	~SendCode();
	ssize_t http_post(char* page, char* poststr);
	ssize_t send_sms(char* content);
	int  socked_connect(char* arg);
	//生成验证码
	string createCode();
	//执行工作
	string doWork();

private:
	int basefd;
	char hostname[20];
	char send_sms_uri[60];
	char phone_number[12];
	char account[12];
	char password[40];
};

