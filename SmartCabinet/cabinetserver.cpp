#include "cabinetserver.h"
#include <QString>
#include <QDebug>
#include <QTime>
#include <QUrl>
#include <QtGlobal>
#include <fcntl.h>
#include <unistd.h>
#include "defines.h"
#include "Device/controldevice.h"
#include "manager/signalmanager.h"
//#define RECV_DEBUG

//#define SERVER_ADDR "http://175.11.185.181"
#define SERVER_ADDR "http://120.77.159.8:8080"
#define API_REG "/sarkApi/SmartCheset/saveOrUpdate/"   //注册接口
#define API_INFO_UPLOAD ""    //柜子信息上传接口
#define API_INFO_REQ "/sarkApi/SmartCheset/query/"      //柜子信息查询接口
#define API_CLONE_REQ "/sarkApi/Cheset/syncCheset/"     //柜子克隆请求接口
#define API_CLONE_SYNC  ""      //柜子克隆数据同步接口
#define API_INSERT_COL "/sarkApi/Cheset/doUpdateChesetCol/"//列插入接口
#define API_LOGIN "/sarkApi/UserInfo/query/"  //登录接口
#define API_LIST_CHECK "/sarkApi/OutStorage/query/goods/" //送货单检查接口
#define API_LIST_STORE "/sarkApi/OutStorage/query/"      //存入完毕销单接口
#define API_LIST_STORE_SCAN    "/sarkApi/Cheset/trace/doGoods/"  //低值柜扫码存货接口
#define API_CAB_BIND "/sarkApi/Cheset/register/"     //柜格物品绑定接口
#define API_GOODS_ACCESS  "/sarkApi/Cheset/doGoods/"
//#define API_GOODS_CHECK  "/sarkApi/Cheset/doUpdataGoods/"     //盘点接口
#define API_GOODS_CHECK    "/sarkApi/Cheset/checkCheset/"      //盘点接口
#define API_CHECK_CREAT     "/sarkApi/TakeStockCheset/create/"       //创建盘点
#define API_CHECK_END       "/sarkApi/TakeStockCheset/end/"          //结束盘点
#define API_CHECK_TIME "/sarkApi/Time/query/"
#define API_REQ_LIST "/sarkApi/OutStorage/find/OutStorageCar/"      //查询是否有送货单在途中
#define API_LIST_CHECK_NEW "/sarkApi/OutStorage/queryfind/goods/"     //查询待存送货单接口NEW
#define API_LIST_CHECK_RFID "/sarkApi/OutStorage/find/all/rfid/OutStorage/" //查询待存rfid送货单
#define API_RFID_ACCESS "/sarkApi/Cheset/rfid/doGoods/"  //rfid存取接口
#define API_NETSTATE_CHECK "/websocket/"    //网络状态检查
#define API_CHECK_TABLES "/sarkApi/TakeStockCheset/query/takestockList/"    //查询盘点清单表
#define API_CHECK_INFO "/sarkApi/TakeStockCheset/query/takestockGoodsList/"    //查询盘点清单内容
#define API_SEARCH_SPELL "/sarkApi/Cheset/query/chesetGoods/"     //首字母搜索物品
#define API_GOODS_REPLY "/sarkApi/Cheset/doPleaseGoods/"  //请货
#define API_DAY_REPORT "/sarkApi/Cheset/query/consumeDate/"  //日清单
#define API_DOWNLOAD_PAC "/sarkApi/cheset/download/package" //下载更新包
#define API_VERSION_CHECK "/sarkApi/cheset/get/package"  //检查更新包
#define API_GOODS_TRACE  "/sarkApi/Cheset/doSaveTraceId"  //物品存入跟踪
#define API_RFID_CONSUME "/sarkApi/Cheset/rfid/consume/goods"   //登记消耗查询
#define API_OPERATION_REQUIRE "/sarkApi/Cheset/query/operate"   //手术单查询

#define API_AIO_OVERVIEW "/sarkApi/Cheset/collect/goods"  //一体机数据总览
#define API_AIO_EXPIRED "/sarkApi/Cheset/effective/goods" //一体机近效期物品
#define API_AIO_GOODS   "/sarkApi/Cheset/cabinet/query/goods" //科室物品
#define API_AIO_TODAY_IN    "/sarkApi/Cheset/take/goods"    //今日入库数据
#define API_AIO_TODAY_OUT   "/sarkApi/Cheset/consume/goods" //今日出库数据
#define API_AIO_WARNING_REP     "/sarkApi/Cheset/warn/goods" //智能柜库存预警

#define GET_JSON_QSTRING(SRC,KEY) QString::fromUtf8(cJSON_GetObjectItem(SRC, KEY)->valuestring)
#define GET_JSON_INT(SRC,KEY) cJSON_GetObjectItem(SRC, KEY)->valueint
#define GET_JSON_DOUBLE(SRC,KEY) cJSON_GetObjectItem(SRC, KEY)->valuedouble

#define API_AIO_TEMP_REPORT     "/sarkApi/Cheset/save/monitor/log"  //温度设备上报


CabinetServer::CabinetServer(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    cur_manager = new UserInfo();
    sqlManager = SqlManager::manager();

    cur_user = NULL;
    checkList = NULL;
    pacUpdate = NULL;
    reply_register = NULL;
    reply_login = NULL;
    reply_check_tables = NULL;
    reply_check_table_info = NULL;
    reply_list_check = NULL;
    reply_cabinet_bind = NULL;
    reply_goods_access = NULL;
    reply_goods_back = NULL;
    reply_goods_check = NULL;
    reply_goods_reply = NULL;
    reply_datetime = NULL;
    reply_list_state = NULL;
    reply_cabinet_info = NULL;
    reply_cabinet_clone = NULL;
    reply_update_col = NULL;
    reply_search_spell = NULL;
    reply_day_report = NULL;
    reply_download = NULL;
    reply_store_trace = NULL;
    reply_aio_overview = NULL;
    reply_aio_data = NULL;
    reply_rfid_sync = NULL;
    reply_rfid_access = NULL;
    reply_rfid_consume = NULL;
    reply_rfid_auto_store = NULL;
    reply_operation = NULL;
    reply_aio_temp = NULL;
    reply_camera = NULL;
    versionInfo = NULL;
    needClearBeforeClone = false;
    list_access_cache.clear();
    needReqCar = true;
    needSaveAddress = false;
    timeIsChecked = false;
    fWatchdog = -1;
    checkId = -1;
#ifndef SIMULATE_ON
    watchdogStart();
#endif
    connect(&tarProcess, SIGNAL(finished(int)), this, SLOT(tarFinished(int)));
    connect(&tarProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(processStandardOutput()));

    if(sqlManager->waitForSync())
        QTimer::singleShot(2000, this, SLOT(cabInfoSync()));

    SignalManager* sigMan = SignalManager::manager();
//    connect(sigMan, SIGNAL(epcAccess(QStringList, UserOpt)), this, SLOT(listAccess(QStringList, UserOpt)));
//    connect(this, SIGNAL(accessSuccess(QString)), sigMan, SIGNAL(accessSuccess(QString)));
//    connect(this, SIGNAL(accessFailed(QString)), sigMan, SIGNAL(accessFailed(QString)));
    connect(this, SIGNAL(rfidOptRst(bool,QString)),sigMan, SIGNAL(rfidOptRst(bool,QString)));
    connect(this, SIGNAL(epcInfoUpdate()), sigMan, SIGNAL(epcInfoUpdate()));
    connect(this, SIGNAL(epcConsumed(QStringList)), sigMan, SIGNAL(epcConsumed(QStringList)));
    connect(this, SIGNAL(operationInfoUpdate()), sigMan, SIGNAL(operationInfoUpdate()));
    connect(sigMan, SIGNAL(epcConsumeCheck(QStringList)), this, SLOT(rfidCheckConsume(QStringList)));
    connect(sigMan, SIGNAL(epcAccess(QStringList,UserOpt)), this, SLOT(rfidAccessOpt(QStringList,UserOpt)));
    connect(sigMan, SIGNAL(epcAccess(QStringList,QStringList, QString)), this, SLOT(rfidAccessOpt(QStringList,QStringList, QString)));
    connect(sigMan, SIGNAL(epcStore(QVariantMap)), this, SLOT(rfidAutoStore(QVariantMap)));
    connect(sigMan, SIGNAL(requireUpdateOperation()), this, SLOT(requireOperationInfo()));
}

CabinetServer::~CabinetServer()
{
    ::close(fWatchdog);
}

bool CabinetServer::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;

    if(config->getCabinetId().isEmpty())
        return true;

    ApiAddress = config->getServerAddress();
    qDebug()<<"[API MARK]"<<getApiMark();

    if(versionInfo != NULL)
    {
        delete versionInfo;
        versionInfo = NULL;
    }
//    if(ApiAddress.isEmpty())
//    {
//        ApiAddress = SERVER_ADDR;
//        config->setServerAddress(ApiAddress);
//    }
    checkTime();
//    config->getCabinetId();

//    if(config->getCabinetId().isEmpty())
//        cabRegister();
//    requireListState();
//    {
//        regId = "835999";
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
    while(regId.length() < 6)
    {
        regId.insert(0,'0');
    }
    qint64 timeStamp = getApiMark();
    QByteArray qba = QString("{\"code\":\"%1\",\"cabLayout\":\"%2\",\"location\":\"%3\",\"isRfidCode\":%4,\"mold\":%5,\"timeStamp\":%6}")
            .arg(regId)
            .arg(config->getCabinetLayout())
            .arg(config->getScreenConfig())
            .arg(config->getCabinetType().at(BIT_RFID)+1)
            .arg(config->getCabinetType().at(BIT_LOW_HIGH)+1)
            .arg(timeStamp).toUtf8();
    QString nUrl = ApiAddress+QString(API_REG);//+'?'+qba.toBase64();
    qDebug()<<"[cabRegister]"<<nUrl<<qba;
    replyCheck(reply_register);
    reply_register = post(nUrl, qba);//注册无离线处理
    connect(reply_register, SIGNAL(finished()), this, SLOT(recvCabRegister()));
}

/**
 * @brief CabinetServer::updateLayout
 * 以下情况更新柜子布局到后台:
 * 插入列
 * 柜子克隆
 */
void CabinetServer::updateLayout()
{
    qint64 timeStamp = getApiMark();
    QByteArray qba = QString("{\"code\":\"%1\",\"cabLayout\":\"%2\",\"location\":\"%3\",\"isRfidCode\":%4,\"mold\":%5,\"timeStamp\":%6}")
            .arg(config->getCabinetId())
            .arg(config->getCabinetLayout())
            .arg(config->getScreenConfig())
            .arg(config->getCabinetType().at(BIT_RFID)+1)
            .arg(config->getCabinetType().at(BIT_LOW_HIGH)+1)
            .arg(timeStamp).toUtf8();
    QString nUrl = ApiAddress+QString(API_REG);//+'?'+qba.toBase64();
    qDebug()<<"[updateLayout]"<<nUrl<<qba;
    replyCheck(reply_register);
    reply_register = post(nUrl, qba,timeStamp, true);//注册无离线处理
    connect(reply_register, SIGNAL(finished()), this, SLOT(recvCabRegister()));
}

void CabinetServer::checkTime()
{
    timeIsChecked = false;

    replyCheck(reply_datetime);

    QString url = ApiAddress + QString(API_CHECK_TIME);
//    qint64 timeStamp = getApiMark();
    reply_datetime = post(url, QByteArray());//同步时间无离线处理
//    reply_datetime = manager->get(QNetworkRequest(QUrl(url)));
    qDebug()<<"[checkTime]"<<url;
    connect(reply_datetime, SIGNAL(readyRead()), this, SLOT(recvDateTime()));
//    connect(reply_datetime, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(recvDateTimeError(QNetworkReply::NetworkError)));

    if(!sysClock.isActive())
    {
        sysClock.start(60000);
        connect(&sysClock, SIGNAL(timeout()), this, SLOT(sysTimeout()));
    }
//    netTimeStart();
}

void CabinetServer::checkSysTime(QDateTime _time)
{
    QProcess pro;
    QString cmd = QString("date -s \"%1\"").arg(_time.toString("yyyy-MM-dd hh:mm:ss"));
    qDebug()<<"[checkSysTime]"<<cmd;
    pro.start(cmd);
    pro.waitForFinished(1000);
//    emit timeUpdate();
//    pro.start("clock -w");
    //    pro.waitForFinished(1000);
}

void CabinetServer::requireListState()
{
    qint64 timeStamp = getApiMark();
    QByteArray qba = QString("{\"code\":\"%1\",\"timeStamp\":%2}").arg(config->getCabinetId()).arg(timeStamp).toUtf8();
    QString url = ApiAddress + QString(API_REQ_LIST);// +'?'+ qba.toBase64();

    replyCheck(reply_list_state);
    reply_list_state = post(url, qba);
//    reply_list_state = manager->get(QNetworkRequest(QUrl(url)));
    qDebug()<<"[requireListState]"<<url;
    qDebug()<<qba;
    connect(reply_list_state, SIGNAL(readyRead()), this, SLOT(recvListState()));

//    netTimeStart();
//    sysClock.start(60000);
    //    connect(&sysClock, SIGNAL(timeout()), this, SLOT(sysTimeout()));
}

void CabinetServer::replyCheck(QNetworkReply *reply)
{
    if(reply != NULL)
    {
        reply->deleteLater();
        reply = NULL;
    }
}

void CabinetServer::netTimeStart()
{
    netFlag = false;
    QTimer::singleShot(2000,this,SLOT(netTimeout()));
}

//本地离线重发
void CabinetServer::localCacheAccess()
{
    return;
    list_access_cache = config->getFetchList();

    accessLoop();
}

//离线重传接口，需要重构
void CabinetServer::accessLoop()
{
    if(list_access_cache.isEmpty())
        return;

    QByteArray qba = list_access_cache.takeFirst();
    QString nUrl = ApiAddress+QString(API_GOODS_ACCESS);//+"?"+qba;
    qDebug()<<"[accessLoop]"<<nUrl;
    qDebug()<<QByteArray::fromBase64(qba);
    replyCheck(reply_goods_access);
    reply_goods_access = post(nUrl, qba);
//    reply_goods_access = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_goods_access, SIGNAL(finished()), this, SLOT(recvListAccess()));
}

