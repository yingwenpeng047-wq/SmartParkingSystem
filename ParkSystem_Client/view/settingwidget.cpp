#include "settingwidget.h"
#include "ui_settingwidget.h"

SettingWidget::SettingWidget(QWidget *parent):QWidget(parent),ui(new Ui::SettingWidget)
{
    ui->setupUi(this);
    this->videoThread=QVideoShow::getInstance();
    this->sc=SocketConnect::getInstance();
    this->thread=CFileSplit::getInstance();
    this->camerathread=CameraThread::getInstance();
    this->videoThread=QVideoShow::getInstance();

    //读取ip txt下是否有数据
    QFile readIpFile("../data/ipAndport.txt");
    if (readIpFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&readIpFile);
        QString content = in.readAll();
        if(content.size())
        {
            int commaPos = content.indexOf(",");  // 查找逗号位置
            if (commaPos != -1) {
                QString frontPart = content.left(commaPos);  // 截取逗号前
                QString backPart = content.mid(commaPos + 1); // 截取逗号后
                this->ui->ipEdit->setText(frontPart);
                this->ui->portEdit->setText(backPart);
            } else {
                qDebug() << "未找到逗号分隔符！";
            }
        }
        readIpFile.close();
    }

    //读取photo txt下是否有数据
    QFile readphotoFile("../data/photo.txt");
    if (readphotoFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&readphotoFile);
        QString content = in.readAll();
        if(content.size())
        {
            this->ui->photoSaveEdit->setText(content);
        }
        readphotoFile.close();
    }

    //读取video txt下是否有数据
    QFile readvideoFile("../data/video.txt");
    if (readvideoFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&readvideoFile);
        QString content = in.readAll();
        if(content.size())
        {
            this->ui->videoSaveEdit->setText(content);
        }
        readvideoFile.close();
    }
    this->photoPath=this->ui->photoSaveEdit->text();
    this->videoPath=this->ui->videoSaveEdit->text();

}

SettingWidget::~SettingWidget()
{
    delete ui;
}



void SettingWidget::on_photoSaveBtn_clicked()
{
    QString fileName=QFileDialog::getExistingDirectory(this,tr("选择文件夹"),QDir::currentPath(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks); //QFileDialog::ShowDirsOnly只显示目录（过滤掉文件）,QFileDialog::DontResolveSymlinks不解析符号链接
    if(!fileName.isEmpty())
    {
        QString relativePhotoPath= QDir(QDir::currentPath()).relativeFilePath(fileName);
        this->ui->photoSaveEdit->setText(relativePhotoPath);
    }
}

void SettingWidget::on_videoSaveBtn_clicked()
{
    QString fileName=QFileDialog::getExistingDirectory(this,tr("选择文件夹"),QDir::currentPath(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks); //QFileDialog::ShowDirsOnly只显示目录（过滤掉文件）,QFileDialog::DontResolveSymlinks不解析符号链接
   if(!fileName.isEmpty())
   {
       QString relativeVideoPath= QDir(QDir::currentPath()).relativeFilePath(fileName);
       this->ui->videoSaveEdit->setText(relativeVideoPath);
   }
}

void SettingWidget::on_enterBtn_clicked()
{
    //save then go to loginwin
    QString ip=this->ui->ipEdit->text();
    QString port=this->ui->portEdit->text();
    QString photosave=this->ui->photoSaveEdit->text();
    QString videosave=this->ui->videoSaveEdit->text();
    if(ip.size()==0 ||  port.size()==0 || photosave.size()==0 || videosave.size()==0)
    {
        QMessageBox::information(this,"设置界面","表单不能为空",QMessageBox::Ok,QMessageBox::Ok);
        return;
    }

    this->sc->setIp(ip);

    this->sc->setPort((unsigned short)port.toInt());
    //判断是否连接成功
    if(!this->sc->createConnect())
    {
        QMessageBox::information(this,"设置界面","服务器连接失败",QMessageBox::Ok,QMessageBox::Ok);
        return;
    }
    QMessageBox::information(this,"设置界面","服务器连接成功",QMessageBox::Ok,QMessageBox::Ok);
    //拆分线程启动!!!!
    this->thread->start();
    qDebug()<<"文件拆分线程启动";

    //write to ip.txt
    QFile writeIpFile("../data/ipAndport.txt");
       if (writeIpFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
           QTextStream out(&writeIpFile);
           out << this->ui->ipEdit->text()+","+this->ui->portEdit->text(); // 实际写入内容
           writeIpFile.close();
       }

    //write to photo.txt
    QFile writephotoFile("../data/photo.txt");
      if (writephotoFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
          QTextStream out(&writephotoFile);
          out << this->ui->photoSaveEdit->text(); // 实际写入内容
          writephotoFile.close();
      }

    //write to video.txt
    QFile writevideoFile("../data/video.txt");
     if (writevideoFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
         QTextStream out(&writevideoFile);
         out << this->ui->videoSaveEdit->text(); // 实际写入内容
         writevideoFile.close();
     }
     this->photoPath=this->ui->photoSaveEdit->text();
     this->videoPath=this->ui->videoSaveEdit->text();
     this->camerathread->setVideoPath(this->videoPath);
     this->camerathread->setPhotoPath(this->photoPath);
     emit this->go2Loginwin();
}

void SettingWidget::on_backBtn_clicked()
{
    emit this->back2MainWin();
}

QString SettingWidget::getVideoPath() const
{
    return videoPath;
}

void SettingWidget::setVideoPath(const QString &value)
{
    videoPath = value;
}

QString SettingWidget::getPhotoPath() const
{
    return photoPath;
}

void SettingWidget::setPhotoPath(const QString &value)
{
    photoPath = value;
}



void SettingWidget::paintEvent(QPaintEvent *event)
{

}
