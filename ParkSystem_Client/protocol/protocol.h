#pragma once

//枚举：请求类型
enum protocolType {
    USER_T,
    REGISTER_T,
    RESULTBACK_T,
    INRECORD_T,
    INFOSEARCH_T,
    OUTRECORD_T,
    INFOBACK_T,
    VIDEOPLAY_T,
    VIDEOBACK_T,
    FILEINFO_T,
    FILEBACK_T,
    FILECHECK_T,
    VIDEODAY_T,
    VIDEOMONTH_T,
    VIDEOUPDATE_T,
    LIFE_T,
    UPDATETABLE_T
};

//请求头
typedef struct head
{
    int bussinessType;		//请求类型
    int bussinessLength;	//请求体长度
    unsigned int crc;		//CRC校验码
}HEAD;

//入场表格结构体
typedef struct updatetable
{
    char plate[12];	//车牌
    char time[30];	//时间
    char locate[20];//入库地点
}UPDATATABLE;


//入场表格结构体数组
typedef struct tables{
    int number;		//有效数量
    UPDATATABLE table[4];
}TABLES;


//登录业务体
typedef struct user
{
    char account[12];		//账号  手机号 11位长度
    char pwd[40];			//密码  MD5加密
}USER;

//注册业务体
typedef struct registe
{
    char account[12];		//账号  手机号 11位长度
    char name[18];			//用户名
}REGISTER;

//结果返回体
typedef struct resultback
{
    int flag;				//1-成功	2-失败
    char message[50];		//返回信息	成功-携带信息  失败-失败原因
}RESULTBACK;

//出入场记录业务体
typedef struct inoutrecord
{
    int cost;					//金额
    int type;					//出入场类型 1-入场 2-出场
    char number[12];			//车牌号码
    char time[20];				//时间
    char locate[20];			//地点
    char filename[50];			//图片名称
}INOUTRECORD;

//车辆信息查询业务体
typedef struct infosearch
{
    int page;				//查询页码
    char number[12];		//车牌号码
    char in_time[20];		//入场时间
    char out_time[20];		//出场时间
}INFOSEARCH;

//车辆信息结构体
typedef struct carinfo
{
    char number[12];			//车牌号码
    char in_time[20];			//入场时间
    char out_time[20];			//出场时间
    char in_picture_path[50];	//出场图片路径
    char out_picture_path[50];	//入场图片路径
    int cost;					//停车金额
}CARINFO;

//车辆信息查询返回体
typedef struct infoback
{
    int number;					//有效数量
    CARINFO picture_back[10];	//车辆信息结构体数组
}INFOBACK;

//视频播放信息请求体
typedef struct videoplay
{
    int page;				//查询页码
    char date[20];			//查询日期
}VIDEOPLAY;

//视频播放信息结构体
typedef struct video
{
    char video_name[32];	//视频名称
    char video_path[50];	//视频路径
    char picture_path[50];	//首帧路径
    int video_frame;		//总帧数
    int video_current;		//上次视频播放帧数
}VIDEO;

//视频日期结构体
typedef struct videodate
{
    int number;				//日期有效数量
    char video_time[31][30];//视频日期
}VIDEODATE;

//视频播放信息返回体
typedef struct videoback
{
    int number;				//结构体数组有效数据量
    VIDEO video_back[9];	//视频播放信息结构体数组
}VIDEOBACK;

//共享内存发送消息结构体
typedef struct sharemem
{
    int fd;
    char buf[2560];
}SHAREMEM;

//信息结构体
typedef struct messagebuf {
    long mtype;      /* message type, must be > 0 */
    char mtext[20]; /* message data */
}MSGBUF;

union semun {
    int val;                    /* Value for SETVAL */
    struct semid_ds* buf;       /* Buffer for IPC_STAT, IPC_SET */
    unsigned short* array;      /* Array for GETALL, SETALL */
    struct seminfo* _buf;       /* Buffer for IPC_INFO */
};


//文件传输协议
typedef struct fileinfo
{
    char user_account[12];	//文件归属用户账号
    char fileName[50];		//文件名称
    int number;				//原文件碎片总个数
    int length;				//原文件总字节
    int fileIndex;			//当前碎片的序号
    int fileLength;			//当前碎片的字节数
    char context[1000];		//文件二进制内容
}FILEINFO;

//客户端文件确认发送完成
typedef struct filecheck
{
    char user_account[12];	//文件归属用户账号
    char fileName[50];		//文件名称			日期+车牌号
    char cpath[50];			//客户端图片保存路径
}FILECHECK;

//文件碎片缺失返回体
typedef struct fileback
{
    char filename[50];	//文件名称
    int arr[100];		//丢包序号
}FILEBACK;
