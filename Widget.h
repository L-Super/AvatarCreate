#pragma once
/*
 * 利用api生成头像
 * 使用https://api.multiavatar.com/ 接口进行get请求，获取返回的头像
 * TODO:接口未调试成功，未获取其json形式进行解析
*/

#include <QWidget>
#include <QTimer>
#include <QUrl>
#include <QNetworkRequest>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE


class QNetworkReply;
class QNetworkAccessManager;
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void ReplyFinished(QNetworkReply*);
    void PostRequest();

private slots:
    void on_refreshPushButton_clicked();

    void on_savePushButton_clicked();

private:
    Ui::Widget *ui;
    QNetworkAccessManager* manager;
    QNetworkRequest request;
    QUrl url;
    QPixmap pixmap;
    QTimer timer;
    int num;
};
