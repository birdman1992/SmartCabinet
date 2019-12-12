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
#include "Widgets/cabinetaccess.h"
#include "FingerPrint/fingerprint.h"

namespace Ui {
class AIOMachine;
}

class AIOMachine : public QWidget
{
    Q_OBJECT

public:
    explicit AIOMachine(QWidget *parent = 0);
    ~AIOMachine();
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

public slots:
    void recvScanData(QByteArray);
    void recvUserCheckRst(UserInfo *);//接收用户校验结果
    void recvUserInfo(QByteArray qba);//接收用户信息
    void sysLock();//系统锁定
    void recvAioOverview(QString msg, AIOOverview* overview);
    void recvAioData(QString msg,AIOMachine::cEvent e,QList<Goods*> lInfo);
    void updateTemp(QString);
    void updateHum(QString);
    void winMsg(QString);

signals:
    void requireUserCheck(QString);//请求身份验证
    void tsCalReq();//屏幕校准请求
    void cabinetStateChange(CabState state);
    void requireOpenLock(int seq, int index);
    void updateLoginState(bool isLogin);

private:
    enum colMark
    {
        unknow=-1,
        goodsId,//物品编码
        goodsName,//物品名称
        packageType,//包类型
        proName,//生产商
        supplyName,//供应商
        goodsSize,//规格
        unit,//单位
        threshold,//预警数量
        maxThreshold,//最大数量
        packageCount,
        goodsCount,
        lifeDay,
        lifeTime,
        productTime,
        price,
        sumCount,
        aioInNum,
        aioOutNum,
        optName,
        optTime,
        batchNumber,
        traceId,
    };

signals:
//    void click_event(cEvent c);
    void click_event(int);
    void reqCheckVersion(bool);
    void reqUpdateOverview();
    void aio_fetch(int, int);
    void aio_return(bool);
    void stack_switch(int index);
    void logout();
    void aio_check(bool);

private slots:
    void on_aio_quit_clicked();

private:
    Ui::AIOMachine *ui;
    CabinetAccess* win_access;
    QMap<QString, colMark> mapColName;
    QList<QLabel*> l_num_label;
    QList<Goods*> cur_list;
    QStringList listColName;
    UserInfo* optUser;
    CabinetConfig* config;
    QTimer* sysTime;
    bool winActive;
//    FingerPrint* win_fingerPrint;
    bool loginState;
    int curPage;
    int curState;
    QStringList optList;
    QMap<QString, int> curStateText;
    QList<Goods*> showList;

    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *, QEvent *);
    void initNumLabel();
    void initColMap();
    void initStateMap();
    void updateState();
    void nextState();
    void setAioInfo(QString departName, QString departId);
    void setNumLabel(AIOOverview* overview);
    void showTable(QString title, QStringList colNames, QList<Goods*>);
    void showNumExpired(QList<Goods*> lInfo);
    void showNumGoods(QList<Goods*> lInfo);
    void showNumTodayIn(QList<Goods*> lInfo);
    void showNumTodayOut(QList<Goods*> lInfo);
    void showNumWarningRep(QList<Goods*> lInfo);
    void showLabTemp(QList<Goods*> lInfo);
    void showLabHum(QList<Goods*> lInfo);
    QString getGoodsInfoText(Goods* info, QString key);
    QList<Goods*> listPage(unsigned int pageNum);

    void sysUnlock();

    void magicCmd(QString cmd);
    void showEvent(QShowEvent *);
    void setPowState(int power);
private slots:
    void loginTimeout();
    void updateTime();
    void on_tab_back_clicked();
//    void on_aio_fetch_clicked();
    void on_aio_return_clicked();
    void on_aio_check_clicked();
    void on_aio_day_report_clicked();
    void on_aio_check_create_clicked();
    void on_tab_last_clicked();
    void on_tab_next_clicked();
    void on_setting_clicked();
    void on_cur_state_clicked();
};

#endif // AIOMACHINE_H