QString CabinetServer::getAbbName(QString fullName)
{
    if(fullName.indexOf("一次性") == 0)
    {
        fullName = fullName.remove(0,3);
    }
    if(fullName.indexOf("使用") == 0)
    {
        fullName = fullName.remove(0,2);
    }
    if(fullName.indexOf("医用") == 0)
    {
        fullName = fullName.remove(0,2);
    }

    return fullName;
}

void CabinetServer::watchdogStart()
{
    fWatchdog = -1;
    fWatchdog = open("/dev/watchdog", O_WRONLY);
    if (fWatchdog == -1)
    {
        qDebug()<<"[watchdog] watchdog start failed.";
        return;
    }
    qDebug()<<"[watchdog]"<<"start";
    watdogClock.start(10000);
    watchdogTimeout();
    connect(&watdogClock, SIGNAL(timeout()), this, SLOT(watchdogTimeout()));
}

QVariant CabinetServer::getCjsonItem(cJSON *json, QByteArray key, QVariant defaultRet)
{
    cJSON* j = cJSON_GetObjectItem(json, key.data());
    if(j == NULL)
        return defaultRet;

    if(j->type == cJSON_Number)
    {
        return QVariant(j->valuedouble);
    }
    else if(j->type == cJSON_String)
    {
        return QVariant(j->valuestring);
    }
    return defaultRet;
}

QString CabinetServer::autoCreateEpcInfo(QString code)
{
//    QStringList ret;
//    foreach (QString code, codes)
//    {
        QString epcHex = code.toLocal8Bit().toHex();
        QString epcStr = QString("%1").arg(epcHex, -24, '0');
//        ret<<epcStr;
//        qDebug()<<"[autoCreateEpcinfo]"<<code<<epcStr;
//    }
    return epcStr;
}

QNetworkReply *CabinetServer::post(QString url, QByteArray postData, qint64 timeStamp, bool need_resend)
{
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(url);
    if(timeStamp != 0)//timeStamp=0表示无需记录此次调用
        sqlManager->newApiLog(url, postData, timeStamp,need_resend);

    return manager->post(request, postData.toBase64());
}

qint64 CabinetServer::getApiMark()
{
    return QDateTime::currentDateTime().toMSecsSinceEpoch();
}

void CabinetServer::getServerAddr()
{
    qDebug("[getServerAddr]");
    ApiAddress = config->getServerAddress();// QString("http://") + addr;
    needSaveAddress = true;
//    config->setServerAddress(ApiAddress);

//    if(config->getCabinetId().isEmpty())
//        cabRegister();
//    {
//        regId = "835999";
//        config->setCabinetId(regId);
//    }
    checkTime();
}

void CabinetServer::userLogin(QString userId)
{
//#ifdef NO_SERVER
//    emit loginRst(true);
//#endif
    if(!config->netState)
        offlineLogin(userId);

    qint64 timeStamp = getApiMark();
    QByteArray qba = QString("{\"cardId\":\"%2\",\"departId\":\"%1\",\"timeStamp\":%3}").arg(config->getCabinetId()).arg(userId).arg(timeStamp).toUtf8();
    QString nUrl = ApiAddress+QString(API_LOGIN);//+'?'+qba.toBase64();
    qDebug()<<"[login]"<<nUrl;
    qDebug()<<qba;
    logId = userId;

//    if(reply_login)
//        return;
    replyCheck(reply_login);

    reply_login = post(nUrl, qba, timeStamp, false);//登录留下记录但不重新调用
//    reply_login = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_login, SIGNAL(finished()), this, SLOT(recvUserLogin()));
//    netTimeStart();


//    UserInfo* info = new UserInfo;
//    info->departName = QString();
//    info->cardId = userId;
//    info->name = QString("演示");
//    info->power = 0;
//    qDebug()<<"[recvUserLogin]"<<info->cardId<<info->power;
//    cur_user = info;
//    emit loginRst(info);
//    config->addUser(info);
//    config->wakeUp(TIMEOUT_BASE);
//    networkState = true;
}

void CabinetServer::listCheck(QString code)
{
    qint64 timeStamp = getApiMark();
    QByteArray qba = QString("{\"barcode\":\"%1\",\"timeStamp\":%2}").arg(code).arg(timeStamp).toUtf8();
    QString nUrl = ApiAddress+QString(API_LIST_CHECK_NEW);//+'?'+qba.toBase64();
    barCode = code;
    qDebug()<<"[listCheck]"<<nUrl<<qba;
    replyCheck(reply_list_check);
    reply_list_check = post(nUrl, qba, timeStamp, false);
//    reply_list_check = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_list_check, SIGNAL(finished()), this, SLOT(recvListCheck()));
}

void CabinetServer::cabInfoUpload()
{
    qint64 timeStamp = getApiMark();
    QByteArray qba = config->creatCabinetJson();
    QString nUrl = ApiAddress+QString(API_INFO_UPLOAD);//+"?"+qba.toBase64();
    qDebug()<<"[cabInfoUpload]"<<nUrl<<qba;
    replyCheck(reply_cabinet_info);
    reply_cabinet_info = post(nUrl, qba, timeStamp, false);
//    reply_cabinet_info = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_cabinet_info, SIGNAL(finished()), this, SLOT(recvInfoUploadResult()));
}

void CabinetServer::cabInfoReq()
{
    qint64 timeStamp = getApiMark();
    QByteArray qba = QString("{\"chesetCode\"\":%1\",\"timeStamp\":%2}").arg(config->getCabinetId()).arg(timeStamp).toUtf8();
    QString nUrl = ApiAddress+QString(API_INFO_REQ);//+"?"+qba.toBase64();
    qDebug()<<"[cabInfoReq]"<<nUrl<<qba;
}

void CabinetServer::cabCloneReq(QString oldCabinetId)
{
    qint64 timeStamp = getApiMark();
    regId = oldCabinetId;
    QByteArray qba = QString("{\"code\":\"%1\",\"timeStamp\":%2}").arg(oldCabinetId).arg(timeStamp).toUtf8();
    QString nUrl = ApiAddress+QString(API_CLONE_REQ);//+"?"+qba.toBase64();
    replyCheck(reply_cabinet_clone);
    reply_cabinet_clone = post(nUrl, qba, timeStamp);//manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_cabinet_clone, SIGNAL(finished()), this, SLOT(recvCabClone()));
    qDebug()<<"[cabCloneReq]"<<nUrl<<qba;
}

void CabinetServer::cabInfoSync()//同步柜子库存信息
{
    qint64 timeStamp = getApiMark();
    QString cabId = config->getCabinetId();
    QByteArray qba = QString("{\"code\":\"%1\",\"timeStamp\":%2}").arg(cabId).arg(timeStamp).toUtf8();
    QString nUrl = ApiAddress+QString(API_CLONE_REQ);//+"?"+qba.toBase64();
    needClearBeforeClone = true;
    replyCheck(reply_cabinet_clone);
    reply_cabinet_clone = post(nUrl, qba, timeStamp, false);
//    reply_cabinet_clone = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_cabinet_clone, SIGNAL(finished()), this, SLOT(recvCabSync()));
    qDebug()<<"[cabInfoSync]"<<nUrl<<qba;
}

void CabinetServer::cabColInsert(int pos, int num)
{
    qint64 timeStamp = getApiMark();
    QString cabId = config->getCabinetId();
    QByteArray qba = QString("{\"departCode\":\"%1\",\"col\":[\"%2\",\"%3\"],\"timeStamp\":%4}").arg(cabId).arg(pos).arg(num).arg(timeStamp).toUtf8();
    QString nUrl = ApiAddress+QString(API_INSERT_COL);//+"?"+qba.toBase64();
    replyCheck(reply_update_col);
    reply_update_col = post(nUrl, qba, timeStamp, false);
//    reply_update_col = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_update_col, SIGNAL(finished()), this, SLOT(recvColInsert()));
    qDebug()<<"[cabColInsert]"<<nUrl<<qba;
}

void CabinetServer::cabinetBind(int seqNum, int index, QString goodsId)
{
//    if(!config->netState)
//    {
//        emit bindRst(true);
//        return;
//    }
    qint64 timeStamp = getApiMark();
    QString optId = config->getOptId();
    QString caseId = QString::number(config->getLockId(seqNum, index));
    QString cabinetId = config->getCabinetId();
//    sqlManager->bindGoodsPackage(goodsId, seqNum, index);

    QByteArray qba = QString("{\"optName\":\"%6\",\"goodsId\":\"%1\",\"chesetCode\":\"%2\",\"goodsCode\":\"%3\",\"cabinetRow\":%4,\"cabinetCol\":%5,\"timeStamp\":%7}").arg(goodsId).arg(cabinetId).arg(caseId).arg(index).arg(seqNum).arg(optId).arg(timeStamp).toUtf8();
//    QByteArray qba = QString("{\"goodId\":\"%1\",\"chesetCode\":\"%2\",\"caseId\":\"%3\"}").arg(goodsId).arg(cabinetId).arg(caseId).toUtf8();
    QString nUrl = ApiAddress+QString(API_CAB_BIND);//+"?"+qba.toBase64();
    qDebug()<<"[cabinetBind]"<<nUrl<<"\n"<<qba;
    replyCheck(reply_cabinet_bind);
    reply_cabinet_bind = post(nUrl, qba, timeStamp);
//    reply_cabinet_bind = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_cabinet_bind, SIGNAL(finished()), this, SLOT(recvCabBind()));
}

//1取货2存货3退货16还货
void CabinetServer::goodsAccess(QPoint addr, QString id, int num, int optType)
{
    qDebug()<<addr.y()<<id<<num<<optType<<networkState;
    QString caseId = QString::number(config->getLockId(addr.x(), addr.y()));
    QString cabinetId = config->getCabinetId();
    QString optName = config->getOptId();
    qint64 timeStamp = getApiMark();
    QByteArray qba;

    if(optType == 2)
        qba = QString("{\"optName\":\"%7\",\"timeStamp\":%8,\"li\":[{\"packageBarcode\":\"%1\",\"chesetCode\":\"%2\",\"goodsCode\":\"%3\",\"optType\":%4,\"optCount\":%5,\"barcode\":\"%6\"}]}")
             .arg(id).arg(cabinetId).arg(caseId).arg(2).arg(num).arg(barCode).arg(optName).arg(timeStamp).toUtf8();
    else
        qba = QString("{\"optName\":\"%6\",\"timeStamp\":%7,\"li\":[{\"packageBarcode\":\"%1\",\"chesetCode\":\"%2\",\"goodsCode\":\"%3\",\"optType\":%4,\"optCount\":%5}]}")
             .arg(id).arg(cabinetId).arg(caseId).arg(optType).arg(num).arg(optName).arg(timeStamp).toUtf8();

//    QString nUrl = ApiAddress+QString(API_GOODS_ACCESS)+"?"+qba.toBase64();
    QString nUrl = ApiAddress+QString(API_GOODS_ACCESS);

    if(!networkState)
    {
        qDebug()<<"[offline access]"<<nUrl<<qba;
        config->saveFetchList(qba.toBase64());
    }
    else
    {
        qDebug()<<"[goodsAccess]"<<nUrl;
        qDebug()<<qba;
        replyCheck(reply_goods_access);
        reply_goods_access = post(nUrl, qba, timeStamp);
        connect(reply_goods_access, SIGNAL(finished()), this, SLOT(recvListAccess()));
    }
}

void CabinetServer::listAccess(QStringList list, int optType)//store:1  fetch:2 refund:3 back:16
{
    cJSON* json = cJSON_CreateObject();
    cJSON* jlist = cJSON_CreateArray();
    qint64 timeStamp = getApiMark();

    QString pack_bar;
    int i = 0;

    for(i=0; i<list.count(); i++)
    {
        pack_bar = list.at(i);
        QString pack_id = SqlManager::getPackageId(pack_bar);
        QByteArray packageBarcode = pack_bar.toLocal8Bit();
        QByteArray chesetCode = config->getCabinetId().toLocal8Bit();
        QByteArray barcode = barCode.toLocal8Bit();
        QPoint addr = SqlManager::searchByPackageId(pack_id);
        QByteArray goodsCode = QString::number(config->getLockId(addr.x(), addr.y())).toLocal8Bit();

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
        if(cur_user != NULL)
        {
            QByteArray optCard = cur_user->cardId.toLocal8Bit();
            QByteArray optTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss").toLocal8Bit();
            cJSON_AddItemToObject(obj, "name", cJSON_CreateString(optCard.data()));
            cJSON_AddItemToObject(obj, "takeOutTime", cJSON_CreateString(optTime.data()));
        }
        cJSON_AddItemToArray(jlist, obj);
    }
    cJSON_AddItemToObject(json, "li",jlist);
    QByteArray optId = config->getOptId().toLocal8Bit();
    cJSON_AddItemToObject(json,"optName", cJSON_CreateString(optId.data()));
    cJSON_AddItemToObject(json,"timeStamp", cJSON_CreateNumber(timeStamp));
    char* buff = cJSON_Print(json);

    cJSON_Delete(json);
    QByteArray qba = QByteArray(buff);

    if(!networkState)
    {
        config->saveFetchList(qba.toBase64());
        free(buff);
    }
    else
    {
//        QString nUrl = ApiAddress+QString(API_GOODS_ACCESS)+"?"+qba.toBase64();
        QString nUrl = ApiAddress+QString(API_GOODS_ACCESS);
        qDebug()<<"[listAccess]"<<nUrl;
        qDebug()<<qba;
        replyCheck(reply_goods_access);
//        reply_goods_access = manager->get(QNetworkRequest(QUrl(nUrl)));
        reply_goods_access = post(nUrl, qba, timeStamp);
        connect(reply_goods_access, SIGNAL(finished()), this, SLOT(recvListAccess()));
        free(buff);
    //    qDebug()<<"[list fetch]"<<cJSON_Print(json);
    }
}

void CabinetServer::goodsCheckReq()
{
    qint64 timeStamp = getApiMark();
    QString cabId = config->getCabinetId();
    QString optId = config->getOptId();
    QByteArray qba = QString("{\"departCode\":\"%1\",\"timeStamp\":%2,\"optName\":\"%3\"}").arg(cabId).arg(timeStamp).arg(optId).toUtf8();
    QString nUrl = ApiAddress+QString(API_CHECK_CREAT);//+"?"+qba.toBase64();
    replyCheck(reply_goods_check);
    reply_goods_check = post(nUrl, qba, timeStamp);
//    reply_goods_check = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_goods_check, SIGNAL(finished()), this, SLOT(recvCheckCreat()));
    qDebug()<<"[goodsCheckCreat]"<<nUrl<<qba;
}

