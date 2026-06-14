#include "reginwidget.h"
#include "ui_reginwidget.h"


//我先点获取验证码，然后去接收服务器发来的消息，判断flag==1，然后将这个char数组也就是验证码发给注册窗口，然后在点击注册按钮的时候，将用户填写的验证码和传过来的对比，一样就将手机号码和用户名再次发上去

ReginWidget::ReginWidget(QWidget *parent):QWidget(parent),ui(new Ui::ReginWidget)
{
    this->isRegin=false;
    this->currentSecond=0;
    ui->setupUi(this);
    bzero(this->buf,sizeof(buf));
    bzero(&this->head,sizeof(HEAD));
    bzero(&this->reg,sizeof(REGISTER));

    this->rd=ReadThread::getInstance();
    this->wd=WriteThread::getInstance();

    //中英文大小写、数字，长度 6~8 位
    QRegExp regExp1("^[\\u4e00-\\u9fa5a-zA-Z0-9]{6,8}$");                     //正则表达式
    QRegExpValidator *validator1=new QRegExpValidator(regExp1,this->ui->userNameEdit);
    this->ui->userNameEdit->setValidator(validator1);

    //只能输入11位的数字0-9
    QRegExp regExp2("^[0-9]{11}$");                     //正则表达式
    QRegExpValidator *validator2=new QRegExpValidator(regExp2,this->ui->phoneEdit);
    this->ui->phoneEdit->setValidator(validator2);

    //6位数字验证码
    QRegExp regExp3("^[0-9]{6}$");                     //正则表达式
    QRegExpValidator *validator3=new QRegExpValidator(regExp3,this->ui->testEdit);
    this->ui->testEdit->setValidator(validator3);

    this->timer=new QTimer(this);

    this->init_connect();
}

ReginWidget::~ReginWidget()
{
    delete ui;
}


void ReginWidget::init_connect()
{
    connect(this->timer,&QTimer::timeout,this,&ReginWidget::testCannotClick);
    connect(this->rd,&ReadThread::Send2ReginWin,this,&ReginWidget::receiveReginInfo,Qt::BlockingQueuedConnection);
}


//need to change
void ReginWidget::on_reginBtn_clicked()
{
    QString phone=this->ui->phoneEdit->text();
    QString userName=this->ui->userNameEdit->text();
    QString test=this->ui->testEdit->text();
    if(phone.size()==0 || userName.size()==0 || test.size()==0)
    {
        QMessageBox::information(this,"注册界面","手机号、用户名和验证码不能为空",QMessageBox::Ok,QMessageBox::Ok);
        this->ui->phoneEdit->clear();
        this->ui->userNameEdit->clear();
        this->ui->testEdit->clear();
        return;
    }
    else if(phone.size()!=11 )
    {
        QMessageBox::information(this,"注册界面","手机号码长度为11",QMessageBox::Ok,QMessageBox::Ok);
        this->ui->phoneEdit->clear();
        this->ui->userNameEdit->clear();
        this->ui->testEdit->clear();
        return;
    }

    else if(userName.size()<6)
    {
        QMessageBox::information(this,"注册界面","用户名长度不低于6位",QMessageBox::Ok,QMessageBox::Ok);
        this->ui->phoneEdit->clear();
        this->ui->userNameEdit->clear();
        this->ui->testEdit->clear();
        return;
    }
    else if(test.size()!=6)
    {
        QMessageBox::information(this,"注册界面","验证码为6位数字",QMessageBox::Ok,QMessageBox::Ok);
        this->ui->phoneEdit->clear();
        this->ui->userNameEdit->clear();
        this->ui->testEdit->clear();
        return;
    }
    if(test!=this->testNum)
    {
        QMessageBox::information(this,"注册界面","验证码不对",QMessageBox::Ok,QMessageBox::Ok);
        this->ui->phoneEdit->clear();
        this->ui->userNameEdit->clear();
        this->ui->testEdit->clear();
        return;
    }
    this->isRegin=true;
    //将用户手机号、用户名、验证码再次发给服务器
    bzero(&this->head,sizeof(this->head));
    bzero(&this->reg,sizeof(this->reg));
    bzero(this->buf,sizeof(this->buf));

    //memcpy(this->reg.code,test.toStdString().c_str(),sizeof(this->reg.code));
    memcpy(this->reg.account,phone.toStdString().c_str(),sizeof(this->reg.account));
    QByteArray nameData = userName.toUtf8();
    size_t copyLen = qMin(nameData.size(), static_cast<int>(sizeof(this->reg.name)) - 1);
    memcpy(this->reg.name, nameData.constData(), copyLen);
    this->reg.name[copyLen] = '\0'; // 显式终止

    this->head.bussinessType=REGISTER_T;
    this->head.bussinessLength=sizeof(REGISTER);

    memcpy(buf,&head,sizeof(HEAD));
    memcpy(buf+sizeof(HEAD),&this->reg,sizeof(REGISTER));

    ((HEAD*)buf)->crc=0;
    unsigned int crc = CTool::crc32(0xFFFFFFFF, (unsigned char*)buf, sizeof(HEAD) + sizeof(REGISTER));
    ((HEAD*)buf)->crc = htonl(crc);

    if(this->rd->getIsStart()==false)
    {
        this->rd->start();
        this->rd->setIsStart(true);
    }
    this->wd->setData(this->buf,sizeof(this->head)+sizeof(this->reg));
    if(this->wd->getIsStart()==false)
    {
        this->wd->start();
        this->wd->setIsStart(true);
    }
}

