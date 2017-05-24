#include "cabinetserver.h"
#include <QString>
#include <QDebug>
#include <QTime>
#include <QUrl>
#include <QtGlobal>
#include "defines.h"

#define SERVER_ADDR "http://113.247.171.145"
#define API_REG "/spd/mapper/Depart/saveOrUpdate/"   //注册接口
#define API_LOGIN "/spd/mapper/UserInfo/query/"  //登录接口
#define API_LIST_CHECK "/spd/work/OutStorage/query/goods/" //送货单检查接口
#define API_GOODS_CHECK "/spd/mapper/Goods/query/"  //货物查询
#define API_LIST_STORE "/spd/mapper/OutStorage/query/"      //存入完毕销单接口



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
    QByteArray qba = QString("{\"cardId\":\"%1\"}").arg(userId).toUtf8();
    QString nUrl = QString(SERVER_ADDR)+QString(API_LOGIN)+'?'+qba.toBase64();
//    qDebug()<<"[login]"<<nUrl;
    reply_login = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_login, SIGNAL(finished()), this, SLOT(recvUserLogin()));
}

void CabinetServer::listCheck(QString code)
{
    QByteArray qba = QString("{\"barcode\":\"%1\"}").arg(code).toUtf8();
    QString nUrl = QString(SERVER_ADDR)+QString(API_LIST_CHECK)+'?'+qba.toBase64();
    qDebug()<<"[listCheck]"<<nUrl;
    reply_list_check = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_list_check, SIGNAL(finished()), this, SLOT(recvListCheck()));
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
    cJSON_Delete(json);
}

void CabinetServer::recvUserLogin()
{
    QByteArray qba = QByteArray::fromBase64(reply_login->readAll());
    reply_login->deleteLater();

    cJSON* json = cJSON_Parse(qba.data());
//    qDebug()<<cJSON_Print(json);

    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        UserInfo info;
        cJSON* json_data = cJSON_GetObjectItem(json,"data");
        if(cJSON_GetArraySize(json_data) <= 0)
        {
            cJSON_Delete(json);
            return;
        }

        cJSON* json_info = cJSON_GetArrayItem(json_data,0);
        info.id = cJSON_GetObjectItem(json_info,"id")->valueint;
        info.cardId = QString(cJSON_GetObjectItem(json_info,"cardId")->valuestring);
        info.departId = QString(cJSON_GetObjectItem(json_info,"departId")->valuestring);
        info.identityId = QString(cJSON_GetObjectItem(json_info,"identityId")->valuestring);
        info.name = QString(cJSON_GetObjectItem(json_info,"name")->valuestring);
        info.power = cJSON_GetObjectItem(json_info,"power")->valueint;
        info.tel = QString(cJSON_GetObjectItem(json_info,"tel")->valuestring);
        qDebug()<<"[recvUserLogin]"<<info.cardId<<info.power;
        emit loginRst(info);
    }
    else
    {
        cJSON_Delete(json);
        return;
    }
    cJSON_Delete(json);
}

void CabinetServer::recvListCheck()
{
    QByteArray qba = QByteArray::fromBase64(reply_list_check->readAll());
    reply_list_check->deleteLater();

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<cJSON_Print(json);

    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        Goods* info;
        GoodsList* list = new GoodsList;
        cJSON* json_data = cJSON_GetObjectItem(json,"data");
        cJSON* json_goods = cJSON_GetObjectItem(json_data,"goods");
        int listCount = cJSON_GetArraySize(json_goods);
        if(listCount <= 0)
        {
            cJSON_Delete(json);
            return;
        }

        for(int i=0; i<listCount; i++)
        {
            info = new Goods;
            cJSON* json_info = cJSON_GetArrayItem(json_goods,i);
            info->batchNumber = cJSON_GetObjectItem(json_info,"batchNumber")->valuedouble;
            info->goodsId = QString::fromUtf8(cJSON_GetObjectItem(json_info,"goodsId")->valuestring);
            info->inStorageId = cJSON_GetObjectItem(json_info,"inStorageId")->valueint;
            info->name = QString::fromUtf8(cJSON_GetObjectItem(json_info,"name")->valuestring);
            info->packageBarcode = QString::fromUtf8(cJSON_GetObjectItem(json_info,"packageBarcode")->valuestring);
            info->roomName = QString::fromUtf8(cJSON_GetObjectItem(json_info,"roomName")->valuestring);
            info->singlePrice = cJSON_GetObjectItem(json_info,"singlePrice")->valueint;
            info->size = QString::fromUtf8(cJSON_GetObjectItem(json_info,"size")->valuestring);
            info->takeCount = cJSON_GetObjectItem(json_info,"takeCount")->valueint;
            info->unit = QString::fromUtf8(cJSON_GetObjectItem(json_info,"unit")->valuestring);
            qDebug()<<"[goods]"<<info->name<<info->batchNumber<<info->takeCount<<info->unit;
            list->addGoods(info);
        }
        cJSON* json_list_info = cJSON_GetObjectItem(json_data,"store");
        list->barcode = QString::fromUtf8(cJSON_GetObjectItem(json_list_info, "barcode")->valuestring);

        emit listRst(list);
    }
}