void CabinetServer::goodsCheckFinish()
{
    if(checkId == -1)
        return;
    qint64 timeStamp = getApiMark();
    QString cabId = config->getCabinetId();
    QString optId = config->getOptId();
    QByteArray qba = QString("{\"departCode\":\"%1\",\"timeStamp\":%2,\"optName\":\"%3\"}").arg(cabId).arg(timeStamp).arg(optId).toUtf8();
    QString nUrl = ApiAddress+QString(API_CHECK_END);//+"?"+qba.toBase64();
    replyCheck(reply_goods_check);
    reply_goods_check = post(nUrl, qba, timeStamp);
//    reply_goods_check = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_goods_check, SIGNAL(finished()), this, SLOT(recvCheckFinish()));
    qDebug()<<"[goodsCheckFinish]"<<nUrl<<qba;
}

void CabinetServer::goodsCheck(QList<CabinetCheckItem *> l, QPoint addr)
{
    cJSON* json = cJSON_CreateObject();
    cJSON* packageList = cJSON_CreateArray();
    qint64 timeStamp = getApiMark();

    CabinetCheckItem* item;
    int i = 0;

    QByteArray chesetCode = config->getCabinetId().toLocal8Bit();
    QByteArray goodsCode = QString::number(config->getLockId(addr.x(), addr.y())).toLocal8Bit();
    cJSON_AddItemToObject(json, "departCode", cJSON_CreateString(chesetCode.data()));
    cJSON_AddItemToObject(json, "cabinetId", cJSON_CreateString(goodsCode.data()));
    cJSON_AddItemToObject(json, "timeStamp", cJSON_CreateNumber(timeStamp));

    for(i=0; i<l.count(); i++)
    {
        item = l.at(i);
        QByteArray packageBarcode = item->itemId().toLocal8Bit();

        cJSON* codeList = cJSON_CreateArray();
        cJSON* package = cJSON_CreateObject();
        cJSON_AddItemToObject(package, "packageType", cJSON_CreateString(packageBarcode.data()));

        int j = 0;
        for(j=0; j<item->list_fullId.count(); j++)
        {
            QByteArray qba = item->list_fullId.at(j).toLocal8Bit();
            cJSON_AddItemToArray(codeList, cJSON_CreateString(qba.data()));
        }

        cJSON_AddItemToObject(package, "packageCode", codeList);
        cJSON_AddItemToArray(packageList, package);
    }
    cJSON_AddItemToObject(json, "packageList",packageList);
    QByteArray optId = config->getOptId().toLocal8Bit();
    cJSON_AddItemToObject(json,"optName", cJSON_CreateString(optId.data()));

    char* buff = cJSON_Print(json);
    cJSON_Delete(json);
    QByteArray qba = QByteArray(buff);

    QString nUrl = ApiAddress+QString(API_GOODS_CHECK);
    qDebug()<<"[goodsCheck]"<<nUrl;
    qDebug()<<qba;
    replyCheck(reply_goods_check);
//    reply_goods_check = manager->get(QNetworkRequest(QUrl(nUrl)));
    reply_goods_check = post(nUrl, qba, timeStamp);
    connect(reply_goods_check, SIGNAL(finished()), this, SLOT(recvGoodsCheck()));
    free(buff);
}

void CabinetServer::goodsCheck(QStringList l, QPoint)
{
    qint64 timeStamp = getApiMark();
    cJSON* json = cJSON_CreateObject();
    cJSON* jlist = cJSON_CreateArray();
    int i = 0;

    QByteArray optId = config->getOptId().toLocal8Bit();
    cJSON_AddItemToObject(json,"optName", cJSON_CreateString(optId.data()));

    cJSON_AddItemToObject(json, "timeStamp", cJSON_CreateNumber(timeStamp));

    QByteArray chesetCode = config->getCabinetId().toLocal8Bit();
    cJSON_AddItemToObject(json, "departCode", cJSON_CreateString(chesetCode.data()));

    for(i=0; i<l.count(); i++)
    {
        QByteArray packageBarcode = l.at(i).toLocal8Bit();
        cJSON_AddItemToArray(jlist, cJSON_CreateString(packageBarcode.data()));
    }
    cJSON_AddItemToObject(json, "packageCode",jlist);
    char* buff = cJSON_Print(json);
    cJSON_Delete(json);
    QByteArray qba = QByteArray(buff);

    QString nUrl = ApiAddress+QString(API_GOODS_CHECK);
    qDebug()<<"[goodsCheck]"<<nUrl;
    qDebug()<<qba;
    replyCheck(reply_goods_check);
    reply_goods_check = post(nUrl, qba, timeStamp);
//    reply_goods_check = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_goods_check, SIGNAL(finished()), this, SLOT(recvGoodsCheck()));
    free(buff);
}

void CabinetServer::goodsListStoreScanAll(QString barcode ,QList<CabinetStoreListItem *> l)
{
    qint64 timeStamp = getApiMark();

    cJSON* json = cJSON_CreateObject();
    cJSON* jlist = cJSON_CreateArray();

    CabinetStoreListItem* goodsItem;
    int optType = 2;
    QByteArray chesetCode = config->getCabinetId().toLocal8Bit();
    QByteArray dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit();
    rejectList.clear();

    foreach (goodsItem, l)
    {
        QStringList itemCodes = goodsItem->itemGoods()->codes;
        QStringList itemCache = goodsItem->itemGoods()->scanCache;
        qDebug()<<"[ListStore]"<<"物品:"<<goodsItem->itemId()<<"扫描:"<<itemCache.count()<<"拒收:"<<goodsItem->waitNum();
        QByteArray packageBarcode = goodsItem->itemId().toLocal8Bit();
//        QByteArray _barcode = barCode.toLocal8Bit();
        QByteArray _barcode = barcode.toLocal8Bit();
        cJSON* obj = cJSON_CreateObject();
        cJSON_AddItemToObject(obj, "packageBarcode",cJSON_CreateString(packageBarcode.data()));
        cJSON_AddItemToObject(obj, "chesetCode", cJSON_CreateString(chesetCode.data()));
        cJSON_AddItemToObject(obj, "optType", cJSON_CreateNumber(optType));
        cJSON_AddItemToObject(obj, "optCount", cJSON_CreateNumber(itemCache.count()));
        cJSON_AddItemToObject(obj, "barcode", cJSON_CreateString(_barcode.data()));
        cJSON_AddItemToObject(obj ,"dateTime", cJSON_CreateString(dateTime.data()));
        cJSON* li_rfidCodes = cJSON_CreateArray();
        cJSON* li_refuseCodes = cJSON_CreateArray();

        foreach (QString code, itemCodes)
        {
            QByteArray bCode = code.toLocal8Bit();
            if(itemCache.contains(code))//扫描的标签
            {
                cJSON_AddItemToArray(li_rfidCodes, cJSON_CreateString(bCode.data()));
            }
            else//拒收的标签
            {
                rejectList<<code;
                cJSON_AddItemToArray(li_refuseCodes, cJSON_CreateString(bCode.data()));
            }
        }

        cJSON_AddItemToObject(obj, "rfidCodes", li_rfidCodes);
        cJSON_AddItemToObject(obj, "refuseCodes", li_refuseCodes);

        cJSON_AddItemToArray(jlist, obj);
    }
    cJSON_AddItemToObject(json, "li",jlist);

    QByteArray optId = config->getOptId().toLocal8Bit();
    cJSON_AddItemToObject(json,"optName", cJSON_CreateString(optId.data()));
    cJSON_AddItemToObject(json, "timeStamp", cJSON_CreateNumber(timeStamp));

    char* buff = cJSON_Print(json);
    cJSON_Delete(json);
    QByteArray qba = QByteArray(buff);

//    QString nUrl = ApiAddress+QString(API_GOODS_ACCESS)+"?"+qba.toBase64();
    QString nUrl = ApiAddress+QString(API_LIST_STORE_SCAN);
    qDebug()<<"[goodsListStoreScanAll]"<<nUrl;
    qDebug()<<qba;
    replyCheck(reply_goods_access);
//    reply_goods_access = manager->get(QNetworkRequest(QUrl(nUrl)));
    reply_goods_access = post(nUrl, qba, timeStamp);
    connect(reply_goods_access, SIGNAL(finished()), this, SLOT(recvListAccess()));
    free(buff);
}

void CabinetServer::goodsListStore(QString barcode, QList<CabinetStoreListItem *> l)
{
    qDebug("goodsListStore");
    qDebug()<<l.count();
    bool needScanAll = config->getStoreMode();
    if(needScanAll)
    {
        goodsListStoreScanAll(barcode, l);
        return;
    }

    qint64 timeStamp = getApiMark();

    cJSON* json = cJSON_CreateObject();
    cJSON* jlist = cJSON_CreateArray();

    CabinetStoreListItem* goodsItem;
    int i = 0;
    int optType = 2;

    for(i=0; i<l.count(); i++)
    {
        goodsItem = l.at(i);
        qDebug()<<goodsItem->itemId()<<goodsItem->itemNum();
        QString pack_id = goodsItem->itemId();
        QByteArray packageBarcode = goodsItem->itemId().toLocal8Bit();
        QByteArray chesetCode = config->getCabinetId().toLocal8Bit();
        QPoint addr = SqlManager::searchByPackageId(pack_id);
        QByteArray goodsCode = QString::number(config->getLockId(addr.x(), addr.y())).toLocal8Bit();
        QByteArray _barcode = barcode.toLocal8Bit();
        cJSON* obj = cJSON_CreateObject();
        cJSON_AddItemToObject(obj, "packageBarcode",cJSON_CreateString(packageBarcode.data()));
        cJSON_AddItemToObject(obj, "chesetCode", cJSON_CreateString(chesetCode.data()));
        cJSON_AddItemToObject(obj, "optType", cJSON_CreateNumber(optType));
        cJSON_AddItemToObject(obj, "goodsCode", cJSON_CreateString(goodsCode.data()));
        cJSON_AddItemToObject(obj, "optCount", cJSON_CreateNumber(goodsItem->itemNum()));
        cJSON_AddItemToObject(obj, "barcode", cJSON_CreateString(_barcode.data()));
        cJSON_AddItemToArray(jlist, obj);
    }
    cJSON_AddItemToObject(json, "li",jlist);

    QByteArray optId = config->getOptId().toLocal8Bit();
    cJSON_AddItemToObject(json,"optName", cJSON_CreateString(optId.data()));
    cJSON_AddItemToObject(json, "timeStamp", cJSON_CreateNumber(timeStamp));

    char* buff = cJSON_Print(json);
    cJSON_Delete(json);
    QByteArray qba = QByteArray(buff);

//    QString nUrl = ApiAddress+QString(API_GOODS_ACCESS)+"?"+qba.toBase64();
    QString nUrl = ApiAddress+QString(API_GOODS_ACCESS);
    qDebug()<<"[goodsListStore]"<<nUrl;
    qDebug()<<qba;
    replyCheck(reply_goods_access);
//    reply_goods_access = manager->get(QNetworkRequest(QUrl(nUrl)));
    reply_goods_access = post(nUrl, qba, timeStamp);
    connect(reply_goods_access, SIGNAL(finished()), this, SLOT(recvListAccess()));
    free(buff);
}

void CabinetServer::goodsStoreTrace(QString goodsCode)
{
    replyCheck(reply_store_trace);
    qint64 timeStamp = getApiMark();
    QString nUrl = ApiAddress+QString(API_GOODS_TRACE);
    QString optId = config->getOptId();
    QString chesetCode = config->getCabinetId();
    QString barcode = barCode;
    QString traceId = goodsCode;
    QByteArray qba = QString("{\"optName\":\"%1\",\"chesetCode\":\"%2\",\"traceId\":\"%3\",\"barcode\":\"%4\",\"timeStamp\":%5}")
             .arg(optId).arg(chesetCode).arg(traceId).arg(barcode).arg(timeStamp).toLocal8Bit();

    qDebug()<<"[goodsStoreTrace]"<<qba;
    reply_store_trace = post(nUrl, qba, timeStamp, false);
    connect(reply_store_trace, SIGNAL(finished()), this, SLOT(recvGoodsTrace()));
}

void CabinetServer::goodsCarScan()
{
    needReqCar = true;//打开定时查询
}

void CabinetServer::goodsBack(QString)
{
//    QByteArray qba = QString("{\"barcode\":\"%1\"}").arg(goodsId).toUtf8();
//    QString nUrl = ApiAddress+QString(API_GOODS_BACK)+'?'+qba.toBase64();
//    qDebug()<<"[ck]"<<nUrl;
    return;
//    reply_goods_back = manager->get(QNetworkRequest(QUrl(nUrl)));
//    connect(reply_goods_back, SIGNAL(finished()), this, SLOT(recvGoodsBack()));
}

void CabinetServer::requireCheckTables(QDate start, QDate finish)
{
    qint64 timeStamp = getApiMark();
    QString cabId = config->getCabinetId();
    QByteArray qba = QString("{\"departCode\":\"%1\",\"sTime\":\"%2 00:00:00\",\"eTime\":\"%3 23:59:59\",\"timeStamp\":%4}").arg(cabId).arg(start.toString("yyyy-MM-dd")).arg(finish.toString("yyyy-MM-dd")).arg(timeStamp).toUtf8();
    QString nUrl = ApiAddress+QString(API_CHECK_TABLES);//+"?"+qba.toBase64();
    replyCheck(reply_check_tables);
    reply_check_tables = post(nUrl, qba);
//    reply_check_tables = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_check_tables, SIGNAL(finished()), this, SLOT(recvCheckTables()));
    qDebug()<<"[requireCheckTables]"<<nUrl<<qba;
}

void CabinetServer::requireCheckTableInfo(QString id)
{
    qint64 timeStamp = getApiMark();
    QString cabId = config->getCabinetId();
    QByteArray qba = QString("{\"id\":\"%1\",\"timeStamp\":%2}").arg(id.toInt()).arg(timeStamp).toUtf8();
    QString nUrl = ApiAddress+QString(API_CHECK_INFO);//+"?"+qba.toBase64();
    replyCheck(reply_check_table_info);
    reply_check_table_info = post(nUrl, qba, timeStamp, false);
//    reply_check_table_info = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_check_table_info, SIGNAL(finished()), this, SLOT(recvCheckTableInfo()));
    qDebug()<<"[requireCheckTableInfo]"<<nUrl<<qba;
}

