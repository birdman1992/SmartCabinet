#ifndef CABINETSERVER_H
#define CABINETSERVER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

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
    QString regId;
    QString barCode;

    void cabRegister();

signals:
    void loginRst(UserInfo);
    void listRst(GoodsList*);
    void bindRst(bool);

public slots:
    void userLogin(QString);
    void listCheck(QString);//送货单信息校验
    void cabinetBind(int, int, QString);
    void goodsAccess(CaseAddress, QString, int, bool);

private slots:
    void recvCabRegister();
    void recvUserLogin();
    void recvListCheck();
    void recvCabBind();
    void recvGoodsAccess();
};

#endif // CABINETSERVER_H
