#include "cabinetserver.h"
#include <QString>
#include <QDebug>
#include <QTime>
#include <QUrl>
#include <QtGlobal>
#include "defines.h"

//#define SERVER_ADDR "http://175.11.185.181"
#define SERVER_ADDR "http://120.77.159.8:8080"
#define API_REG "/spd-web/mapper/SmartCheset/saveOrUpdate/"   //注册接口
#define API_LOGIN "/spd-web/mapper/UserInfo/query/"  //登录接口
#define API_LIST_CHECK "/spd-web/work/OutStorage/query/goods/" //送货单检查接口
//#define API_GOODS_CHECK "/spd-web/mapper/Goods/query/"  //货物查询
#define API_LIST_STORE "/spd-web/mapper/OutStorage/query/"      //存入完毕销单接口
#define API_CAB_BIND "/spd-web/work/Cheset/register/"     //柜格物品绑定接口
#define API_GOODS_ACCESS  "/spd-web/work/Cheset/doGoods/"
#define API_GOODS_CHECK  "/spd-web/work/Cheset/doUpdataGoods/"     //退货接口
#define API_CHECK_TIME "/spd-web/mapper/Time/query/"
#define API_REQ_LIST "/spd-web/work/OutStorage/find/OutStorageCar/"      //查询待存送货单接口



CabinetServer::CabinetServer(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    reply_datetime = NULL;
    reply_login = NULL;
    needReqCar = true;
    checkTime();
}

bool CabinetServer::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    if(config->getCabinetId().isEmpty())
        cabRegister();
    requireListState();
//    {
//        regId = "896443";
//        config->setCabinetId(regId);
//    }

    return true;
}

