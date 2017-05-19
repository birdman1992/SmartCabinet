#ifndef CABINETSERVER_H
#define CABINETSERVER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "cabinetconfig.h"
#include "Json/cJSON.h"


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
    QString regId;

    void cabRegister();

signals:
    void loginRst(bool);

public slots:
    void userLogin(QString);
private slots:
    void recvCabRegister();
};

#endif // CABINETSERVER_H
