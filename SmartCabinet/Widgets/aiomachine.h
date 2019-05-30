#ifndef AIOMACHINE_H
#define AIOMACHINE_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QEvent>
#include <QList>
#include <QLabel>
#include <QString>
#include <cabinetconfig.h>
#include <QTimer>
#include <QDateTime>
#include "aiobutton.h"
#include "Structs/userinfo.h"

namespace Ui {
class AIOMachine;
}

class AIOMachine : public QWidget
{
    Q_OBJECT

public:
    explicit AIOMachine(QWidget *parent = 0);
    ~AIOMachine();

public slots:
    void recvScanData(QByteArray);
    void recvUserCheckRst(UserInfo *);//接收用户校验结果
    void recvUserInfo(QByteArray qba);//接收用户信息
    void sysLock();//系统锁定

signals:
    void requireUserCheck(QString);//请求身份验证

private:
    enum cEvent
    {
        click_num_expired,
        click_num_goods,
        click_num_today_in,
        click_num_today_out,
        click_num_warning_rep,
        click_lab_temp,
        click_lab_hum,
    };

signals:
    void click_event(cEvent c);
    void reqCheckVersion(bool);

private slots:
    void on_aio_quit_clicked();

private:
    Ui::AIOMachine *ui;
    QList<QLabel*> l_num_label;
    UserInfo* optUser;
    CabinetConfig* config;
    QTimer* sysTime;
    bool loginState;

    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *, QEvent *);
    void initNumLabel();
    void setAioInfo(QString departName, QString departId);

    void sysUnlock();

private slots:
    void loginTimeout();
    void updateTime();
};

#endif // AIOMACHINE_H