void CabinetServer::waitForListTimeout()
{

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

void CabinetServer::checkTime()
{
    timeIsChecked = false;

    if(reply_datetime != NULL)
    {
        reply_datetime->deleteLater();
        reply_datetime = NULL;
    }

    QString url = QString(SERVER_ADDR) + QString(API_CHECK_TIME);
    reply_datetime = manager->get(QNetworkRequest(QUrl(url)));
    qDebug()<<"[checkTime]"<<url;
    connect(reply_datetime, SIGNAL(readyRead()), this, SLOT(recvDateTime()));

    sysClock.start(60000);
    connect(&sysClock, SIGNAL(timeout()), this, SLOT(sysTimeout()));
}

void CabinetServer::checkSysTime(QDateTime _time)
{
    QProcess pro;
    QString cmd = QString("date -s \"%1\"").arg(_time.toString("yyyy-MM-dd hh:mm:ss"));
    qDebug()<<"[checkSysTime]"<<cmd;
    pro.start(cmd);
    pro.waitForFinished(1000);
    emit timeUpdate();
//    pro.start("clock -w");
    //    pro.waitForFinished(1000);
}

void CabinetServer::requireListState()
{
    QByteArray qba = QString("{\"code\":\"%1\"}").arg(config->getCabinetId()).toUtf8();
    QString url = QString(SERVER_ADDR) + QString(API_REQ_LIST) +'?'+ qba.toBase64();
    reply_list_state = manager->get(QNetworkRequest(QUrl(url)));
    qDebug()<<"[requireListState]"<<url;
    qDebug()<<qba;
    connect(reply_list_state, SIGNAL(readyRead()), this, SLOT(recvListState()));

//    sysClock.start(60000);
//    connect(&sysClock, SIGNAL(timeout()), this, SLOT(sysTimeout()));
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

    if(reply_login != NULL)
    {
        reply_login->deleteLater();
    }
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

void CabinetServer::goodsAccess(CaseAddress addr, QString id, int num, int optType)
{qDebug()<<addr.cabinetSeqNUM<<id<<num<<optType;
    QString caseId = QString::number(config->getLockId(addr.cabinetSeqNUM, addr.caseIndex));
    QString cabinetId = config->getCabinetId();
    QByteArray qba;

    if(optType == 2)
        qba = QString("{\"li\":[{\"packageBarcode\":\"%1\",\"chesetCode\":\"%2\",\"goodsCode\":\"%3\",\"optType\":%4,\"optCount\":%5,\"barcode\":\"%6\"}]}")
             .arg(id).arg(cabinetId).arg(caseId).arg(2).arg(num).arg(barCode).toUtf8();
    else if(optType == 1)
        qba = QString("{\"li\":[{\"packageBarcode\":\"%1\",\"chesetCode\":\"%2\",\"goodsCode\":\"%3\",\"optType\":%4,\"optCount\":%5}]}")
             .arg(id).arg(cabinetId).arg(caseId).arg(1).arg(num).toUtf8();
    else if(optType == 3)
        qba = QString("{\"li\":[{\"packageBarcode\":\"%1\",\"chesetCode\":\"%2\",\"goodsCode\":\"%3\",\"optType\":%4,\"optCount\":%5}]}")
             .arg(id).arg(cabinetId).arg(caseId).arg(3).arg(num).toUtf8();

    QString nUrl = QString(SERVER_ADDR)+QString(API_GOODS_ACCESS)+"?"+qba.toBase64();
    qDebug()<<"[goodsAccess]"<<nUrl;
    qDebug()<<qba;
    reply_goods_access = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_goods_access, SIGNAL(finished()), this, SLOT(recvListAccess()));

}

void CabinetServer::listAccess(QStringList list, int optType)
{
    cJSON* json = cJSON_CreateObject();
    cJSON* jlist = cJSON_CreateArray();

    QString pack_bar;
    int i = 0;

    for(i=0; i<list.count(); i++)
    {
        pack_bar = list.at(i);
        QString pack_id = config->scanDataTrans(pack_bar);
        QByteArray packageBarcode = pack_bar.toLocal8Bit();
        QByteArray chesetCode = config->getCabinetId().toLocal8Bit();
        QByteArray barcode = barCode.toLocal8Bit();
        CaseAddress addr = config->checkCabinetByBarCode(pack_id);
        QByteArray goodsCode = QString::number(config->getLockId(addr.cabinetSeqNUM, addr.caseIndex)).toLocal8Bit();
        cJSON* obj = cJSON_CreateObject();
        cJSON_AddItemToObject(obj, "packageBarcode",cJSON_CreateString(packageBarcode.data()));
        cJSON_AddItemToObject(obj, "chesetCode", cJSON_CreateString(chesetCode.data()));
        cJSON_AddItemToObject(obj, "optType", cJSON_CreateNumber(optType));
        if(optType == 2)
        {
            cJSON_AddItemToObject(obj, "barcode", cJSON_CreateString(barcode.data()));
        }
        cJSON_AddItemToObject(obj, "goodsCode", cJSON_CreateString(goodsCode.data()));
        cJSON_AddItemToObject(obj, "optCount", cJSON_CreateNumber(1));
        cJSON_AddItemToArray(jlist, obj);
    }
    cJSON_AddItemToObject(json, "li",jlist);
    char* buff = cJSON_Print(json);
    cJSON_Delete(json);
    QByteArray qba = QByteArray(buff);

    QString nUrl = QString(SERVER_ADDR)+QString(API_GOODS_ACCESS)+"?"+qba.toBase64();
    qDebug()<<"[goodsAccess]"<<nUrl;
    qDebug()<<qba;
    reply_goods_access = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_goods_access, SIGNAL(finished()), this, SLOT(recvListAccess()));
    free(buff);
//    qDebug()<<"[list fetch]"<<cJSON_Print(json);
}

void CabinetServer::goodsCheck(QList<CabinetCheckItem *> l, CaseAddress addr)
{
    cJSON* json = cJSON_CreateObject();
    cJSON* jlist = cJSON_CreateArray();

    CabinetCheckItem* item;
    int i = 0;

    for(i=0; i<l.count(); i++)
    {
        item = l.at(i);

        QByteArray packageBarcode = item->itemId().toLocal8Bit();
        QByteArray chesetCode = config->getCabinetId().toLocal8Bit();
        QByteArray goodsCode = QString::number(config->getLockId(addr.cabinetSeqNUM, addr.caseIndex)).toLocal8Bit();
        int optCount = item->itemNum();
        cJSON* obj = cJSON_CreateObject();
        cJSON_AddItemToObject(obj, "packageBarcode",cJSON_CreateString(packageBarcode.data()));
        cJSON_AddItemToObject(obj, "chesetCode", cJSON_CreateString(chesetCode.data()));
        cJSON_AddItemToObject(obj, "optCount", cJSON_CreateNumber(optCount));
        cJSON_AddItemToObject(obj, "goodsCode", cJSON_CreateString(goodsCode.data()));

        cJSON_AddItemToArray(jlist, obj);
    }
    cJSON_AddItemToObject(json, "li",jlist);
    char* buff = cJSON_Print(json);
    cJSON_Delete(json);
    QByteArray qba = QByteArray(buff);

    QString nUrl = QString(SERVER_ADDR)+QString(API_GOODS_CHECK)+"?"+qba.toBase64();
    qDebug()<<"[goodsCheck]"<<nUrl;
    qDebug()<<qba;
    reply_goods_check = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_goods_check, SIGNAL(finished()), this, SLOT(recvGoodsCheck()));
    free(buff);
}

