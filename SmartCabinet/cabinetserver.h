#ifndef CABINETSERVER_H
#define CABINETSERVER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDateTime>
#include <QProcess>

#include "cabinetconfig.h"
#include "Structs/userinfo.h"
#include "Json/cJSON.h"
#include "Structs/goodslist.h"


class CabinetServer : public QObject
{
    Q_OBJECT
public:
    explicit CabinetServer(QObject *parent = 0);

    bool installGlobalConfig(CabinetConfig *globalConfig);
private:
    QNetworkAccessManager* manager;
    CabinetConfig* config;
    QNetworkReply* reply_register;
    QNetworkReply* reply_login;
    QNetworkReply* reply_list_check;
    QNetworkReply* reply_cabinet_bind;
    QNetworkReply* reply_goods_access;
    QNetworkReply* reply_goods_back;
    QNetworkReply* reply_datetime;
    QString regId;
    QString barCode;

    void cabRegister();
    void checkTime();
    void checkSysTime(QDateTime _time);

signals:
    void loginRst(UserInfo);
    void listRst(GoodsList*);
    void bindRst(bool);
    void goodsNumChanged(QString goodsId, int goodsNum);

public slots:
    void userLogin(QString);
    void listCheck(QString);//送货单信息校验
    void cabinetBind(int, int, QString);
    void goodsAccess(CaseAddress, QString, int, int optType);
    void goodsBack(QString);//退货

private slots:
    void recvCabRegister();
    void recvUserLogin();
    void recvListCheck();
    void recvCabBind();
    void recvGoodsAccess();
    void recvGoodsBack();
    void recvDateTime();
};

#endif // CABINETSERVER_H
