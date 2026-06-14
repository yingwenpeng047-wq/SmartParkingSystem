#include "SendCode.h"

SendCode::SendCode(char* phone_num)
{
    bzero(this->phone_number, sizeof(phone_num));
	bzero(this->hostname, sizeof(this->hostname));
	bzero(this->send_sms_uri, sizeof(this->send_sms_uri));
    bzero(this->account, sizeof(this->account));
    bzero(this->password, sizeof(this->password));
    strcpy(this->phone_number, phone_num);
	strcpy(this->hostname, "106.ihuyi.cn");
	strcpy(this->send_sms_uri, "/webservice/sms.php?method=Submit&format=json");
    strcpy(this->account, "C90233692");
    strcpy(this->password, "7c0a02a3080d27e46bf7331ac28809dc");
}

SendCode::~SendCode()
{

}

ssize_t SendCode::http_post(char* page, char* poststr)
{
    char sendline[MAXLINE + 1], recvline[MAXLINE + 1];
    ssize_t n;
    snprintf(sendline, MAXSUB,
        "POST %s HTTP/1.0\r\n"
        "Host: %s\r\n"
        "Content-type: application/x-www-form-urlencoded\r\n"
        "Content-length: %zu\r\n\r\n"
        "%s", page, hostname, strlen(poststr), poststr);

    write(basefd, sendline, strlen(sendline));
    while ((n = read(basefd, recvline, MAXLINE)) > 0) {
        recvline[n] = '\0';
        printf("%s", recvline);
    }
    return n;
}

ssize_t SendCode::send_sms(char* content)
{
    char params[MAXPARAM + 1];
    char* cp = params;
    sprintf(cp, "account=%s&password=%s&mobile=%s&content=%s", this->account, this->password, this->phone_number, content);
    return http_post(send_sms_uri, cp);
}

int SendCode::socked_connect(char* arg)
{
    struct sockaddr_in their_addr = { 0 };
    char buf[1024] = { 0 };
    char rbuf[1024] = { 0 };
    char pass[128] = { 0 };
    struct hostent* host = NULL;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("create the sockfd is failed\n");
        return -1;
    }

    if ((host = gethostbyname(arg)) == NULL)
    {
        printf("Gethostname error, %s\n");
        return -1;
    }

    memset(&their_addr, 0, sizeof(their_addr));
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(80);
    their_addr.sin_addr = *((struct in_addr*)host->h_addr);
    if (connect(sockfd, (struct sockaddr*)&their_addr, sizeof(struct sockaddr)) < 0)
    {
        close(sockfd);
        return  -1;
    }
    printf("connect is success\n");
    return sockfd;
}

string SendCode::createCode()
{
    string code;
    // 初始化随机数种子
    srand(static_cast<unsigned int>(std::time(nullptr)));

    int length = 6;

    for (int i = 0; i < length; ++i) {
        // 生成0-9的随机数
        int digit = rand() % 10;
        code += to_string(digit);
    }

    return code;
}

string SendCode::doWork()
{
    struct sockaddr_in servaddr;
    char str[50];

#if 0
    //建立socket连接
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_addr = *(hostname);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(80);
    inet_pton(AF_INET, str, &servaddr.sin_addr);
    connect(sockfd, (SA*)&servaddr, sizeof(servaddr));
#endif

    if ((basefd = socked_connect(hostname)) == -1)
    {
        printf("connect is failed\n");
        return "";
    }
    printf("basefd is =%d\n", basefd);

    string code = this->createCode();
    string content = "您的验证码是：" + code + "。请不要把验证码泄露给其他人。";
    //短信内容
    char message[80] = { 0 };

    strcpy(message, content.c_str());

    /**************** 发送短信 *****************/
    send_sms(message);
    printf("send the message is success\n");
    close(basefd);

    return code;
}