void CabinetServer::goodsListStore(QList<CabinetStoreListItem *> l)
{
    cJSON* json = cJSON_CreateObject();
    cJSON* jlist = cJSON_CreateArray();

    CabinetStoreListItem* goodsItem;
    int i = 0;
    int optType = 2;

    for(i=0; i<l.count(); i++)
    {
        goodsItem = l.at(i);
        QString pack_id = goodsItem->itemId();
        QByteArray packageBarcode = goodsItem->itemId().toLocal8Bit();
        QByteArray chesetCode = config->getCabinetId().toLocal8Bit();
        CaseAddress addr = config->checkCabinetByBarCode(pack_id);
        QByteArray barcode = barCode.toLocal8Bit();
        QByteArray goodsCode = QString::number(config->getLockId(addr.cabinetSeqNUM, addr.caseIndex)).toLocal8Bit();
        cJSON* obj = cJSON_CreateObject();
        cJSON_AddItemToObject(obj, "packageBarcode",cJSON_CreateString(packageBarcode.data()));
        cJSON_AddItemToObject(obj, "chesetCode", cJSON_CreateString(chesetCode.data()));
        cJSON_AddItemToObject(obj, "optType", cJSON_CreateNumber(optType));
        cJSON_AddItemToObject(obj, "goodsCode", cJSON_CreateString(goodsCode.data()));
        cJSON_AddItemToObject(obj, "optCount", cJSON_CreateNumber(goodsItem->itemNum()));
        cJSON_AddItemToObject(obj, "barcode", cJSON_CreateString(barcode.data()));
        cJSON_AddItemToArray(jlist, obj);
    }
    cJSON_AddItemToObject(json, "li",jlist);
    char* buff = cJSON_Print(json);
    cJSON_Delete(json);
    QByteArray qba = QByteArray(buff);

    QString nUrl = QString(SERVER_ADDR)+QString(API_GOODS_ACCESS)+"?"+qba.toBase64();
    qDebug()<<"[goodsAccess]"<<nUrl;
    qDebug()<<qba;
    reply_goods_access = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_goods_access, SIGNAL(finished()), this, SLOT(recvListAccess()));
    free(buff);
}

void CabinetServer::goodsCarScan()
{
    needReqCar = true;//打开定时查询
}

void CabinetServer::goodsBack(QString)
{
//    QByteArray qba = QString("{\"barcode\":\"%1\"}").arg(goodsId).toUtf8();
//    QString nUrl = QString(SERVER_ADDR)+QString(API_GOODS_BACK)+'?'+qba.toBase64();
//    qDebug()<<"[listCheck]"<<nUrl;
    return;
//    reply_goods_back = manager->get(QNetworkRequest(QUrl(nUrl)));
//    connect(reply_goods_back, SIGNAL(finished()), this, SLOT(recvGoodsBack()));
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
    reply_login = NULL;

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
                                   \"packageCount\": 20,\
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
                                \"packageCount\": 20,\
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
//    QByteArray qba = test;
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
            info->packageType = cJSON_GetObjectItem(json_info, "packageType")->valueint;
            info->roomName = QString::fromUtf8(cJSON_GetObjectItem(json_info,"roomName")->valuestring);
            info->singlePrice = cJSON_GetObjectItem(json_info,"singlePrice")->valueint;
            info->size = QString::fromUtf8(cJSON_GetObjectItem(json_info,"size")->valuestring);
//            info->takeCount = cJSON_GetObjectItem(json_info,"takeCount")->valueint;
            info->takeCount = cJSON_GetObjectItem(json_info,"packageCount")->valueint;
            info->totalNum = info->takeCount;
            info->unit = QString::fromUtf8(cJSON_GetObjectItem(json_info,"unit")->valuestring);
            qDebug()<<"[goods]"<<info->name<<info->goodsId<<info->takeCount<<info->unit;
            list->addGoods(info);
        }
        cJSON* json_list_info = cJSON_GetObjectItem(json_data,"store");
        list->barcode = QString::fromUtf8(cJSON_GetObjectItem(json_list_info, "barcode")->valuestring);
        if(config->getCabinetId() == QString::fromUtf8(cJSON_GetObjectItem(json_list_info, "departName")->valuestring))
        {
            emit listRst(list);
        }
        else
        {
            delete list;
            list = new GoodsList;
            emit listRst(list);
        }
    }
    cJSON_Delete(json);
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
        cJSON* data = cJSON_GetObjectItem(json, "data");
        QString goodsId = QString::fromUtf8(cJSON_GetObjectItem(data,"goodsId")->valuestring);
        int goodsNum = cJSON_GetObjectItem(data, "packageCount")->valueint;
        emit goodsNumChanged(goodsId, goodsNum);
    }
    else
    {
        emit accessFailed(QString(cJSON_GetObjectItem(json,"msg")->valuestring));
    }
    cJSON_Delete(json);
}