//获取日清单信息
void CabinetServer::requireListInfo(QDate sDate, QDate eDate)
{
    qint64 timeStamp = getApiMark();
    QString cabId = config->getCabinetId();
    QByteArray qba = QString("{\"departCode\":\"%1\", \"sTime\":\"%2\",\"eTime\":\"%3\",\"timeStamp\":%4}").arg(cabId).arg(sDate.toString("yyyy-MM-dd")).arg(eDate.toString("yyyy-MM-dd")).arg(timeStamp).toUtf8();
    QString nUrl = ApiAddress+QString(API_DAY_REPORT);//+"?"+qba.toBase64();
    replyCheck(reply_day_report);
    reply_day_report = post(nUrl, qba, timeStamp, false);
//    reply_day_report = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_day_report, SIGNAL(finished()), this, SLOT(recvDayReportInfo()));
    qDebug()<<"[requireListInfo]"<<nUrl<<qba;
}

void CabinetServer::cameraSnapshot()
{

}

void CabinetServer::requireAioOverview()
{
    qint64 timeStamp = getApiMark();
    QString cabId = config->getCabinetId();
    QByteArray qba = QString("{\"departCode\":\"%1\",\"timeStamp\":%2}").arg(cabId).arg(timeStamp).toUtf8();
    QString nUrl = ApiAddress+QString(API_AIO_OVERVIEW);
    replyCheck(reply_aio_overview);
    reply_aio_overview = post(nUrl, qba);
    connect(reply_aio_overview, SIGNAL(finished()), this, SLOT(recvAioOverview()));
    qDebug()<<"[requireAioOverview]"<<nUrl<<qba;
}

void CabinetServer::requireAioData(int cevent)
{
    qint64 timeStamp = getApiMark();
    QString cabId = config->getCabinetId();
    QByteArray qba = QString("{\"departCode\":\"%1\",\"timeStamp\":%2}").arg(cabId).arg(timeStamp).toUtf8();
    qDebug()<<"requireAioData"<<cevent;
    aio_state = (AIOMachine::cEvent)cevent;

    QString apiPath;
    switch(cevent)
    {
    case AIOMachine::click_num_expired:apiPath = QString(API_AIO_EXPIRED);
        break;
    case AIOMachine::click_num_goods:apiPath = QString(API_AIO_GOODS);
        break;
    case AIOMachine::click_num_today_in:apiPath = QString(API_AIO_TODAY_IN);
        break;
    case AIOMachine::click_num_today_out:apiPath = QString(API_AIO_TODAY_OUT);
        break;
    case AIOMachine::click_num_warning_rep:apiPath = QString(API_AIO_WARNING_REP);
        break;
    default:
        break;
    }

    QString nUrl = ApiAddress+apiPath;
    replyCheck(reply_aio_data);
    reply_aio_data = post(nUrl, qba);
    connect(reply_aio_data, SIGNAL(finished()), this, SLOT(recvAioData()));
    qDebug()<<"[requireAioData]"<<cevent<<nUrl<<qba;
}

void CabinetServer::checkUpdate(bool needConfirm)
{
    qint64 timeStamp = getApiMark();
    needConfirmUpdate = needConfirm;
    if(reply_download != NULL)
    {
        reply_download->deleteLater();
        if(pacUpdate!=NULL)
        {
            if(pacUpdate->isOpen())
                pacUpdate->close();
            pacUpdate->deleteLater();
        }
    }
    if(versionInfo == NULL)
        versionInfo = new VersionInfo(config->getCurVersion());
    QString nUrl = ApiAddress+QString(API_VERSION_CHECK);
    qDebug()<<nUrl;
    reply_download = post(nUrl, QByteArray(), timeStamp, false);
//    reply_download = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_download, SIGNAL(finished()), this, SLOT(recvVersionInfo()));
}

void CabinetServer::getUpdatePac(QString fileName)
{
    if(reply_download != NULL)
        return;

    QDir dir("/home/update/");
    if(!dir.exists())
        dir.mkdir("/home/update/");

    fileName = "/home/update/"+fileName;
    pacUpdate = new QFile(fileName);
    if(!pacUpdate->open(QFile::WriteOnly))
    {
        qDebug()<<"[getUpdatePac]:file open failed";
        return;
    }
    QString nUrl = ApiAddress+QString(API_DOWNLOAD_PAC);
    qDebug()<<nUrl;
    qint64 timeStamp = getApiMark();
    reply_download = post(nUrl, QByteArray(), timeStamp, false);
//    reply_download = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_download, SIGNAL(finished()), this, SLOT(updatePacFinish()));
    connect(reply_download, SIGNAL(readyRead()), this, SLOT(recvUpdatePac()));
}

void CabinetServer::updateStart()
{
    qDebug("[updatePacFinish]:package is legal,update start.");
    QString cmd = QString("tar -jxvf /home/update/%1 -C /home/update/").arg(versionInfo->pacFile);
    qDebug()<<cmd;
    tarProcess.start(cmd);
}

void CabinetServer::waitForRepaitOK()
{
    if(timeIsChecked)
        return;

    checkTime();
}

void CabinetServer::updateCurBarcode(QString code)
{
    barCode = code;
}

/**
 * @brief CabinetServer::rfidListSync 同步rfid柜的送货单信息
 *
 */
void CabinetServer::rfidListSync()
{
    qint64 timeStamp = getApiMark();
    QString cabId = config->getCabinetId();
    QByteArray qba = QString("{\"code\":\"%1\",\"timeStamp\":%2}").arg(cabId).arg(timeStamp).toLocal8Bit();

    QString nUrl = ApiAddress+QString(API_LIST_CHECK_RFID);//+"?"+qba.toBase64();
    replyCheck(reply_rfid_sync);
    reply_rfid_sync = post(nUrl, qba, timeStamp,false);
    connect(reply_rfid_sync, SIGNAL(finished()), this, SLOT(recvRfidListSync()));
    qDebug()<<"[rfidListSync]"<<nUrl<<qba;
}

void CabinetServer::rfidAccessOpt(QString storeListCode, QMap<QString, QStringList> storeGoods, UserOpt optType)
{
    qint64 timeStamp = getApiMark();
    QByteArray optName = cur_user->cardId.toLocal8Bit();
    QByteArray departCode = config->getCabinetId().toLocal8Bit();
    QByteArray barCode = storeListCode.toLocal8Bit();
    QByteArray dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit();

    cJSON* json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "optName", cJSON_CreateString(optName.data()));

    cJSON* li = cJSON_CreateArray();
    QMap<QString, QStringList>::iterator storeIt;
    for(storeIt=storeGoods.begin(); storeIt!=storeGoods.end(); storeIt++)
    {
        cJSON* li_info = cJSON_CreateObject();
        QByteArray packageBarcode = storeIt.key().toLocal8Bit();
        QStringList codes = storeIt.value();
        if(codes.isEmpty())
        {
            cJSON_Delete(li_info);
            continue;
        }

        cJSON_AddItemToObject(li_info ,"chesetCode", cJSON_CreateString(departCode.data()));
        cJSON_AddItemToObject(li_info ,"packageBarcode", cJSON_CreateString(packageBarcode.data()));
        cJSON_AddItemToObject(li_info ,"barcode", cJSON_CreateString(barCode.data()));
        cJSON_AddItemToObject(li_info ,"optType", cJSON_CreateNumber(optType));
        cJSON_AddItemToObject(li_info ,"optCount", cJSON_CreateNumber(codes.count()));
        cJSON_AddItemToObject(li_info ,"dateTime", cJSON_CreateString(dateTime.data()));

        cJSON* rfidCodes = cJSON_CreateArray();
        foreach (QString code, codes)
        {
            QByteArray bCode = code.toLocal8Bit();
            cJSON_AddItemToArray(rfidCodes, cJSON_CreateString(bCode.data()));
        }
        cJSON_AddItemToObject(li_info ,"rfidCodes", rfidCodes);

        cJSON_AddItemToArray(li, li_info);
    }

    QByteArray qba = QByteArray(cJSON_Print(json));
    cJSON_Delete(json);

    QString nUrl = ApiAddress+QString(API_RFID_ACCESS);//+"?"+qba.toBase64();
    replyCheck(reply_rfid_access);
    reply_rfid_access = post(nUrl, qba, timeStamp, false);
    connect(reply_rfid_access, SIGNAL(finished()), this, SLOT(recvRfidAccessRst()));
}

void CabinetServer::rfidAccessOpt(QStringList epcs, UserOpt optType)
{
    qint64 timeStamp = getApiMark();
    QByteArray optName = cur_user->cardId.toLocal8Bit();
    QByteArray departCode = config->getCabinetId().toLocal8Bit();
    QByteArray dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit();

    if(epcs.isEmpty())
        return;

    cJSON* json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "optName", cJSON_CreateString(optName.data()));

    cJSON* li = cJSON_CreateArray();

    foreach (QString epc, epcs)
    {
        QByteArray packageBarcode = epc.toLocal8Bit();

        cJSON* li_info = cJSON_CreateObject();

        cJSON_AddItemToObject(li_info ,"chesetCode", cJSON_CreateString(departCode.data()));
        cJSON_AddItemToObject(li_info ,"packageBarcode", cJSON_CreateString(packageBarcode.data()));
        cJSON_AddItemToObject(li_info ,"optType", cJSON_CreateNumber(optType));
        cJSON_AddItemToObject(li_info ,"optCount", cJSON_CreateNumber(1));
        cJSON_AddItemToObject(li_info ,"dateTime", cJSON_CreateString(dateTime.data()));

        cJSON_AddItemToArray(li, li_info);
    }

    cJSON_AddItemToObject(json, "li", li);
    cJSON_AddItemToObject(json, "timeStamp", cJSON_CreateNumber(timeStamp));
    QByteArray qba = QByteArray(cJSON_Print(json));
    cJSON_Delete(json);

    qDebug()<<"[rfidAccessOpt]"<<qba;
    QString nUrl = ApiAddress+QString(API_RFID_ACCESS);//+"?"+qba.toBase64();
    replyCheck(reply_rfid_access);
    reply_rfid_access = post(nUrl, qba, timeStamp, false);
    connect(reply_rfid_access, SIGNAL(finished()), this, SLOT(recvRfidAccessRst()));
}

void CabinetServer::rfidAccessOpt(QStringList fetchEpcs, QStringList backEpcs, QString optNo)
{
    qint64 timeStamp = getApiMark();
    QByteArray optName = QByteArray("UNKNOW");
    if(cur_user)
        optName = cur_user->cardId.toLocal8Bit();
    QByteArray departCode = config->getCabinetId().toLocal8Bit();
    QByteArray dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit();

    if(fetchEpcs.isEmpty() && backEpcs.isEmpty())
        return;

    cJSON* json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "optName", cJSON_CreateString(optName.data()));

    cJSON* li = cJSON_CreateArray();

    if(!fetchEpcs.isEmpty())
    {
        foreach (QString epc, fetchEpcs)
        {
            QByteArray packageBarcode = epc.toLocal8Bit();
            QByteArray surgeryBillNo = optNo.toLocal8Bit();

            cJSON* li_info = cJSON_CreateObject();

            cJSON_AddItemToObject(li_info ,"chesetCode", cJSON_CreateString(departCode.data()));
            cJSON_AddItemToObject(li_info ,"packageBarcode", cJSON_CreateString(packageBarcode.data()));
            cJSON_AddItemToObject(li_info ,"optType", cJSON_CreateNumber(opt_fetch));
            cJSON_AddItemToObject(li_info ,"optCount", cJSON_CreateNumber(1));
            cJSON_AddItemToObject(li_info ,"dateTime", cJSON_CreateString(dateTime.data()));
            cJSON_AddItemToObject(li_info , "sscSurgeryBillId", cJSON_CreateString(surgeryBillNo.data()));

            cJSON_AddItemToArray(li, li_info);
        }
    }
    if(!backEpcs.isEmpty())
    {
        foreach (QString epc, backEpcs)
        {
            QByteArray packageBarcode = epc.toLocal8Bit();
            cJSON* li_info = cJSON_CreateObject();

            cJSON_AddItemToObject(li_info ,"chesetCode", cJSON_CreateString(departCode.data()));
            cJSON_AddItemToObject(li_info ,"packageBarcode", cJSON_CreateString(packageBarcode.data()));
            cJSON_AddItemToObject(li_info ,"optType", cJSON_CreateNumber(opt_back));
            cJSON_AddItemToObject(li_info ,"optCount", cJSON_CreateNumber(1));
            cJSON_AddItemToObject(li_info ,"dateTime", cJSON_CreateString(dateTime.data()));

            cJSON_AddItemToArray(li, li_info);
        }
    }

    cJSON_AddItemToObject(json, "li", li);
    cJSON_AddItemToObject(json, "timeStamp", cJSON_CreateNumber(timeStamp));
    QByteArray qba = QByteArray(cJSON_Print(json));
    cJSON_Delete(json);

//    qDebug()<<"optNo:"<<optNo;
    qDebug()<<"[rfidAccessOpt]"<<qba;
    QString nUrl = ApiAddress+QString(API_RFID_ACCESS);//+"?"+qba.toBase64();
    replyCheck(reply_rfid_access);
    reply_rfid_access = post(nUrl, qba, timeStamp, false);
    connect(reply_rfid_access, SIGNAL(finished()), this, SLOT(recvRfidAccessRst()));
}

void CabinetServer::rfidAutoStore(QVariantMap reportMap)
{
    qint64 timeStamp = getApiMark();
    QByteArray optName = QByteArray("UNKNOW");
    if(cur_user)
        optName = cur_user->cardId.toLocal8Bit();
    QByteArray departCode = config->getCabinetId().toLocal8Bit();
//    QByteArray barCode = storeListCode.toLocal8Bit();
    QByteArray dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit();

    cJSON* json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "optName", cJSON_CreateString(optName.data()));

    cJSON* li = cJSON_CreateArray();
