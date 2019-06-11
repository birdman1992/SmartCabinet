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
#include <QMap>
#include "aiobutton.h"
#include "Structs/userinfo.h"
#include "aiooverview.h"

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
    void recvAioOverview(QString msg, AIOOverview* overview);

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
    enum colMark
    {
        unknow=-1,
        goodsId,//物品编码
        goodsName,//物品名称
        packageType,//包类型
        proName,//生产商
        supplyName,//供应商
        size,//规格
        unit,//单位
        threshold,//预警数量
        maxThreshold,//最大数量
    };

signals:
    void click_event(cEvent c);
    void reqCheckVersion(bool);
    void reqUpdateOverview();

private slots:
    void on_aio_quit_clicked();

private:
    Ui::AIOMachine *ui;
    QMap<QString, colMark> mapColName;
    QList<QLabel*> l_num_label;
    QList<GoodsInfo*> cur_list;
    QStringList listColName;
    UserInfo* optUser;
    CabinetConfig* config;
    QTimer* sysTime;
    bool loginState;

    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *, QEvent *);
    void initNumLabel();
    void initColMap();
    void setAioInfo(QString departName, QString departId);
    void setNumLabel(AIOOverview* overview);
    void showTable(QString title, QStringList colNames, QList<GoodsInfo*>);
    QString getGoodsInfoText(GoodsInfo* info, QString key);
    QList<GoodsInfo*> listPage(unsigned int pageNum);

    void sysUnlock();

private slots:
    void loginTimeout();
    void updateTime();
};

#endif // AIOMACHINE_H
