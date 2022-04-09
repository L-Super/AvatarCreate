#include "Widget.h"
#include "ui_Widget.h"
#include <QMessageBox>
#include <QSslSocket>
#include <QtNetwork>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , manager(new QNetworkAccessManager(this))
    ,num(0)
{
    ui->setupUi(this);
//    qDebug()<< QSslSocket::sslLibraryBuildVersionString();
    setWindowTitle("头像生成器");
    setWindowIcon(QIcon(":/resource/Multiavatar_256.ico"));
    ui->lineEdit->setFrame(false);
    ui->lineEdit->setPlaceholderText("输入任何内容并回车");
    url = ("https://api.multiavatar.com/");

    connect(manager,&QNetworkAccessManager::finished,this,&Widget::ReplyFinished);
    connect(ui->lineEdit,&QLineEdit::returnPressed,this,&Widget::PostRequest);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::ReplyFinished(QNetworkReply *reply)
{
    if(reply->error()!=QNetworkReply::NoError)
    {
        qDebug()<<"reply error:"<<reply->errorString();
    }
    QByteArray replyData = reply->readAll();
//    qDebug()<<"data: "<<QString(replyData);

    pixmap.loadFromData(replyData);
    ui->avatarLabel->setPixmap(pixmap.scaled(QSize(200,200),
                                             Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation));

    reply->deleteLater();
    ui->progressBar->setValue(100);
    ui->progressBar->setFormat("生成头像成功");
    timer.stop();
    qDebug()<<"reply finished";
}

void Widget::PostRequest()
{
    if(ui->lineEdit->text().isEmpty())
    {
        qDebug()<<"lineEdit is empty";
        //生成随机数
        auto randNum = QRandomGenerator::global()->generate();

        auto randUrl = url.url()  + QString::number(randNum) + ".png";
//        randUrl.append("?apikey=").append(apiKey);
        request.setUrl(randUrl);
        manager->get(request);
    }
    else
    {
        qDebug()<<"post requst";
        QString lineText = ui->lineEdit->text();
        auto tUrl = url.url() + lineText + ".png";
//        tUrl.append("?apikey=").append(apiKey);
//        qDebug()<<"url"<<tUrl;
        request.setUrl(tUrl);
        manager->get(request);
    }

    num = 0;
    ui->progressBar->reset();
    ui->progressBar->setFormat(QString("正在生成中%p%"));

    connect(&timer,&QTimer::timeout,[&](){
        qDebug()<<"timeout"<<num;
        if(num<100)
        {
            ui->progressBar->setValue(num++);
        }
    });
    timer.start(500);
}


void Widget::on_refreshPushButton_clicked()
{
    auto randNum = QRandomGenerator::global()->generate();

    auto randUrl = url.url()  + QString::number(randNum) + ".png";
    randUrl.append("?apikey=");
    randUrl.append(apiKey);
    request.setUrl(randUrl);
    manager->get(request);
}

void Widget::on_savePushButton_clicked()
{
    if(pixmap.isNull())
    {
        qDebug()<<"pixmap is null";
        return;
    }
    QString exePath = QCoreApplication::applicationDirPath();
    QString fileName = (exePath + "/avatar");
    //需要指定保存格式，否则有些字符不能作为文件名
    QString timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss");;
    fileName.append("-");
    fileName.append(timeStr);
    fileName.append(".png");

    if(pixmap.save(fileName))
    {
        QMessageBox box(this);
        box.setWindowTitle("Avatar");
        box.setIconPixmap(QPixmap(":/resource/info.png").scaled(QSize(50,50),
                                                                       Qt::KeepAspectRatio,
                                                                       Qt::SmoothTransformation));
        box.setText(QString("头像保存成功!\n保存在%1路径下").arg(exePath));
        box.exec();
    }
    else
    {
        qDebug()<<"save fail";
    }
}
