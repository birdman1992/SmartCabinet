#include "cabinetserver.h"
#include <QString>
#include <QDebug>
#include <QTime>
#include <QUrl>
#include <QtGlobal>
#include "defines.h"

//#define SERVER_ADDR "http://175.11.98.137"
#define SERVER_ADDR "http://120.77.159.8:8080"
#define API_REG "/spd/mapper/SmartCheset/saveOrUpdate/"   //注册接口
#define API_LOGIN "/spd/mapper/UserInfo/query/"  //登录接口
#define API_LIST_CHECK "/spd/work/OutStorage/query/goods/" //送货单检查接口
//#define API_GOODS_CHECK "/spd/mapper/Goods/query/"  //货物查询
#define API_LIST_STORE "/spd/mapper/OutStorage/query/"      //存入完毕销单接口
#define API_CAB_BIND "/spd/work/Cheset/register/"     //柜格物品绑定接口
#define API_GOODS_ACCESS  "/spd/work/Cheset/doGoods/"



CabinetServer::CabinetServer(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

bool CabinetServer::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    if(config->getCabinetId().isEmpty())
//        cabRegister();
    {
        regId = "896443";
        config->setCabinetId(regId);
    }

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
    qDebug()<<"[cabRegister]"<<nUrl;
    reply_register = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_register, SIGNAL(finished()), this, SLOT(recvCabRegister()));
}

void CabinetServer::userLogin(QString userId)
{
#ifdef NO_SERVER
    emit loginRst(true);
#endif
    QByteArray qba = QString("{\"cardId\":\"%2\",\"departId\":\"%1\"}").arg(config->getCabinetId()).arg(userId).toUtf8();
    QString nUrl = QString(SERVER_ADDR)+QString(API_LOGIN)+'?'+qba.toBase64();
    qDebug()<<"[login]"<<nUrl;
    qDebug()<<qba;
    reply_login = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_login, SIGNAL(finished()), this, SLOT(recvUserLogin()));
}

void CabinetServer::listCheck(QString code)
{
    QByteArray qba = QString("{\"barcode\":\"%1\"}").arg(code).toUtf8();
    QString nUrl = QString(SERVER_ADDR)+QString(API_LIST_CHECK)+'?'+qba.toBase64();
    barCode = code;
    qDebug()<<"[listCheck]"<<nUrl;
    reply_list_check = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_list_check, SIGNAL(finished()), this, SLOT(recvListCheck()));
}

void CabinetServer::cabinetBind(int seqNum, int index, QString goodsId)
{
    QString caseId = QString::number(config->getLockId(seqNum, index));
    QString cabinetId = config->getCabinetId();
    QByteArray qba = QString("{\"goodsId\":\"%1\",\"chesetCode\":\"%2\",\"goodsCode\":\"%3\"}").arg(goodsId).arg(cabinetId).arg(caseId).toUtf8();
    QString nUrl = QString(SERVER_ADDR)+QString(API_CAB_BIND)+"?"+qba.toBase64();
    qDebug()<<"[cabinetBind]"<<nUrl<<"\n"<<qba;
    reply_cabinet_bind = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_cabinet_bind, SIGNAL(finished()), this, SLOT(recvCabBind()));
}

void CabinetServer::goodsAccess(CaseAddress addr, QString id, int num, bool isStore)
{
    QString caseId = QString::number(config->getLockId(addr.cabinetSeqNUM, addr.caseIndex));
    QString cabinetId = config->getCabinetId();
    QByteArray qba;

    if(isStore)
        qba = QString("{\"packageBarcode\":\"%1\",\"chesetCode\":\"%2\",\"goodsCode\":\"%3\",\"optType\":%4,\"optCount\":%5,\"barcode\":\"%6\"}")
             .arg(id).arg(cabinetId).arg(caseId).arg(2).arg(num).arg(barCode).toUtf8();
    else
        qba = QString("{\"packageBarcode\":\"%1\",\"chesetCode\":\"%2\",\"goodsCode\":\"%3\",\"optType\":%4,\"optCount\":%5}")
             .arg(id).arg(cabinetId).arg(caseId).arg(1).arg(num).toUtf8();

    QString nUrl = QString(SERVER_ADDR)+QString(API_GOODS_ACCESS)+"?"+qba.toBase64();
    qDebug()<<"[goodsAccess]"<<nUrl;
    qDebug()<<qba;
    reply_goods_access = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_goods_access, SIGNAL(finished()), this, SLOT(recvGoodsAccess()));

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
        qDebug()<<"reg"<<config->getCabinetId();
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
    qDebug()<<cJSON_Print(json);

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