//    qDebug()<<"barcode count:"<<reportMap.uniqueKeys().count();
//    qDebug()<<reportMap.uniqueKeys();

    foreach (QString _barcode, reportMap.uniqueKeys())//按送货单遍历
    {
        qDebug()<<"barcode:"<<_barcode<<reportMap.values(_barcode).count();
        foreach (QVariant packageMap, reportMap.values(_barcode))//同送货单物品
        {
            qDebug()<<"package count:"<<packageMap.toMap().uniqueKeys().count();
//            qDebug()<<packageMap.toMap();
            foreach(QString packageId, packageMap.toMap().uniqueKeys())//按包ID遍历
            {
//                qDebug()<<packageId;
                QList<QVariant> codeList = packageMap.toMap().values(packageId);

                cJSON* li_info = cJSON_CreateObject();
                QByteArray packageBarcode = packageId.toLocal8Bit();
                QByteArray dataBarCode = _barcode.toLocal8Bit();//codeList.at(0).value("store_list").toByteArray();

                cJSON_AddItemToObject(li_info ,"chesetCode", cJSON_CreateString(departCode.data()));
                cJSON_AddItemToObject(li_info ,"packageBarcode", cJSON_CreateString(packageBarcode.data()));
                cJSON_AddItemToObject(li_info ,"barcode", cJSON_CreateString(dataBarCode.data()));
                cJSON_AddItemToObject(li_info ,"optType", cJSON_CreateNumber(opt_store));
                cJSON_AddItemToObject(li_info ,"optCount", cJSON_CreateNumber(codeList.count()));
                cJSON_AddItemToObject(li_info ,"dateTime", cJSON_CreateString(dateTime.data()));

                cJSON* rfidCodes = cJSON_CreateArray();
                foreach (QVariant codeInfo, codeList)
                {

                    QByteArray bCode = codeInfo.toMap().value("code").toByteArray();
                    cJSON_AddItemToArray(rfidCodes, cJSON_CreateString(bCode.data()));
//                    qDebug()<<bCode;
                }
                cJSON_AddItemToObject(li_info ,"rfidCodes", rfidCodes);
                cJSON_AddItemToArray(li, li_info);
            }
        }
    }
    cJSON_AddItemToObject(json, "li", li);
    cJSON_AddItemToObject(json, "timeStamp", cJSON_CreateNumber(timeStamp));

    QByteArray qba = QByteArray(cJSON_Print(json));
    cJSON_Delete(json);
    qDebug()<<"[rfidAutoStore]"<<qba;
    QString nUrl = ApiAddress+QString(API_RFID_ACCESS);//+"?"+qba.toBase64();
    replyCheck(reply_rfid_auto_store);
    reply_rfid_auto_store = post(nUrl, qba, timeStamp, false);
    connect(reply_rfid_auto_store, SIGNAL(finished()), this, SLOT(recvRfidStoreRst()));
}

void CabinetServer::rfidCheckConsume(QStringList epcs)
{
    QByteArray departCode = config->getCabinetId().toLocal8Bit();
    qint64 timeStamp = getApiMark();
    QByteArray qba = QString("{\"chesetCode\":\"%1\", \"rfidCodes\":[\"%2\"],\"timeStamp\":\"%3\"}").arg(departCode.data()).arg(epcs.join("\",\"")).arg(timeStamp).toLocal8Bit();
    QString nUrl = ApiAddress+QString(API_RFID_CONSUME);//+"?"+qba.toBase64();
    replyCheck(reply_rfid_consume);
    reply_rfid_consume = post(nUrl, qba, timeStamp, false);
    connect(reply_rfid_consume, SIGNAL(finished()), this, SLOT(recvRfidConsume()));
}

void CabinetServer::requireOperationInfo()
{
    QString departCode = config->getCabinetId().toLocal8Bit();
    qint64 timeStamp = getApiMark();
    QByteArray qba = QString("{\"departCode\":\"%1\",\"timeStamp\":\"%2\"}").arg(departCode).arg(timeStamp).toLocal8Bit();
    QString nUrl = ApiAddress+QString(API_OPERATION_REQUIRE);
    replyCheck(reply_operation);
    reply_operation = post(nUrl, qba, timeStamp, false);
    connect(reply_operation, SIGNAL(finished()), this, SLOT(recvOperationInfo()));
    qDebug()<<"[requireOperationInfo]"<<qba;
}

void CabinetServer::tempDevReport(QByteArray report)
{
//    qint64 timeStamp = getApiMark();
    QByteArray qba = report;
    QString nUrl = ApiAddress+QString(API_AIO_TEMP_REPORT);//+"?"+qba.toBase64();
    replyCheck(reply_aio_temp);
    reply_aio_temp = post(nUrl, qba, 0, false);
    connect(reply_aio_temp, SIGNAL(finished()), this, SLOT(recvTempDevReport()));
    qDebug()<<"[tempDevReport]"<<nUrl<<qba;
}

void CabinetServer::searchSpell(QString spell)
{
    qint64 timeStamp = getApiMark();
    QByteArray qba = QString("{\"spell\":\"%1\", \"departCode\":\"%2\"}").arg(spell).arg(config->getCabinetId()).toLocal8Bit();
    QString nUrl = ApiAddress+QString(API_SEARCH_SPELL);//+"?"+qba.toBase64();
    replyCheck(reply_search_spell);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(nUrl);
    reply_search_spell = post(nUrl, qba, timeStamp,false);
//    reply_search_spell = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_search_spell, SIGNAL(finished()), this, SLOT(recvSearchSpell()));
    qDebug()<<"[searchSpell]"<<nUrl<<qba;
}

void CabinetServer::replyRequire(QList<GoodsCheckInfo *> l)
{
    qint64 timeStamp = getApiMark();
    QByteArray optName = cur_user->cardId.toLocal8Bit();
    QByteArray departCode = config->getCabinetId().toLocal8Bit();

    cJSON* json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "optName", cJSON_CreateString(optName.data()));
    cJSON_AddItemToObject(json, "departCode", cJSON_CreateString(departCode.data()));
    cJSON_AddItemToObject(json, "timeStamp", cJSON_CreateNumber(timeStamp));

    cJSON* StoreGoodsModel = cJSON_CreateArray();
    foreach(GoodsCheckInfo* info, l)
    {
        cJSON* goods = cJSON_CreateObject();
        cJSON_AddItemToObject(goods, "goodsId", cJSON_CreateString(info->id.toLocal8Bit()));
        cJSON_AddItemToObject(goods, "goodsCount", cJSON_CreateNumber(info->num_pack*info->type));
        cJSON_AddItemToObject(goods, "packageBarcode", cJSON_CreateString(info->packageBarCode.toLocal8Bit()));
        cJSON_AddItemToObject(goods, "supplyId", cJSON_CreateNumber(info->supplyId));
        cJSON_AddItemToArray(StoreGoodsModel, goods);
    }
    cJSON_AddItemToObject(json, "li", StoreGoodsModel);
    QByteArray qba = QByteArray(cJSON_Print(json));
    cJSON_Delete(json);
    qDeleteAll(l.begin(), l.end());
    QString nUrl = ApiAddress+QString(API_GOODS_REPLY);//+"?"+qba.toBase64();
    replyCheck(reply_goods_reply);
    reply_goods_reply = post(nUrl, qba, timeStamp);
//    reply_goods_reply = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_goods_reply, SIGNAL(finished()), this, SLOT(recvGoodsReply()));
    qDebug()<<"[replyRequire]"<<nUrl<<qba;
}

void CabinetServer::recvCabRegister()
{
    QByteArray qba = QByteArray::fromBase64(reply_register->readAll());
    reply_register->deleteLater();
    reply_register = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvCabRegister]"<<cJSON_Print(json);

    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");

    if(json_rst->type == cJSON_True)
    {
        qDebug()<<"[Cabinet register]:success"<<regId;
        config->setCabinetId(regId);
        qDebug()<<"reg"<<config->getCabinetId();
        emit idUpdate();
        emit regResult(true);
    }
    else
    {
        emit regResult(false);
//        cabRegister();
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

    apiComplete(json);
    netFlag = true;
    emit netState(true);
    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        UserInfo* info = new UserInfo;
        cJSON* json_data = cJSON_GetObjectItem(json,"data");
        if(cJSON_GetArraySize(json_data) <= 0)
        {
            cJSON_Delete(json);
            return;
        }

        cJSON* json_info = cJSON_GetArrayItem(json_data,0);
        cJSON* dpt_name = cJSON_GetObjectItem(json_info, "departName");
        if(dpt_name != NULL)
        {
            info->departName = QString(dpt_name->valuestring);
        }
        else
        {
            info->departName = QString();
        }
//        info->id = cJSON_GetObjectItem(json_info,"id")->valueint;
        info->cardId = QString(cJSON_GetObjectItem(json_info,"cardId")->valuestring);
        info->departId = QString(cJSON_GetObjectItem(json_info,"departId")->valuestring);
        info->identityId = QString(cJSON_GetObjectItem(json_info,"identityId")->valuestring);
        info->name = QString(cJSON_GetObjectItem(json_info,"name")->valuestring);
        info->power = cJSON_GetObjectItem(json_info,"power")->valueint;
        info->tel = QString(cJSON_GetObjectItem(json_info,"tel")->valuestring);
        qDebug()<<"[recvUserLogin]:login success "<<info->cardId<<info->power;
        cur_user = info;
        emit loginRst(info);
        config->addUser(info);
        config->wakeUp(TIMEOUT_BASE);
        networkState = true;
        if(config->getCabinetType().at(BIT_RFID))
            rfidListSync();//同步入柜信息
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
    reply_list_check = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvListCheck]"<<cJSON_Print(json);

    if(!json)
        return;

    apiComplete(json);
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

        cJSON* json_store = cJSON_GetObjectItem(json_data,"store");
        list->barcode = QString(cJSON_GetObjectItem(json_store, "barcode")->valuestring);

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
            info->abbName = QString::fromUtf8(cJSON_GetObjectItem(json_info,"abbName")->valuestring);
            info->packageId = QString::fromUtf8(cJSON_GetObjectItem(json_info,"packageBarcode")->valuestring);
            info->packageType = cJSON_GetObjectItem(json_info, "packageType")->valueint;
            info->roomName = QString::fromUtf8(cJSON_GetObjectItem(json_info,"roomName")->valuestring);
            info->singlePrice = cJSON_GetObjectItem(json_info,"singlePrice")->valueint;
            info->size = QString::fromUtf8(cJSON_GetObjectItem(json_info,"size")->valuestring);
            info->proName = QString::fromUtf8(cJSON_GetObjectItem(json_info,"proName")->valuestring);
            info->supName = QString::fromUtf8(cJSON_GetObjectItem(json_info,"supplyName")->valuestring);
//            info->takeCount = cJSON_GetObjectItem(json_info,"takeCount")->valueint;
            info->takeCount = cJSON_GetObjectItem(json_info,"outCount")->valueint;//packageCount
            info->waitNum = info->takeCount;
            info->totalNum = info->takeCount;
            info->unit = QString::fromUtf8(cJSON_GetObjectItem(json_info,"unit")->valuestring);
            info->codes.clear();
            info->Py = config->getPyCh(info->name);
            cJSON* jCodes = cJSON_GetObjectItem(json_info, "traceIds");
            int idCount = cJSON_GetArraySize(jCodes);
            for(int j=0; j<idCount; j++)
            {
                info->codes << QString(cJSON_GetArrayItem(jCodes, j)->valuestring);
            }

            if(info->abbName == info->name)
            {
                info->abbName = getAbbName(info->name);
            }

            qDebug()<<"[goods]"<<info->name<<info->goodsId<<info->takeCount<<info->unit;
            list->addGoods(info);
            sqlManager->replaceGoodsInfo(info, list->barcode, SqlManager::no_rep, SqlManager::mask_all);
        }
        cJSON* json_list_info = cJSON_GetObjectItem(json_data,"store");
        list->barcode = QString::fromUtf8(cJSON_GetObjectItem(json_list_info, "barcode")->valuestring);
        list->departName = QString::fromUtf8(cJSON_GetObjectItem(json_list_info, "departCode")->valuestring);
        if(config->getCabinetId() == QString::fromUtf8(cJSON_GetObjectItem(json_list_info, "departName")->valuestring))
        {
            list->legalList = true;//合法送货单
            emit listRst(list);
        }
        else
        {
            list->legalList = false;//非法送货单
            emit listRst(list);
        }
    }
    cJSON_Delete(json);
}

void CabinetServer::recvCabBind()
{
    QByteArray qba = QByteArray::fromBase64(reply_cabinet_bind->readAll());
    reply_cabinet_bind->deleteLater();
    reply_cabinet_bind = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvCabBind]"<<cJSON_Print(json);

    if(!json)
        return;

    apiComplete(json);
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
    reply_goods_access = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<cJSON_Print(json);

    if(!json)
        return;

    apiComplete(json);
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
    accessLoop();
}

void CabinetServer::recvListAccess()
{
    QByteArray qba = QByteArray::fromBase64(reply_goods_access->readAll());
    reply_goods_access->deleteLater();
    reply_goods_access = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvListAccess]";
    qDebug()<<cJSON_Print(json);

    if(!json)
        return;

    apiComplete(json);

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        goodsCarScan();
        if(config->state == STATE_STORE)
            SqlManager::listStoreAffirm(barCode, SqlManager::remote_rep, rejectList);

        rejectList.clear();
        qDebug()<<"ACCESS success";
        cJSON* data = cJSON_GetObjectItem(json, "data");
        int listCount = cJSON_GetArraySize(data);
        if(listCount <= 0)
        {
            cJSON_Delete(json);
            return;
        }
        int i=0;
        SqlManager::begin();
        for(i=0; i<listCount; i++)
        {
            cJSON* item = cJSON_GetArrayItem(data, i);
            QString goodsId = QString::fromUtf8(cJSON_GetObjectItem(item,"goodsId")->valuestring);
            int goodsType = cJSON_GetObjectItem(item, "goodsType")->valueint;
            int goodsNum = cJSON_GetObjectItem(item, "packageCount")->valueint;
            float goodsPrice = cJSON_GetObjectItem(item, "price")->valuedouble;

            //goodsId=packageId
            if(goodsType<10)
                goodsId += "-0"+QString::number(goodsType);
            else
                goodsId += "-"+QString::number(goodsType);

            qDebug()<<goodsId<<goodsNum;
            emit goodsNumChanged(goodsId, goodsNum);
            emit accessSuccess(QString(cJSON_GetObjectItem(item,"msg")->valuestring));
            emit updateGoodsPrice(goodsPrice, goodsPrice*goodsType);
            if(config->state == STATE_BACK)
            {
                QString goodsName = QString::fromUtf8(cJSON_GetObjectItem(item, "goodsName")->valuestring);
                QString traceId = QString::fromUtf8(cJSON_GetObjectItem(item,"traceId")->valuestring);
                config->showMsg(QString("还货成功:%1\n%2").arg(traceId).arg(goodsName), false);
                QList<QVariantMap> backGoods;
                QVariantMap backMap;
                backMap.insert("code", traceId);
                backMap.insert("package_id", goodsId);
                backGoods<<backMap;
                SqlManager::replace("CodeInfo", backGoods);
//                qDebug()<<"replace"<<backMap;

                if(config->getCabinetType().at(BIT_RFID))//RFID模式下还需要插入RFID信息
                {
                    QString epc = autoCreateEpcInfo(traceId);
                    QList<QVariantMap> backEpcs;
                    QVariantMap backEpcMap;
                    backEpcMap.insert("epc_code", epc);
                    backEpcMap.insert("goods_code", traceId);
                    backEpcMap.insert("state", epc_in);
                    backEpcs<<backEpcMap;
                    SqlManager::replace("EpcInfo", backEpcs);
//                    qDebug()<<"replace"<<backEpcs;
                    emit rfidOptRst(true, QString("还货成功:%1 %2").arg(goodsName).arg(traceId));
                }
            }
        }
        SqlManager::commit();
        config->updateGoodsDisplay();
    }
    else
    {
        QString msg = QString(cJSON_GetObjectItem(json,"msg")->valuestring);
        emit accessFailed(msg);
        if(config->getCabinetType().at(BIT_RFID))//返回RFID操作信息
        {
            emit rfidOptRst(false, msg);
        }
    }
    cJSON_Delete(json);

    accessLoop();
}

