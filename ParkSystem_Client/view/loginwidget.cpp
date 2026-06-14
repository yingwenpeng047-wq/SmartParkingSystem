#include "loginwidget.h"
#include "ui_loginwidget.h"

LoginWidget::LoginWidget(QWidget *parent):QWidget(parent),ui(new Ui::LoginWidget)
{
    ui->setupUi(this);
    this->thread=CFileSplit::getInstance();
    this->heartThread=HeartBoomThread::getInstance();
    this->camerathread=CameraThread::getInstance();
    this->timer=new QTimer();
    this->reginWidget=new ReginWidget();

    bzero(this->buf,sizeof(buf));
    bzero(&this->head,sizeof(HEAD));
    bzero(&this->login,sizeof(USER));

    this->sc=SocketConnect::getInstance();
    this->rd=ReadThread::getInstance();
    this->wd=WriteThread::getInstance();

    this->rd->setSocketfd(this->sc->getSocketfd());
    this->wd->setSocketfd(this->sc->getSocketfd());

    QRegExp regExp1("^[a-zA-Z0-9]{6}$");                       //正则表达式
    QRegExpValidator *validator1=new QRegExpValidator(regExp1,this->ui->passwdEdit);
    this->ui->passwdEdit->setValidator(validator1);

    QRegExp regExp2("^[a-zA-Z0-9_]{11}$");                     //正则表达式
    QRegExpValidator *validator2=new QRegExpValidator(regExp2,this->ui->phoneEdit);
    this->ui->phoneEdit->setValidator(validator2);

    this->code=new MyCode(this);
    this->code->setGeometry(506,280,100,21);

    this->codeNumber = this->code->getCode();

    this->init_connect();

}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::init_connect()
{
    connect(this->reginWidget,SIGNAL(regin2Login()),this,SLOT(showLoginWin()));
    connect(this->reginWidget,SIGNAL(go2Loginwin(QString)),this,SLOT(showLoginWin(QString)));

    connect(this->rd,&ReadThread::send2LoginWin,this,&LoginWidget::receiveLoginInfo1,Qt::BlockingQueuedConnection);
    connect(this->rd,&ReadThread::send2LoginWin5,this,&LoginWidget::receiveLoginInfo2,Qt::BlockingQueuedConnection);

    connect(this->timer,&QTimer::timeout,this,&LoginWidget::sendheart);
}



void LoginWidget::paintEvent(QPaintEvent *event)
{

}

void LoginWidget::on_reginBtn_clicked()
{
    this->ui->phoneEdit->clear();
    this->ui->testEdit->clear();
    this->ui->passwdEdit->clear();
    this->hide();
    this->reginWidget->show();

}

void LoginWidget::showLoginWin()
{
    this->reginWidget->hide();
    this->show();
}

void LoginWidget::showLoginWin(QString phone)
{
    this->reginWidget->hide();
    this->show();
    this->ui->phoneEdit->setText(phone);

}


void LoginWidget::on_loginBtn_clicked()
{
    //发送账号密码过去，然后开始读，根据读到的信息，来判断是否成功
    QString phone=this->ui->phoneEdit->text();
    QString passwd=this->ui->passwdEdit->text();
    QString test=this->ui->testEdit->text();
    this->codeNumber=this->code->getCode();
    if(phone.size()==0 || passwd.size()==0 || test.size()==0)
    {
        QMessageBox::information(this,"登录界面","表单不能为空",QMessageBox::Ok,QMessageBox::Ok);
        return;
    }
    else if(phone.size()!=11)
    {
        QMessageBox::information(this,"登录界面","手机号码长度为11",QMessageBox::Ok,QMessageBox::Ok);
        return;
    }
    else if(passwd.size()!=6)
    {
        QMessageBox::information(this,"登录界面","密码长度为6",QMessageBox::Ok,QMessageBox::Ok);
        return;
    }

    else if(this->ui->testEdit->text().toLower()!=this->codeNumber.toLower())      //不验证大小写
    {
       QMessageBox::warning(this,"登录界面","验证码不对");
       qDebug()<<"code="<<codeNumber;
       this->code->refreshCode();                          //生成新的验证码
       this->codeNumber = this->code->getCode();           //获取新的验证码
       this->code->update();
       this->ui->testEdit->clear();                        //清空输入框
       return;
    }

    const char *account=phone.toStdString().c_str();
    memcpy(this->login.account,account,sizeof(this->login.account));

    // 用UTF-8 兼容非 ASCII
    QByteArray md5Hash = QCryptographicHash::hash(passwd.toUtf8(),QCryptographicHash::Md5).toHex(); // 32字符
    // 安全复制到 login.pwd（40字节缓冲区）
    // +1 包含终止符
    size_t copyLen = qMin(static_cast<size_t>(md5Hash.size()) + 1, sizeof(this->login.pwd));
    memcpy(this->login.pwd, md5Hash.constData(), copyLen);

    this->head.bussinessType=USER_T;
    this->head.bussinessLength=sizeof(USER);

    memcpy(buf,&head,sizeof(HEAD));
    memcpy(buf+sizeof(HEAD),&this->login,sizeof(USER));

    ((HEAD*)buf)->crc=0;
    unsigned int crc = CTool::crc32(0xFFFFFFFF, (unsigned char*)buf, sizeof(HEAD) + sizeof(USER));
    ((HEAD*)buf)->crc = htonl(crc);

    if(this->rd->getIsStart()==false)
    {
        this->rd->start();
        this->rd->setIsStart(true);
    }
    this->wd->setData(this->buf,sizeof(this->head)+sizeof(this->login));
    if(this->wd->getIsStart()==false)
    {
        this->wd->start();
        this->wd->setIsStart(true);
    }
}

void LoginWidget::receiveLoginInfo2(int num, const QVector<UPDATATABLE> &temptable)
{
    QMessageBox::information(this,"登录界面","登录成功",QMessageBox::Ok,QMessageBox::Ok);
    //文件拆分线程的手机号也可以确定了
    this->thread->setUser_account(this->ui->phoneEdit->text());
    this->ui->passwdEdit->clear();
    this->ui->phoneEdit->clear();
    this->code->refreshCode();
    this->codeNumber = this->code->getCode();
    this->ui->testEdit->clear();

    this->camerathread->setWhichwidget(MAINWIDGET);
    this->camerathread->activate();
    this->camerathread->start();

    this->heartThread->setSocketfd(this->sc->getSocketfd());
    this->heartThread->start();

    //每隔5min发送一次心跳
    this->timer->start(1000*5*60);

    emit this->go2Mainwin();
    emit this->tableshow(num,temptable);
    return;
}

void LoginWidget::receiveLoginInfo1(int flag,char *info)
{
    //登陆失败要提示加清空表单
    QString qstr=QString::fromStdString(info);
    QMessageBox::warning(this,"登录界面","登陆失败"+qstr,QMessageBox::Ok,QMessageBox::Ok);
    this->ui->passwdEdit->clear();
    this->ui->phoneEdit->clear();
    this->code->refreshCode();
    this->codeNumber = this->code->getCode();
    this->code->update();
    this->ui->testEdit->clear();
}

void LoginWidget::sendheart()
{
    //每过5min利用心跳线程发送一个头给到服务器
    this->heartThread->activate();
}