void ReginWidget::on_backBtn_clicked()
{
    this->ui->testEdit->clear();
    this->ui->phoneEdit->clear();
    this->ui->userNameEdit->clear();
    this->hide();
    emit this->regin2Login();
}

void ReginWidget::on_testBtn_clicked()
{
    QString phone=this->ui->phoneEdit->text();
    if(phone.size()==0)
    {
        QMessageBox::information(this,"注册界面","手机号不能为空",QMessageBox::Ok,QMessageBox::Ok);
        this->ui->phoneEdit->clear();
        this->ui->userNameEdit->clear();
        return;
    }
    else if(phone.size()!=11 )
    {
        QMessageBox::information(this,"注册界面","手机号码长度为11",QMessageBox::Ok,QMessageBox::Ok);
        this->ui->phoneEdit->clear();
        this->ui->userNameEdit->clear();
        return;
    }

    //按了之后60S不能再按
    this->ui->testBtn->setEnabled(false);
    this->timer->start(1000);

    bzero(this->reg.name,sizeof(this->reg.name));
    memcpy(this->reg.account,phone.toStdString().c_str(),sizeof(this->reg.account));

    this->head.bussinessType=REGISTER_T;
    this->head.bussinessLength=sizeof(REGISTER);

    memcpy(buf,&head,sizeof(HEAD));
    memcpy(buf+sizeof(HEAD),&this->reg,sizeof(REGISTER));

    ((HEAD*)buf)->crc=0;
    unsigned int crc = CTool::crc32(0xFFFFFFFF, (unsigned char*)buf, sizeof(HEAD) + sizeof(REGISTER));
    ((HEAD*)buf)->crc = htonl(crc);

    if(this->rd->getIsStart()==false)
    {
        this->rd->start();
        this->rd->setIsStart(true);
    }
    this->wd->setData(this->buf,sizeof(this->head)+sizeof(this->reg));
    if(this->wd->getIsStart()==false)
    {
        this->wd->start();
        this->wd->setIsStart(true);
    }
}



void ReginWidget::testCannotClick()
{
    this->currentSecond++;
    this->ui->testBtn->setText(QString::number(60-this->currentSecond)+"秒");
    if(this->currentSecond==60)
    {
        this->ui->testBtn->setEnabled(true);
        this->timer->stop();
        this->ui->testBtn->setText("获取验证码");
        this->currentSecond=0;
    }
}

void ReginWidget::receiveReginInfo(int flag,char *info)
{
    //flag==1说明手机号不存在,发的是验证码
    if(flag==1)
    {
        if(this->isRegin==true)
        {
            QString phone=this->ui->phoneEdit->text();
            //QString userName=this->ui->userNameEdit->text();
            QMessageBox::information(this,"注册界面","注册成功",QMessageBox::Ok,QMessageBox::Ok);
            //编辑框清空，同时跳转到登录界面，将手机号也带过去
            this->ui->testEdit->clear();
            this->ui->phoneEdit->clear();
            this->ui->userNameEdit->clear();

            this->currentSecond=0;
            this->ui->testBtn->setEnabled(true);
            this->ui->testBtn->setText("获取验证码");

            this->isRegin=false;

            emit this->go2Loginwin(phone);
        }
        else if(this->isRegin==false)
        {
            this->testNum=QString::fromStdString(info);
            qDebug()<<this->testNum;
            return;
        }
    }
    //flag==2说明发的是错误信息
    else if(flag==2)
    {
        QString qstr=QString::fromStdString(info);
        QMessageBox::information(this,"注册界面","注册失败:"+qstr,QMessageBox::Ok,QMessageBox::Ok);
        this->ui->testEdit->clear();
        this->ui->phoneEdit->clear();
        this->ui->userNameEdit->clear();
    }
}