QByteArray test = QByteArray("{\
                             \"callTime\": 0,\
                             \"success\": true,\
                             \"errorCode\": \"5000\",\
                             \"msg\": \"\",\
                             \"data\": {\
                               \"goods\": [     \
                                 {\
                                   \"name\": \"3M高强度外壳胶带（丝绸布胶带）\",\
                                   \"goodsId\": \"A0201002349\",\
                                   \"size\": \"1538-0\",\
                                   \"unit\": \"卷\",\
                                   \"takeCount\": 20,\
                                   \"singlePrice\": 10.00,\
                                   \"packageBarcode\": \"A0201002349\",\
                                   \"packageType\": 1,\
                                   \"batchNumber\": 20170523,\
                                   \"inStorageId\": 42,\
                                   \"lifeTime\": null,\
                                   \"roomName\": \"丝绸\",\
                                   \"producerName\": null\
                                 },\
                                {\
                                \"name\": \"BD真空测试包\",\
                                \"goodsId\": \"EO501002219\",\
                                \"size\": \"1538-0\",\
                                \"unit\": \"个\",\
                                \"takeCount\": 20,\
                                \"singlePrice\": 10.00,\
                                \"packageBarcode\": \"EO501002219\",\
                                \"packageType\": 1,\
                                \"batchNumber\": 20170523,\
                                \"inStorageId\": 42,\
                                \"lifeTime\": null,\
                                \"roomName\": \"丝绸\",\
                                \"producerName\": null\
                                }\
                               ],\
                               \"store\": {\
                                 \"id\": 0,\
                                 \"barcode\": \"T220170523094959259\",\
                                 \"carId\": 3,\
                                 \"totalPrice\": 200.00,\
                                 \"hosId\": 0,\
                                 \"departId\": 0,\
                                 \"warehouseId\": 0,\
                                 \"deliveryMan\": \"yyx\",\
                                 \"state\": 0,\
                                 \"remark\": null,\
                                 \"optUser\": null,\
                                 \"optTime\": null,\
                                 \"hosName\": \"安化县人民医院\",\
                                 \"departName\": \"内科(智能柜)\",\
                                 \"warehouseName\": \"安化一人民医院中心库\",\
                                 \"nowTime\": \"2017-05-23 09:53:03\",\
                                 \"goods\": null\
                               }\
                             }\
                           }");

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
        if(json_data->type == cJSON_NULL)
        {
            emit listRst(list);
            return;
        }

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
            info->batchNumber = cJSON_GetObjectItem(json_info,"batchNumber")->valueint;
            info->goodsId = QString::fromUtf8(cJSON_GetObjectItem(json_info,"goodsId")->valuestring);
            info->inStorageId = cJSON_GetObjectItem(json_info,"inStorageId")->valueint;
            info->name = QString::fromUtf8(cJSON_GetObjectItem(json_info,"name")->valuestring);
            info->packageBarcode = QString::fromUtf8(cJSON_GetObjectItem(json_info,"packageBarcode")->valuestring);
            info->roomName = QString::fromUtf8(cJSON_GetObjectItem(json_info,"roomName")->valuestring);
            info->singlePrice = cJSON_GetObjectItem(json_info,"singlePrice")->valueint;
            info->size = QString::fromUtf8(cJSON_GetObjectItem(json_info,"size")->valuestring);
            info->takeCount = cJSON_GetObjectItem(json_info,"packageCount")->valueint;
            info->totalNum = info->takeCount;
            info->unit = QString::fromUtf8(cJSON_GetObjectItem(json_info,"unit")->valuestring);
            qDebug()<<"[goods]"<<info->name<<info->goodsId<<info->takeCount<<info->unit;
            list->addGoods(info);
        }
        cJSON* json_list_info = cJSON_GetObjectItem(json_data,"store");
        list->barcode = QString::fromUtf8(cJSON_GetObjectItem(json_list_info, "barcode")->valuestring);

        emit listRst(list);
    }
}

void CabinetServer::recvCabBind()
{
    QByteArray qba = QByteArray::fromBase64(reply_cabinet_bind->readAll());
    reply_cabinet_bind->deleteLater();

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<cJSON_Print(json);

    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        qDebug()<<"bind success";
        emit bindRst(true);
    }
    else
    {
        emit bindRst(false);
    }
    cJSON_Delete(json);
}

void CabinetServer::recvGoodsAccess()
{
    QByteArray qba = QByteArray::fromBase64(reply_goods_access->readAll());
    reply_goods_access->deleteLater();

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<cJSON_Print(json);

    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        qDebug()<<"ACCESS success";
    }
    cJSON_Delete(json);
}