void CabinetServer::recvGoodsTrace()
{
    QByteArray qba = QByteArray::fromBase64(reply_store_trace->readAll());
    reply_store_trace->deleteLater();
    reply_store_trace = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvGoodsTrace]"<<cJSON_Print(json);
    if(!json)
        return;

    apiComplete(json);
    QString msg = QString(cJSON_GetObjectItem(json, "msg")->valuestring);
    QString goodsCode = QString(cJSON_GetObjectItem(json, "data")->valuestring);
    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    emit goodsTraceRst(json_rst->type, msg, goodsCode);
}

void CabinetServer::recvGoodsCheck()
{
    QByteArray qba = QByteArray::fromBase64(reply_goods_check->readAll());
    reply_goods_check->deleteLater();
    reply_goods_check = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvGoodsCheck]"<<cJSON_Print(json);
    if(!json)
        return;

    apiComplete(json);
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

void CabinetServer::recvCheckCreat()
{
    QByteArray qba = QByteArray::fromBase64(reply_goods_check->readAll());
    reply_goods_check->deleteLater();
    reply_goods_check = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvCheckCreat]"<<cJSON_Print(json);
    if(!json)
        return;

    apiComplete(json);
    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        emit checkCreatRst(true, QString(cJSON_GetObjectItem(json, "msg")->valuestring));
        cJSON* jData = cJSON_GetObjectItem(json, "data");
        checkId = cJSON_GetObjectItem(jData,"id")->valueint;
    }
    else
    {
        emit checkCreatRst(false, QString(cJSON_GetObjectItem(json, "msg")->valuestring));
    }

    cJSON_Delete(json);
}

void CabinetServer::recvCheckFinish()
{
    QByteArray qba = QByteArray::fromBase64(reply_goods_check->readAll());
    reply_goods_check->deleteLater();
    reply_goods_check = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvCheckFinish]"<<cJSON_Print(json);
    if(!json)
        return;

    apiComplete(json);
    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        qDebug()<<"check finished.";
        if(checkList != NULL)
            delete checkList;
        checkList = new CheckList();
        cJSON* jData = cJSON_GetObjectItem(json,"data");
        checkList->setCheckDateTime(QString(cJSON_GetObjectItem(jData, "strTime")->valuestring));
        checkList->departCode = QString(cJSON_GetObjectItem(jData, "departCode")->valuestring);
//        checkList->departCode = QString(cJSON_GetObjectItem(jData, "departCode")->valuestring);
        cJSON* jInfo = cJSON_GetObjectItem(jData, "goodsChesetEntityList");
        int listSize = cJSON_GetArraySize(jInfo);
        for(int i=0; i<listSize; i++)
        {
            cJSON* jItem = cJSON_GetArrayItem(jInfo, i);
            if(jItem == NULL)
                break;

            GoodsCheckInfo* info = new GoodsCheckInfo();
            info->id = QString(cJSON_GetObjectItem(jItem, "goodsId")->valuestring);
            info->name = QString(cJSON_GetObjectItem(jItem, "goodsName")->valuestring);
            info->goodsSize = QString(cJSON_GetObjectItem(jItem, "size")->valuestring);
            info->num_in = cJSON_GetObjectItem(jItem, "inCount")->valueint;
            info->num_out = cJSON_GetObjectItem(jItem, "outCount")->valueint;
            info->num_back = cJSON_GetObjectItem(jItem, "backCount")->valueint;
            info->num_cur = cJSON_GetObjectItem(jItem, "currCount")->valueint;
            checkList->addInfo(info);
        }
        emit curCheckList(checkList);
        checkFinish(true);
        //盘点后同步
        cabInfoSync();
    }
    else
    {
        checkFinish(false);
//        qDebug()<<"check not finished, resend after 30s.";
//        QTimer::singleShot(3000, this, SLOT(goodsCheckFinish()));
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

    apiComplete(json);
    netFlag = true;
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
//    getUpdatePac("test.tar.bz2");

    return;
//    if(!needSaveAddress)
//        return;
//    needSaveAddress = false;
//    config->setServerAddress(ApiAddress);

//    if(config->getCabinetId().isEmpty())
//        cabRegister();
//    {
//        regId = "835999";
//        config->setCabinetId(regId);
//    }
}

void CabinetServer::recvDateTimeError(QNetworkReply::NetworkError code)
{
    qDebug()<<code;
    config->clearConfig();
}

void CabinetServer::recvListState()
{
    QByteArray qba = QByteArray::fromBase64(reply_list_state->readAll());
    reply_list_state->deleteLater();
    reply_list_state = NULL;


    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvListState]"<<cJSON_Print(json);

    if(!json)
    {
        cJSON_Delete(json);
        return;
    }
    apiComplete(json);
    netFlag = true;
    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        cJSON* json_data = cJSON_GetObjectItem(json,"data");
        if(json_data->type == cJSON_NULL)
        {
            cJSON_Delete(json);
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

    cJSON_Delete(json);
}

void CabinetServer::recvInfoUploadResult()
{

}

void CabinetServer::recvCabClone()
{
    QByteArray qba = QByteArray::fromBase64(reply_cabinet_clone->readAll());
    reply_cabinet_clone->deleteLater();
    reply_cabinet_clone = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvCabClone]"<<cJSON_Print(json);
//    return;
    if(!json)
    {
        cJSON_Delete(json);
        return;
    }
    apiComplete(json);
    netFlag = true;
    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        cJSON* json_data = cJSON_GetObjectItem(json,"data");
        if(json_data->type == cJSON_NULL)
        {
            cJSON_Delete(json);
            return;
        }
        int listSize = cJSON_GetArraySize(json_data);
        int i = 0;

        SqlManager::sqlDelete();
        config->clearGoodsConfig();

        for(i=0; i<listSize; i++)
        {
            cJSON* item = cJSON_GetArrayItem(json_data, i);
            Goods* info = new Goods;

            int row = QString(cJSON_GetObjectItem(item, "cabinetRow")->valuestring).toInt();
            int col = QString(cJSON_GetObjectItem(item, "cabinetCol")->valuestring).toInt();
            info->abbName = QString(cJSON_GetObjectItem(item, "abbName")->valuestring);
            info->name = QString(cJSON_GetObjectItem(item, "goodsName")->valuestring);
            info->num = cJSON_GetObjectItem(item,"packageCount")->valueint;
            info->outNum = 0;
            info->goodsId = QString(cJSON_GetObjectItem(item, "goodsId")->valuestring);
            info->packageType = cJSON_GetObjectItem(item, "goodsType")->valueint;
            info->unit = QString(cJSON_GetObjectItem(item, "unit")->valuestring);
            info->size = QString(cJSON_GetObjectItem(item, "size")->valuestring);
            info->Py = config->getPyCh(info->name);//qDebug()<<"[PY]"<<info->Py;
            info->packageId = info->goodsId;
            info->pos = QPoint(col, row);
//            info->row = row;
//            info->col = col;

            //codes
            info->codes.clear();
            cJSON* jTraceIds = cJSON_GetObjectItem(item, "traceIds");
            int idCount = cJSON_GetArraySize(jTraceIds);
            for(int j=0; j<idCount; j++)
            {
                info->codes << QString(cJSON_GetArrayItem(jTraceIds, j)->valuestring);
            }

            if(info->packageType<10)
                info->packageId += "-0"+QString::number(info->packageType);
            else
                info->packageId += "-"+QString::number(info->packageType);

            if(info->abbName.isEmpty())
                info->abbName = getAbbName(info->name);
//            qDebug()<<"[newGoods]"<<row<<col<<info->name<<info->abbName<<info->id<<info->packageId<<info->num<<info->unit;
//            config->insertGoods(info, row, col);
            sqlManager->replaceGoodsInfo(info, SqlManager::all_rep, SqlManager::mask_all);//只更新远程物品状态

            //创建默认rfid信息
            if(config->getCabinetType().at(BIT_RFID))
            {
                QList<QVariantMap> epcList;
                SqlManager::begin();
//                QStringList epcs = autoCreateEpcInfo(info->codes);
                foreach (QString code, info->codes)
                {
                    QVariantMap epcMap;
                    QString epc = autoCreateEpcInfo(code);
                    epcMap.insert("epc_code", epc);
                    epcMap.insert("goods_code", code);
                    epcMap.insert("state", epc_in);
                    epcList<<epcMap;
                }
                SqlManager::replace("EpcInfo", epcList);
                SqlManager::commit();
            }
//            if(col < config->list_cabinet.count() && col >= 0)
//                config->list_cabinet[col]->updateCase(row);
        }
    }
    else
    {
        emit cloneResult(false,"柜子编码无效");
        return;
    }

    cJSON_Delete(json);
    config->configInit();
    config->setCabinetId(regId);
    updateLayout();//更新柜子布局到后台
    config->updateGoodsDisplay();
    emit cabinetCreated();
    emit cloneResult(true,"智能柜数据克隆成功");
}

void CabinetServer::recvCabSync()
{
    QByteArray qba = QByteArray::fromBase64(reply_cabinet_clone->readAll());
    reply_cabinet_clone->deleteLater();
    reply_cabinet_clone = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvCabSync]"<<cJSON_Print(json);
//    return;
    if(!json)
    {
        cJSON_Delete(json);
        emit cabSyncResult(false);
        return;
    }
    apiComplete(json);
    netFlag = true;
    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        cJSON* json_data = cJSON_GetObjectItem(json,"data");
        if(json_data->type == cJSON_NULL)
        {
            cJSON_Delete(json);
            return;
        }
        if(needClearBeforeClone)
        {
            needClearBeforeClone = false;
            SqlManager::sqlDelete();
            config->clearGoodsConfig();
            emit insertRst(true);
        }
        int listSize = cJSON_GetArraySize(json_data);
        int i = 0;

        for(i=0; i<listSize; i++)
        {
            cJSON* item = cJSON_GetArrayItem(json_data, i);
            Goods* info = new Goods;

            int row = QString(cJSON_GetObjectItem(item, "cabinetRow")->valuestring).toInt();
            int col = QString(cJSON_GetObjectItem(item, "cabinetCol")->valuestring).toInt();
            info->abbName = QString(cJSON_GetObjectItem(item, "abbName")->valuestring);
            info->name = QString(cJSON_GetObjectItem(item, "goodsName")->valuestring);
            info->num = cJSON_GetObjectItem(item,"packageCount")->valueint;
            info->outNum = 0;
            info->goodsId = QString(cJSON_GetObjectItem(item, "goodsId")->valuestring);
            info->packageType = cJSON_GetObjectItem(item, "goodsType")->valueint;
            info->unit = QString(cJSON_GetObjectItem(item, "unit")->valuestring);
            info->size = QString(cJSON_GetObjectItem(item, "size")->valuestring);
            info->Py = config->getPyCh(info->name);//qDebug()<<"[PY]"<<info->Py;
//            info->size = GET_JSON_QSTRING(item, "size");
//            info->proName = GET_JSON_QSTRING(item, "proName");
            info->packageId = info->goodsId;
//            info->row = row;
//            info->col = col;
            info->pos = QPoint(col, row);

            info->codes.clear();
            cJSON* jTraceIds = cJSON_GetObjectItem(item, "traceIds");
            int idCount = cJSON_GetArraySize(jTraceIds);
            for(int j=0; j<idCount; j++)
            {
                info->codes << QString(cJSON_GetArrayItem(jTraceIds, j)->valuestring);
            }

            if(info->packageType<10)
                info->packageId += "-0"+QString::number(info->packageType);
            else
                info->packageId += "-"+QString::number(info->packageType);

            if(info->abbName.isEmpty())
                info->abbName = getAbbName(info->name);

            qDebug()<<"[newGoods]"<<row<<col<<info->name<<info->abbName<<info->goodsId<<info->packageId<<info->num<<info->unit;
//            config->syncGoods(info, row, col);
            sqlManager->replaceGoodsInfo(info, SqlManager::all_rep, SqlManager::mask_all);//只更新远程物品状态

            //添加默认RFID
            if(config->getCabinetType().at(BIT_RFID))
            {
                QList<QVariantMap> epcList;
                SqlManager::begin();
//                QStringList epcs = autoCreateEpcInfo(info->codes);
                foreach (QString code, info->codes)
                {
                    QVariantMap epcMap;
                    QString epc = autoCreateEpcInfo(code);
                    epcMap.insert("epc_code", epc);
                    epcMap.insert("goods_code", code);
                    epcMap.insert("state", epc_in);
                    epcList<<epcMap;
                }
                SqlManager::replace("EpcInfo", epcList);
                SqlManager::commit();
            }

            if(col < config->list_cabinet.count() && col >= 0)
                config->list_cabinet[col]->updateCase(row);
        }
    }
    else
    {
        emit cabSyncResult(false);
        return;
    }
    emit cabSyncResult(true);
    cJSON_Delete(json);
}

void CabinetServer::recvColInsert()
{
    QByteArray qba = QByteArray::fromBase64(reply_update_col->readAll());
    reply_update_col->deleteLater();
    reply_update_col = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvColInsert]"<<cJSON_Print(json);
//    return;
    if(!json)
    {
        cJSON_Delete(json);
        return;
    }
    netFlag = true;
    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
//        config->clearGoodsConfig();
        needClearBeforeClone = true;
        cabInfoSync();
    }
    else
    {
        emit insertRst(false);
    }
}

