#include "carinfocheckwidget.h"
#include "ui_carinfocheckwidget.h"

CarInfoCheckWidget::CarInfoCheckWidget(QWidget *parent):QWidget(parent),ui(new Ui::CarInfoCheckWidget)
{
    ui->setupUi(this);

    this->wd=WriteThread::getInstance();
    this->rd=ReadThread::getInstance();
    this->currentPage=1;
    this->maxPage=1;
    this->isnext=false;

    bzero(&this->head,sizeof(this->head));
    bzero(&this->carinfo,sizeof(this->carinfo));
    bzero(this->buf,sizeof(this->buf));
    this->init_connect();

}

CarInfoCheckWidget::~CarInfoCheckWidget()
{
    delete ui;
}

void CarInfoCheckWidget::init_connect()
{
    connect(this->rd,&ReadThread::send2CarinfoWin,this,&CarInfoCheckWidget::showCarInfo,Qt::BlockingQueuedConnection);
}

void CarInfoCheckWidget::on_checkBtn_clicked()
{
    this->tempcarinfo.clear();

    QString carNum=this->ui->carNumEdit->text();
    QString inTime=this->ui->inTimeEdit->text();
    QString outTime=this->ui->outTimeEdit->text();
    //给车辆查询结构体赋值
    this->carinfo.page=this->currentPage;
    memcpy(this->carinfo.number,carNum.toStdString().c_str(),sizeof(this->carinfo.number));
    memcpy(this->carinfo.in_time,inTime.toStdString().c_str(),sizeof(this->carinfo.in_time));
    memcpy(this->carinfo.out_time,outTime.toStdString().c_str(),sizeof(this->carinfo.out_time));

    this->head.bussinessType=INFOSEARCH_T;
    this->head.bussinessLength=sizeof(INFOSEARCH);

    memcpy(buf,&head,sizeof(HEAD));
    memcpy(buf+sizeof(HEAD),&this->carinfo,sizeof(INFOSEARCH));

    ((HEAD*)buf)->crc=0;
    unsigned int crc = CTool::crc32(0xFFFFFFFF, (unsigned char*)buf, sizeof(HEAD) + sizeof(INFOSEARCH));
    ((HEAD*)buf)->crc = htonl(crc);

    this->wd->setData(this->buf,sizeof(this->head)+sizeof(this->carinfo));

}

void CarInfoCheckWidget::showCarInfo(int num,const QVector<CARINFO> &carinfo)
{
    //防止第一次点击查询没查询到结果就直接页码减去1
    if(num==0 && this->isnext==true)
    {
        qDebug()<<"已经是最后一页了";
        this->isnext=false;
        this->currentPage--;
        return;
    }
    if(num>0)
    {
        //如果是第一次查询，第一页，就将传过来的直接赋值给这个vector
        if(this->currentPage==1 && this->maxPage<=this->currentPage)
        {
            this->tempcarinfo=carinfo;
        }
        qDebug()<<"车辆信息查询";
        //把车辆信息显示在表格中
        //先删除所有的行的内容
        this->ui->tableWidget->clearContents();
        int j=0;
        //传回来的结构体中的车辆数据为新的表格的行号
        for(int i=0;i<num;i++)
        {
            if(this->currentPage-1==this->maxPage)
            {
                this->tempcarinfo.push_back(carinfo[i]);
            }
            this->ui->tableWidget->setItem(i,j,new QTableWidgetItem(QString::number(i+1)));
            qDebug()<<i+1;
            this->ui->tableWidget->setItem(i,j+1,new QTableWidgetItem(carinfo[i].number));
            qDebug()<<carinfo[i].number;
            this->ui->tableWidget->setItem(i,j+2,new QTableWidgetItem(carinfo[i].in_time));
            qDebug()<<carinfo[i].in_time;
            this->ui->tableWidget->setItem(i,j+3,new QTableWidgetItem(carinfo[i].out_time));
            qDebug()<<carinfo[i].out_time;

            //设置入场图片
            QLabel *labelIn = new QLabel();
            QPixmap pixmapIn(QString::fromUtf8(carinfo[i].in_picture_path));
            pixmapIn = pixmapIn.scaled(this->ui->tableWidget->columnWidth(j+4),this->ui->tableWidget->rowHeight(i),Qt::KeepAspectRatio,Qt::SmoothTransformation);
            labelIn->setPixmap(pixmapIn);
            labelIn->setScaledContents(true); // 使图片填充整个 QLabel
            this->ui->tableWidget->setCellWidget(i, j+4, labelIn);

            //设置出场图片
            QLabel *labelIn1 = new QLabel();
            QPixmap pixmapIn1(QString::fromUtf8(carinfo[i].out_picture_path));
            pixmapIn1 = pixmapIn1.scaled(this->ui->tableWidget->columnWidth(j+5),this->ui->tableWidget->rowHeight(i),Qt::KeepAspectRatio,Qt::SmoothTransformation);
            labelIn1->setPixmap(pixmapIn1);
            labelIn1->setScaledContents(true); // 使图片填充整个 QLabel
            this->ui->tableWidget->setCellWidget(i, j+5, labelIn1);

            this->ui->tableWidget->setItem(i,j+6,new QTableWidgetItem(QString::number(carinfo[i].cost)));
            qDebug()<<carinfo[i].cost;
        }
    }
    if(this->maxPage<=this->currentPage)
    {
        this->maxPage=this->currentPage;
    }
}

void CarInfoCheckWidget::on_backMainWinBtn_clicked()
{
    this->tempcarinfo.clear();
    this->currentPage=1;
    this->maxPage=1;
    emit this->backMain();
}

