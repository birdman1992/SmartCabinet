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
    QString regId;

    void cabRegister();

signals:
    void loginRst(UserInfo);
    void listRst(GoodsList*);

public slots:
    void userLogin(QString);
    void listCheck(QString);//送货单信息校验
private slots:
    void recvCabRegister();
    void recvUserLogin();
    void recvListCheck();
};

#endif // CABINETSERVER_H