void CabinetServer::recvCheckTables()
{
    QByteArray qba = QByteArray::fromBase64(reply_check_tables->readAll());
    reply_check_tables->deleteLater();
    reply_check_tables = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvCheckTables]"<<cJSON_Print(json);
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        cJSON* jData = cJSON_GetObjectItem(json, "data");
        int listSize = cJSON_GetArraySize(jData);
        QList<CheckTableInfo*> l;

        for(int i=0; i<listSize; i++)
        {
            cJSON* jItem = cJSON_GetArrayItem(jData, i);
            if(jItem == NULL)
                break;

            CheckTableInfo* info = new CheckTableInfo;
            info->id = QString::number(cJSON_GetObjectItem(jItem, "id")->valueint);
            info->sTime = QString(cJSON_GetObjectItem(jItem, "sTime")->valuestring);
            info->eTime = QString(cJSON_GetObjectItem(jItem, "eTime")->valuestring);

            l<<info;
        }
        emit checkTables(l);
    }
    else
    {

    }

    cJSON_Delete(json);
}

void CabinetServer::recvCheckTableInfo()
{
    QByteArray qba = QByteArray::fromBase64(reply_check_table_info->readAll());
    reply_check_table_info->deleteLater();
    reply_check_table_info = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvCheckTableInfo]"<<cJSON_Print(json);
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        if(checkList != NULL)
            delete checkList;
        checkList = new CheckList();
        cJSON* jData = cJSON_GetObjectItem(json,"data");
//        checkList->setCheckDateTime(QString(cJSON_GetObjectItem(jData, "strTime")->valuestring));
//        checkList->departCode = QString(cJSON_GetObjectItem(jData, "departCode")->valuestring);
//        checkList->departCode = QString(cJSON_GetObjectItem(jData, "departCode")->valuestring);
        cJSON* jInfo = jData;//cJSON_GetObjectItem(jData, "goodsChesetEntityList");
        int listSize = cJSON_GetArraySize(jInfo);

        for(int i=0; i<listSize; i++)
        {
            cJSON* jItem = cJSON_GetArrayItem(jInfo, i);
            if(jItem == NULL)
                break;

            GoodsCheckInfo* info = new GoodsCheckInfo();
            info->id = QString(cJSON_GetObjectItem(jItem, "goodsId")->valuestring);
            info->name = QString(cJSON_GetObjectItem(jItem, "goodsName")->valuestring);
            info->goodsSize = QString(cJSON_GetObjectItem(jItem, "size")->valuestring);
            info->num_in = cJSON_GetObjectItem(jItem, "inCount")->valueint;
            info->num_out = cJSON_GetObjectItem(jItem, "outCount")->valueint;
            info->num_back = cJSON_GetObjectItem(jItem, "backCount")->valueint;
            info->num_cur = cJSON_GetObjectItem(jItem, "currCount")->valueint;
            checkList->addInfo(info);
        }
        emit curCheckList(checkList);
    }
    else
    {
//        qDebug()<<"check not finished, resend after 30s.";
//        QTimer::singleShot(3000, this, SLOT(goodsCheckFinish()));
    }

    cJSON_Delete(json);
}

void CabinetServer::recvSearchSpell()
{
    QByteArray qba = QByteArray::fromBase64(reply_search_spell->readAll());
    reply_search_spell->deleteLater();
    reply_search_spell = NULL;
    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvSearchSpell]"<<cJSON_Print(json);
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        if(checkList != NULL)
            delete checkList;
        checkList = new CheckList();
        cJSON* jData = cJSON_GetObjectItem(json,"data");
        cJSON* jInfo = jData;
        int listSize = cJSON_GetArraySize(jInfo);

        for(int i=0; i<listSize; i++)
        {
            cJSON* jItem = cJSON_GetArrayItem(jInfo, i);
            if(jItem == NULL)
                break;

            GoodsCheckInfo* info = new GoodsCheckInfo();
            info->id = QString(cJSON_GetObjectItem(jItem, "b2bNum")->valuestring);//物品ID
            info->packageBarCode = QString(cJSON_GetObjectItem(jItem, "packageBarcode")->valuestring);
            info->name = QString(cJSON_GetObjectItem(jItem, "name")->valuestring);//物品名
            info->goodsSize = QString(cJSON_GetObjectItem(jItem, "size")->valuestring);//规格
            info->num_cur = cJSON_GetObjectItem(jItem, "goodsCount")->valueint;//库存
            info->unit = cJSON_GetObjectItem(jItem, "unit")->valueint;//单位
            info->producerName = cJSON_GetObjectItem(jItem, "producerName")->valuestring;//生产商
            info->type = QString(cJSON_GetObjectItem(jItem, "packageType")->valuestring).toInt();//type
            info->supplyId = cJSON_GetObjectItem(jItem, "supplyId")->valueint;
            info->supplyName = cJSON_GetObjectItem(jItem, "supplyName")->valuestring;
            if(info->type == 0)
                info->type = 1;
            checkList->addInfo(info);
        }
        emit curSearchList(checkList);
    }
    else
    {

    }

    cJSON_Delete(json);
}

void CabinetServer::recvGoodsReply()
{
    QByteArray qba = QByteArray::fromBase64(reply_goods_reply->readAll());
    reply_goods_reply->deleteLater();
    reply_goods_reply = NULL;
    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvGoodsReply]"<<cJSON_Print(json);
    if(!json)
        return;

    apiComplete(json);
    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        emit goodsReplyRst(true, "请货成功");
    }
    else
    {
        emit goodsReplyRst(false, QString(cJSON_GetObjectItem(json, "msg")->valuestring));
    }

    cJSON_Delete(json);
}

void CabinetServer::recvDayReportInfo()
{
    QByteArray qba = QByteArray::fromBase64(reply_day_report->readAll());
//    QFile testFile("/home/dayreport.test");
//    if(!testFile.open(QFile::ReadOnly))
//    {
//        qDebug("day report open failed");
//    }
//    QByteArray qba = testFile.readAll();
    reply_day_report->deleteLater();
    reply_day_report = NULL;
    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvDayReportInfo]"<<cJSON_Print(json);
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    QList<DayReportInfo*> rst;
    if(json_rst->type == cJSON_True)
    {
        cJSON* data = cJSON_GetObjectItem(json, "data");
        int arraySize = cJSON_GetArraySize(data);
        for(int i=0; i<arraySize; i++)
        {
            cJSON* item = cJSON_GetArrayItem(data, i);
            DayReportInfo* info = new DayReportInfo;
            info->goodsId = QString(cJSON_GetObjectItem(item, "goodsId")->valuestring);
            info->goodsName = QString(cJSON_GetObjectItem(item, "goodsName")->valuestring);
            info->size = QString(cJSON_GetObjectItem(item, "size")->valuestring);
            info->proName = QString(cJSON_GetObjectItem(item, "proName")->valuestring);
            info->goodsCount = QString(cJSON_GetObjectItem(item, "goodsCount")->valuestring);
            info->optTime = QString(cJSON_GetObjectItem(item, "optTime")->valuestring);
            info->traceId = QString(cJSON_GetObjectItem(item, "traceId")->valuestring);
            info->unit = QString(cJSON_GetObjectItem(item, "unit")->valuestring);
            info->batchNumber = QString(cJSON_GetObjectItem(item, "batchNumber")->valuestring);
            info->optName = QString(cJSON_GetObjectItem(item, "optName")->valuestring);
            info->supplyName = QString(cJSON_GetObjectItem(item, "supplyName")->valuestring);
            info->price = cJSON_GetObjectItem(item, "price")->valuedouble;
            info->sumCount = cJSON_GetObjectItem(item, "sumCount")->valuedouble;
            info->state = cJSON_GetObjectItem(item, "state")->valueint;
            rst<<info;
        }
        emit dayReportRst(rst, QString(cJSON_GetObjectItem(json, "msg")->valuestring));
    }
    else
    {
        emit dayReportRst(rst, QString(cJSON_GetObjectItem(json, "msg")->valuestring));
    }

    cJSON_Delete(json);
}

void CabinetServer::recvVersionInfo()
{
    QByteArray qba = QByteArray::fromBase64(reply_download->readAll());
    qDebug()<<"[recvVersionInfo]"<<qba;
    if(versionInfo == NULL)
        return;
    if(versionInfo->needUpdate(qba))
    {
        reply_download->deleteLater();
        reply_download = NULL;
        getUpdatePac(versionInfo->pacFile);
    }
    else
    {
        emit updateCheckRst(false, "当前最新版本");
    }
}

void CabinetServer::recvUpdatePac()
{
    QByteArray qba =  reply_download->readAll();
//    qDebug()<<"[recvUpdatePac]"<<qba.size();
//    qDebug()<<qba;
    pacUpdate->write(qba);
}

void CabinetServer::recvAioOverview()
{
    QByteArray qba = QByteArray::fromBase64(reply_aio_overview->readAll());
    reply_aio_overview->deleteLater();
    reply_aio_overview = NULL;
    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvAioOverview]"<<cJSON_Print(json);
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    cJSON* json_data = cJSON_GetObjectItem(json,"data");
    QString msg = QString::fromUtf8(cJSON_GetObjectItem(json, "msg")->valuestring);
    AIOOverview* ret = NULL;

    if(json_rst->type == cJSON_True)
    {
        ret = new AIOOverview;
        ret->chesetGoodsCount = cJSON_GetObjectItem(json_data, "chesetGoodsCount")->valueint;
        ret->lifeTimeGoodsCount = cJSON_GetObjectItem(json_data, "lifeTimeGoodsCount")->valueint;
        ret->warnGoodsCount = cJSON_GetObjectItem(json_data, "warnGoodsCount")->valueint;
        ret->inSumCount = cJSON_GetObjectItem(json_data, "inSumCount")->valuedouble;
        ret->outSumCount = cJSON_GetObjectItem(json_data, "outSumCount")->valuedouble;
    }
    emit aioOverview(msg, ret);
}

void CabinetServer::recvAioData()
{
    QByteArray qba = QByteArray::fromBase64(reply_aio_data->readAll());
    reply_aio_data->deleteLater();
    reply_aio_data = NULL;
    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvAioData]"<<cJSON_Print(json);
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    cJSON* json_data = cJSON_GetObjectItem(json,"data");
    QString msg = QString::fromUtf8(cJSON_GetObjectItem(json, "msg")->valuestring);
    QList<Goods*> ret;

    if(json_rst->type == cJSON_True)
    {
       int dataSize = cJSON_GetArraySize(json_data);
       for(int i=0; i<dataSize; i++)
       {
            cJSON* jGoods = cJSON_GetArrayItem(json_data, i);
            Goods* info = new Goods;
            info->name = getCjsonItem(jGoods, "goodsName", QString()).toString();
            info->goodsId = getCjsonItem(jGoods, "goodsId", QString()).toString();
            info->size = getCjsonItem(jGoods, "size", QString()).toString();
            info->proName = getCjsonItem(jGoods, "producerName", QString()).toString();
            info->supName = getCjsonItem(jGoods, "supplyName", QString()).toString();
            info->unit = getCjsonItem(jGoods, "unit", QString()).toString();
            info->aioInNum = getCjsonItem(jGoods, "inCount", QString()).toString();
            info->aioOutNum = getCjsonItem(jGoods, "goodsCount", QString()).toString();
            info->packageType = getCjsonItem(jGoods, "packageType", 1).toInt();
            info->threshold = getCjsonItem(jGoods, "threshold", 0).toInt();
            info->maxThreshold = getCjsonItem(jGoods, "maxThreshold", 0).toInt();
            info->batch = getCjsonItem(jGoods, "batchNumber", QString()).toString();
            info->lifeDay = getCjsonItem(jGoods, "lifeDay", 0).toInt();
            info->productTime = getCjsonItem(jGoods, "productTime", QString()).toString();
            info->lifeTime = getCjsonItem(jGoods, "lifeTime", QString()).toString();
            info->packageCount = getCjsonItem(jGoods, "packageCount", 0).toInt();
            info->goodsCount = getCjsonItem(jGoods, "goodsCount", 0).toInt();
            info->price = getCjsonItem(jGoods, "price", 0.0).toFloat();
            info->sumCount = getCjsonItem(jGoods, "sumCount", 0.0).toFloat();
            info->optName = getCjsonItem(jGoods, "optName", QString()).toString();
            info->optTime = getCjsonItem(jGoods, "optTime", QString()).toString();
            info->traceId = getCjsonItem(jGoods, "traceId", QString()).toString();
            ret<<info;
       }
    }
    else
    {
        ret.clear();
    }
    emit aioData(msg, aio_state, ret);
}