void CabinetServer::recvListAccess()
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
        goodsCarScan();
        qDebug()<<"ACCESS success";
        cJSON* data = cJSON_GetObjectItem(json, "data");
        int listCount = cJSON_GetArraySize(data);
        if(listCount <= 0)
        {
            cJSON_Delete(json);
            return;
        }
        int i=0;
        for(i=0; i<listCount; i++)
        {
            cJSON* item = cJSON_GetArrayItem(data, i);
            QString goodsId = QString::fromUtf8(cJSON_GetObjectItem(item,"goodsId")->valuestring);
            int goodsNum = cJSON_GetObjectItem(item, "packageCount")->valueint;
            emit goodsNumChanged(goodsId, goodsNum);
        }
    }
    else
    {
        emit accessFailed(QString(cJSON_GetObjectItem(json,"msg")->valuestring));
    }
    cJSON_Delete(json);
}

void CabinetServer::recvGoodsCheck()
{
    QByteArray qba = QByteArray::fromBase64(reply_goods_check->readAll());
    reply_goods_check->deleteLater();
    reply_goods_check = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<cJSON_Print(json);
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        emit goodsCheckRst(QString());
    }
    else
    {
        emit goodsCheckRst(QString(cJSON_GetObjectItem(json, "msg")->valuestring));
    }

    cJSON_Delete(json);
}

void CabinetServer::recvGoodsBack()
{

}

void CabinetServer::recvDateTime()
{
    QByteArray qba = QByteArray::fromBase64(reply_datetime->readAll());
    reply_datetime->deleteLater();
    reply_datetime = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<cJSON_Print(json);

    if(!json)
        return;

    cJSON* rst = cJSON_GetObjectItem(json, "success");

    if(rst->type != cJSON_True)
    {
        qDebug("[check time] failed");
        return;
    }
    timeIsChecked = true;

    rst = cJSON_GetObjectItem(json, "data");

//    cJSON* jsTime = cJSON_GetObjectItem(rst, "datetime_1");
    QString str(rst->valuestring);
//    qDebug()<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    checkSysTime(QDateTime::fromString(str,"yyyy-MM-dd hh:mm:ss"));
    //    qDebug()<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    cJSON_Delete(json);
}

void CabinetServer::recvListState()
{
    QByteArray qba = QByteArray::fromBase64(reply_list_state->readAll());
    reply_list_state->deleteLater();

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvListState]"<<cJSON_Print(json);

    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        cJSON* json_data = cJSON_GetObjectItem(json,"data");
        if(json_data->type == cJSON_NULL)
        {
            return;
        }
        int listSize = cJSON_GetArraySize(json_data);
        int i = 0;

        for(i=0; i<listSize; i++)
        {
            cJSON* item = cJSON_GetArrayItem(json_data, i);
            GoodsCar car;
            car.listId = QString(cJSON_GetObjectItem(item, "barcode")->valuestring);
            car.rfid = QString(cJSON_GetObjectItem(item, "code")->valuestring);
            qDebug()<<"[newGoodsCar]"<<car.rfid<<car.listId;
            emit newGoodsCar(car);
//            needReqCar = false;
        }
    }
}

void CabinetServer::sysTimeout()
{
    if(timeIsChecked)
        emit timeUpdate();
    else
        checkTime();
    if(needReqCar)
        requireListState();
}