void CarInfoCheckWidget::on_lastPageBtn_clicked()
{
    QString carNum=this->ui->carNumEdit->text();
    QString inTime=this->ui->inTimeEdit->text();
    QString outTime=this->ui->outTimeEdit->text();
    if(this->currentPage == 1) {
        qDebug()<<"已经是第一页了";
        return;
    }
    this->currentPage--;  // 先减少页码
    //清空表格中原有的数据
    this->ui->tableWidget->clearContents();
    int j=0;
    for(int i=(this->currentPage-1)*10;i<this->currentPage*10;i++)
    {
        this->ui->tableWidget->setItem(i,j,new QTableWidgetItem(QString::number(i+1)));
        this->ui->tableWidget->setItem(i,j+1,new QTableWidgetItem(this->tempcarinfo[i].number));
        this->ui->tableWidget->setItem(i,j+2,new QTableWidgetItem(this->tempcarinfo[i].in_time));
        this->ui->tableWidget->setItem(i,j+3,new QTableWidgetItem(this->tempcarinfo[i].out_time));
        //设置入场图片
        QLabel *labelIn = new QLabel();
        QPixmap pixmapIn(QString::fromUtf8(this->tempcarinfo[i].in_picture_path));
        pixmapIn = pixmapIn.scaled(this->ui->tableWidget->columnWidth(j+4),this->ui->tableWidget->rowHeight(i),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        labelIn->setPixmap(pixmapIn);
        labelIn->setScaledContents(true); // 使图片填充整个 QLabel
        this->ui->tableWidget->setCellWidget(i, j+4, labelIn);

        //设置出场图片
        QLabel *labelIn1 = new QLabel();
        QPixmap pixmapIn1(QString::fromUtf8(this->tempcarinfo[i].out_picture_path));
        pixmapIn1 = pixmapIn1.scaled(this->ui->tableWidget->columnWidth(j+5),this->ui->tableWidget->rowHeight(i),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        labelIn1->setPixmap(pixmapIn1);
        labelIn1->setScaledContents(true); // 使图片填充整个 QLabel
        this->ui->tableWidget->setCellWidget(i, j+5, labelIn1);

        this->ui->tableWidget->setItem(i,j+6,new QTableWidgetItem(QString::number(this->tempcarinfo[i].cost)));
    }
}

void CarInfoCheckWidget::on_nextPageBtn_clicked()
{
    qDebug()<<"next";
    this->isnext=true;
    QString carNum=this->ui->carNumEdit->text();
    QString inTime=this->ui->inTimeEdit->text();
    QString outTime=this->ui->outTimeEdit->text();
    this->currentPage++;
    bzero(&this->carinfo,sizeof(this->carinfo));
    bzero(this->buf,sizeof(this->buf));
    bzero(&this->head,sizeof(HEAD));

    memcpy(this->carinfo.number,carNum.toStdString().c_str(),sizeof(this->carinfo.number));
    memcpy(this->carinfo.in_time,inTime.toStdString().c_str(),sizeof(this->carinfo.in_time));
    memcpy(this->carinfo.out_time,outTime.toStdString().c_str(),sizeof(this->carinfo.out_time));

    this->head.bussinessType=INFOSEARCH_T;
    this->head.bussinessLength=sizeof(INFOSEARCH);

    memcpy(buf,&head,sizeof(HEAD));
    memcpy(buf+sizeof(HEAD),&this->carinfo,sizeof(INFOSEARCH));

    ((HEAD*)buf)->crc=0;
    unsigned int crc = CTool::crc32(0xFFFFFFFF, (unsigned char*)buf, sizeof(HEAD) + sizeof(INFOSEARCH));
    ((HEAD*)buf)->crc = htonl(crc);

    this->wd->setData(this->buf,sizeof(this->head)+sizeof(this->carinfo));
    qDebug()<<"next";
}


void CarInfoCheckWidget::on_dataexportBtn_clicked()
{
    // 获取当前时间并格式化为字符串
    QDateTime now = QDateTime::currentDateTime();
    QString filename = "../data/"+now.toString("yyyyMMdd_hhmmss")+"table.txt";

    // 检查文件是否存在，如果不存在则创建
    QFileInfo fileInfo(filename);
    if (!fileInfo.exists())
    {
        QFile newFile(filename);
        if (!newFile.open(QIODevice::WriteOnly))
        {
            qDebug() << "无法创建文件:" << filename;
            return;
        }
        newFile.close(); // 创建文件后立即关闭
    }

    // 打开文件以写入数据
    QFile file(filename);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        qDebug() << "无法打开文件进行写入:" << filename;
        return;
    }

    // 使用QTextStream写入数据
    QTextStream stream(&file);
    QStringList headers = {"序号", "车牌信息", "入场时间\t", "出场时间\t", "入场图片\t\t\t\t", "出场图片\t\t\t\t", "停车金额"};
    if (file.pos() == 0) { // 如果文件是新创建的，则写入表头
        stream << headers.join("\t") << "\n";
    }

    // 写入数据
    for (int i = 0; i < this->tempcarinfo.size(); ++i)
    {
        this->car = this->tempcarinfo[i];
        stream << (i + 1) << "\t"
               << QString::fromUtf8(car.number) << "\t"
               << QString::fromUtf8(car.in_time) << "\t"
               << QString::fromUtf8(car.out_time) << "\t"
               << QString::fromUtf8(car.in_picture_path) << "\t"
               << QString::fromUtf8(car.out_picture_path) << "\t"
               << QString::number(car.cost) << "\n";
    }
    file.close(); // 关闭文件
    qDebug() << "数据已导出到" << filename;
    //QMessageBox::information(this,"车辆信息查询界面","数据已导出到"+filename,QMessageBox::Ok,QMessageBox::Ok);
}
