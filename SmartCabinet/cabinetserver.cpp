#include "cabinetserver.h"
#include <QString>
#include <QDebug>
#include <QTime>
#include <QUrl>
#include <QtGlobal>
#include "defines.h"

#define SERVER_ADDR "http://175.11.186.197"
#define API_REG "/spd/mapper/Depart/saveOrUpdate/"   //注册接口


CabinetServer::CabinetServer(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

bool CabinetServer::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    if(config->cabId.isEmpty())
        cabRegister();
    return true;
}

void CabinetServer::cabRegister()
{
#ifdef NO_SERVER
    qDebug()<<"[Cabinet register]:success"<<regId;
    config->setCabinetId(regId);
#endif
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    regId = QString::number(qrand()%1000000);
    QByteArray qba = QString("{\"code\":\"%1\"}").arg(regId).toUtf8();
    QString nUrl = QString(SERVER_ADDR)+QString(API_REG)+'?'+qba.toBase64();
    reply_register = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_register, SIGNAL(finished()), this, SLOT(recvCabRegister()));
}

void CabinetServer::userLogin(QString userId)
{
#ifdef NO_SERVER
    emit loginRst(true);
#endif


}

void CabinetServer::recvCabRegister()
{
    QByteArray qba = QByteArray::fromBase64(reply_register->readAll());
    reply_register->deleteLater();

    cJSON* json = cJSON_Parse(qba.data());
//    qDebug()<<cJSON_Print(json);

    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");

    if(json_rst->type == cJSON_True)
    {
        qDebug()<<"[Cabinet register]:success"<<regId;
        config->setCabinetId(regId);
    }
    else
    {
        cabRegister();
    }
}