void CabinetServer::recvRfidListSync()
{
    QByteArray qba = QByteArray::fromBase64(reply_rfid_sync->readAll());
    reply_rfid_sync->deleteLater();
    reply_rfid_sync = NULL;
    cJSON* json = cJSON_Parse(qba.data());
//    qDebug()<<"[recvRfidListSync]"<<cJSON_Print(json);
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    QString msg = QString::fromUtf8(cJSON_GetObjectItem(json, "msg")->valuestring);

    if(json_rst->type == cJSON_False)//失败
    {
        qDebug()<<"[recvRfidListSync] error:"<<msg;
        return;
    }
    qDebug()<<"[recvRfidListSync]:success";

    cJSON* data = cJSON_GetObjectItem(json, "data");//data []
    int szData = cJSON_GetArraySize(data);
//    QList<StoreList*> listGroup;
    SqlManager::begin();//开始更新数据

    for(int i=0; i<szData; i++)//data[i]
    {
//        StoreList* list = new StoreList;
        cJSON* dataItem = cJSON_GetArrayItem(data, i);//data->{}
        QMap<QString ,QVariantMap> goodsList;//物品信息
        QMap<QString ,QVariantMap> codeList;//条码信息,insert multi
        QList<QVariantMap> epcList;//rfid信息

        //读取rfid信息
        cJSON* rfidCode = cJSON_GetObjectItem(dataItem, "rfidCode");//data->{rfidCode[]}
        int arraySize = cJSON_GetArraySize(rfidCode);
        for(int j=0; j<arraySize; j++)//rfidCode[j]
        {
            cJSON* jRfidInfo = cJSON_GetArrayItem(rfidCode, j);//rfidCode[]->{}
            QVariantMap codeMap;//need pro_name sup_name
            codeMap.insert("code", GET_JSON_QSTRING(jRfidInfo, "traceId"));
            codeMap.insert("package_id", GET_JSON_QSTRING(jRfidInfo, "packageBarcode"));
            codeMap.insert("store_list", GET_JSON_QSTRING(jRfidInfo, "barcode"));
//            codeMap.insert("epc_code", GET_JSON_QSTRING(jRfidInfo, "rfidCode"));
            codeMap.insert("batch_number", GET_JSON_QSTRING(jRfidInfo, "batchNumber"));

            codeList.insertMulti(codeMap["package_id"].toString() , codeMap);

            QVariantMap epcMap;
            epcMap.insert("epc_code", GET_JSON_QSTRING(jRfidInfo, "rfidCode").toUpper());
            epcMap.insert("goods_code", GET_JSON_QSTRING(jRfidInfo, "traceId"));

            epcList<<epcMap;
//            QString rfCode = QString::fromUtf8(cJSON_GetObjectItem(jRfidInfo, "rfidCode")->valuestring);
//            QString code = GET_JSON_QSTRING(jRfidInfo, "traceId");
//            QString packageBarcode = QString::fromUtf8(cJSON_GetObjectItem(jRfidInfo, "packageBarcode")->valuestring);
//            list->rfidMap.insert(rfCode, code);
//            list->codeMap.insert(code, packageBarcode);
        }

        //读取物品信息
        cJSON* goods = cJSON_GetObjectItem(dataItem, "goods");//data->{goods[]}
        arraySize = cJSON_GetArraySize(goods);

        for(int j=0; j<arraySize; j++)//goods[j]
        {
//            SqlManager::getPubQuery()->prepare("REPLACE INTO GoodsInfo (package_id, goods_id, package_type, name, abbname, size, unit, single_price, pro_name, sup_name) "
//                                      "VALUES(:package_id, :goods_id, :package_type, :name, :abbname, :size, :unit, :single_price, :pro_name, :sup_name)");
            cJSON* jGoodsInfo = cJSON_GetArrayItem(goods, j);//goods[]->{}
//            Goods* goodsInfo = new Goods;
//            goodsInfo->name = GET_JSON_QSTRING(jGoodsInfo, "name");
//            goodsInfo->goodsId = GET_JSON_QSTRING(jGoodsInfo,"goodsId");
//            goodsInfo->size = GET_JSON_QSTRING(jGoodsInfo, "size");
//            goodsInfo->unit = GET_JSON_QSTRING(jGoodsInfo, "unit");
//            goodsInfo->packageCount = GET_JSON_INT(jGoodsInfo, "packageCount");
//            goodsInfo->price = GET_JSON_DOUBLE(jGoodsInfo, "singlePrice");
//            goodsInfo->packageId = GET_JSON_QSTRING(jGoodsInfo, "packageBarcode");
//            goodsInfo->packageType = GET_JSON_INT(jGoodsInfo, "packageType");
//            goodsInfo->batch = GET_JSON_QSTRING(jGoodsInfo, "batchNUmber");
//            goodsInfo->proName = GET_JSON_QSTRING(jGoodsInfo, "proName");
//            goodsInfo->lifeTime = GET_JSON_QSTRING(jGoodsInfo, "lifeTime");
//            goodsInfo->abbName = GET_JSON_QSTRING(jGoodsInfo, "abbName");
//            goodsInfo->supName = GET_JSON_QSTRING(jGoodsInfo, "supplyName");
//            list->goodsMap.insert(goodsInfo->packageId, goodsInfo);
            QVariantMap goodsMap;
            goodsMap.insert("package_id",GET_JSON_QSTRING(jGoodsInfo, "packageBarcode"));
            goodsMap.insert("goods_id",GET_JSON_QSTRING(jGoodsInfo, "goodsId"));
            goodsMap.insert("package_type",GET_JSON_INT(jGoodsInfo, "packageType"));
            goodsMap.insert("name",GET_JSON_QSTRING(jGoodsInfo, "name"));
            goodsMap.insert("abbname",GET_JSON_QSTRING(jGoodsInfo, "abbName"));
            goodsMap.insert("size",GET_JSON_QSTRING(jGoodsInfo, "size"));
            goodsMap.insert("unit",GET_JSON_QSTRING(jGoodsInfo, "unit"));
            goodsMap.insert("single_price",GET_JSON_DOUBLE(jGoodsInfo, "singlePrice"));
            goodsMap.insert("pro_name",GET_JSON_QSTRING(jGoodsInfo, "proName"));
            goodsMap.insert("sup_name",GET_JSON_QSTRING(jGoodsInfo, "supplyName"));
            goodsList.insert(goodsMap["package_id"].toString(), goodsMap);

            //pro_name sup_name
            QMap<QString, QVariantMap>::iterator it;
            for(it=codeList.begin(); it!=codeList.end(); it++)
            {
                if(it.key() == goodsMap["package_id"].toString())
                {
                    QVariantMap codeInfo = it.value();
                    codeInfo.insert("pro_name", goodsMap["pro_name"]);
                    codeInfo.insert("sup_name", goodsMap["sup_name"]);
                }
            }

//            codeList[goodsMap["package_id"].toString()]["pro_name"] = goodsMap["pro_name"];
//            codeList[goodsMap["package_id"].toString()]["sup_name"] = goodsMap["sup_name"];
        }
        //更新物品信息
        SqlManager::replace("GoodsInfo", goodsList.values());
        SqlManager::replace("CodeInfo", codeList.values());
        SqlManager::insert("EpcInfo", epcList);
        emit epcInfoUpdate();
        qDebug()<<"[recvRfidListSync]:new"<<codeList.count();

        //自动存物品
//        rfidAutoStore(codeList);
#if 0
        //读取存货单信息
        cJSON* store = cJSON_GetObjectItem(dataItem, "store");//data->{store}
        list->barCode = GET_JSON_QSTRING(store, "barcode");
        list->departName = GET_JSON_QSTRING(store, "departCode");//柜子号
        list->departCode = GET_JSON_QSTRING(store, "departName");//科室
        list->hosName = GET_JSON_QSTRING(store, "hosName");//医院

        //打印时间
        list->printTime = QString::fromUtf8(cJSON_GetObjectItem(dataItem, "printTime")->valuestring);
        listGroup<<list;
#endif
    }
    SqlManager::commit();
}

void CabinetServer::recvRfidAccessRst()
{
    QByteArray qba = QByteArray::fromBase64(reply_rfid_access->readAll());
    reply_rfid_access->deleteLater();
    reply_rfid_access = NULL;
    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvRfidAccessRst]"<<cJSON_Print(json);
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    QString msg = QString::fromUtf8(cJSON_GetObjectItem(json, "msg")->valuestring);
    emit aioMsg(msg);
    if(json_rst->type == cJSON_True)
    {
        if(msg.isEmpty())
        {
            msg = QString("物品操作成功");
        }
        emit rfidOptRst(true, msg);
        SqlManager::commit();
    }
    else
    {
        emit rfidOptRst(false, msg);
        SqlManager::rollback();
    }
}

void CabinetServer::recvRfidStoreRst()
{
    QByteArray qba = QByteArray::fromBase64(reply_rfid_auto_store->readAll());
    reply_rfid_auto_store->deleteLater();
    reply_rfid_auto_store = NULL;
    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvRfidStoreRst]"<<cJSON_Print(json);
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    QString msg = QString::fromUtf8(cJSON_GetObjectItem(json, "msg")->valuestring);
    if(json_rst->type == cJSON_True)
    {
        SqlManager::commit();
        if(msg.isEmpty())
        {
            msg = QString("物品存入成功");
        }
        emit rfidOptRst(true, msg);
    }
    else
    {
        SqlManager::rollback();
        emit rfidOptRst(false, msg);
    }
}

void CabinetServer::recvRfidConsume()
{
    QByteArray qba = QByteArray::fromBase64(reply_rfid_consume->readAll());
    reply_rfid_consume->deleteLater();
    reply_rfid_consume = NULL;
    cJSON* json = cJSON_Parse(qba.data());
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    QString msg = QString::fromUtf8(cJSON_GetObjectItem(json, "msg")->valuestring);
    QStringList consumedEpcs;
    if(json_rst->type == cJSON_True)
    {
        cJSON* data = cJSON_GetObjectItem(json, "data");
        int count = cJSON_GetArraySize(data);
        for(int i=0; i<count; i++)
        {
            consumedEpcs<<QString::fromUtf8(cJSON_GetArrayItem(data, i)->valuestring);
        }
        emit epcConsumed(consumedEpcs);
    }
    else
    {
        qWarning()<<"[recvRfidConsume]"<<msg;
        return;
    }
}

void CabinetServer::recvOperationInfo()
{
    QByteArray qba = QByteArray::fromBase64(reply_operation->readAll());
    reply_operation->deleteLater();
    reply_operation = NULL;
//    qDebug()<<qba;
    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvOperationInfo]"<<cJSON_Print(json);
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    QString msg = QString::fromUtf8(cJSON_GetObjectItem(json, "msg")->valuestring);
    if(!json_rst)
    {
        qDebug()<<"[recvOperationInfo]"<<msg;
        return;
    }

    cJSON* data = cJSON_GetObjectItem(json, "data");
    int szData = cJSON_GetArraySize(data);

    SqlManager::begin();
    QList<QVariantMap> optList;

    for(int i=0; i<szData; i++)
    {
        cJSON* dataItem = cJSON_GetArrayItem(data, i);//data[i]
        QVariantMap optInfo;
        optInfo.insert("ssc_surgery_bill_id", QString::number(GET_JSON_INT(dataItem, "sscSurgeryBillId")));
        optInfo.insert("surgery_bill_no", GET_JSON_QSTRING(dataItem, "surgeryBillNo"));
        optInfo.insert("surgery_bill_name", GET_JSON_QSTRING(dataItem, "surgeryBillName"));
        optInfo.insert("apply_surgery_date", GET_JSON_QSTRING(dataItem, "applySurgeryDate"));
        optInfo.insert("surgery_order_no", GET_JSON_QSTRING(dataItem, "surgeryOrderNo"));
        optInfo.insert("operating_table", GET_JSON_QSTRING(dataItem, "operatingTable"));
        optInfo.insert("exec_surgery_date", GET_JSON_QSTRING(dataItem, "execSurgeryDate"));
        optInfo.insert("apply_depot_name", GET_JSON_QSTRING(dataItem, "applyDepotName"));
        optInfo.insert("apply_doctor_name", GET_JSON_QSTRING(dataItem, "applyDoctorName"));
        optInfo.insert("exec_depot_name", GET_JSON_QSTRING(dataItem, "execDepotName"));
        optInfo.insert("exec_doctor_name", GET_JSON_QSTRING(dataItem, "execDoctorName"));
        optInfo.insert("patient_name", GET_JSON_QSTRING(dataItem, "patientName"));
        optInfo.insert("patient_gender", GET_JSON_QSTRING(dataItem, "patientGender"));
        optInfo.insert("patient_age", GET_JSON_QSTRING(dataItem, "patientAge"));
        optInfo.insert("patient_no", GET_JSON_QSTRING(dataItem, "patientNo"));
        optList<<optInfo;
//        qDebug()<<GET_JSON_QSTRING(dataItem, "sscSurgeryBillId")<<GET_JSON_QSTRING(dataItem, "surgeryBillNo");
    }
    //更新到数据库
    SqlManager::replace("OperationInfo", optList);
    SqlManager::commit();
    emit operationInfoUpdate();
}

void CabinetServer::recvTempDevReport()
{
    QByteArray qba = QByteArray::fromBase64(reply_aio_temp->readAll());
    qDebug()<<"[recvTempDevReport]"<<qba;
    reply_aio_temp->deleteLater();
    reply_aio_temp = NULL;
}

void CabinetServer::processStandardOutput()
{
    QByteArray qba = tarProcess.readAllStandardOutput();
    qDebug()<<"[update process]:"<<qba;
}

void CabinetServer::updatePacFinish()
{
    qDebug()<<"[updatePacFinish]"<<versionInfo->pacFile;
    reply_download->deleteLater();
    reply_download = NULL;
    pacUpdate->close();
    if(versionInfo->pacIsLegal(pacUpdate))
    {
        if(needConfirmUpdate)
        {
            emit updateCheckRst(true, versionInfo->serverVersion);
            return;
        }
        qDebug("[updatePacFinish]:package is legal,update start.");
        QString cmd = QString("tar -jxvf /home/update/%1 -C /home/update/").arg(versionInfo->pacFile);
        qDebug()<<cmd;
        tarProcess.start(cmd);
    }
    else
    {
        emit updateCheckRst(false, "非法更新包");
        qDebug("[updatePacFinish]:package is not legal,update stop.");
    }
}

void CabinetServer::offlineLogin(QString cardId)
{
    localCacheAccess();
//    if(config->checkManagers(logId))//检查是否为管理员
//    {
//        cur_manager->name = "管理员";
//        cur_manager->power = 0;
//        cur_manager->cardId = logId;
//        cur_user = cur_manager;

//        config->wakeUp(TIMEOUT_FETCH);
//        emit loginRst(cur_user);
//    }

    //检查本地用户缓存
    cur_user = config->checkUserLocal(cardId);

    if(cur_user == NULL)
    {
        qDebug()<<"离线登录失败";
        return;
    }
    qDebug()<<"离线登录成功:"<<cur_user->name<<cur_user->cardId;
    config->wakeUp(TIMEOUT_FETCH);
    emit loginRst(cur_user);

}

int CabinetServer::watchdogTimeout()
{
    int ret = 0;
//    qDebug()<<"[watchdog]"<<"write";
    if (fWatchdog != -1)
    {
        ret = write(fWatchdog, "a", 1);
    }
    return ret;
}

void CabinetServer::tarFinished(int code)
{
    qDebug()<<"update package tar finished:"<<code;
    if(code == 0)
    {
        qDebug()<<"restart..";
        QProcess::startDetached("/sbin/reboot");
    }
}

QByteArray CabinetServer::apiComplete(cJSON *json)
{
    cJSON* jTime = cJSON_GetObjectItem(json, "timeStamp");
    if(jTime == NULL)
        return QByteArray();

    return sqlManager->apiComplete(jTime->valuedouble);
}

void CabinetServer::sysTimeout()
{
    if(!timeIsChecked)
    {
//        sysClock.stop();
//        disconnect(&sysClock, SIGNAL(timeout()), this, SLOT(sysTimeout()));
        checkTime();
    }
//    if(needReqCar&& config->state == STATE_NO);//不再轮询送货单
//        requireListState();

    if(config->sleepFlagTimeout())
    {
        qDebug("[lock]");
        emit sysLock();
        localCacheAccess();
    }
}

void CabinetServer::updateAddress()
{
    qDebug("CabinetServer::updateAddress");
    ApiAddress = config->getServerAddress();
}

