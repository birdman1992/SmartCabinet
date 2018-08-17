#include "tcpserver.h"
#include <QDebug>
#include <QTime>
#include <QCryptographicHash>
#include <QNetworkRequest>

#include "defines.h"
#include "Json/cJSON.h"
#define TCP_SERVER_PORT 8888

#define API_LOGIN "/api/card"
#define API_CHECK_STORE_LIST "/api/delivery_note"
#define API_STORE_LIST "/api/delivery_note"
#define API_BIND_CASE "/api/grid_goods"
#define API_REBIND_CASE "/api/grid_goods"
#define API_FETCH "/api/package_code"
#define API_REFUND "/api/refund/package_code"
#define API_CHECK_CREAT "/api/stocktaking"
#define API_CHECK "/api/stocktaking/package_codes"
#define API_CHECK_FINISH "/api/stocktaking"
#define API_CHECK_HISTORY "/api/stocktakings"
#define API_CHECK_REQ "/api/stocktaking"
#define API_APPLY_REQ "/api/outstorage_goods"
#define API_APPLY_PUSH "/api/outstorage_goods"

/*
app_id=dc52853b3264e67f7237263927266613
card_no=11111
delivery_note_no=CK20180417000001
nonce_str=ibuaiVcKdpRxkhJA
timestamp=1529391684
app_secret=8f3eedb6e9155bddcd3f5b945b09b61c
    QStringList params;
    params<<"app_id=dc52853b3264e67f7237263927266613";
    params<<"card_no=11111";
    params<<"delivery_note_no=CK20180417000001";
    params<<"nonce_str=ibuaiVcKdpRxkhJA";
    params<<"timestamp=1529391684";
    params<<"grid=[{\"number\": 1,\"status\": 1}]";
    apiJson(params, "8f3eedb6e9155bddcd3f5b945b09b61c");
*/
tcpServer::tcpServer(QObject *parent) : QObject(parent)
{
    tcpState = noState;
    needReg = false;
    syncFLag = false;
    needClone = false;
    reply_login = NULL;
    reply_check_store_list = NULL;
    reply_goods_access = NULL;
    reply_bind_case = NULL;
    reply_refund = NULL;
    reply_check = NULL;
    reply_apply = NULL;
    reply_spell = NULL;
    checkList = NULL;
    cabManager = CabinetManager::manager();
    userManager = UserManager::manager();
    goodsManager = GoodsManager::manager();
    manager = new QNetworkAccessManager(this);
    socket = new QTcpSocket();
    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(connectChanged(QAbstractSocket::SocketState)));
    aesCodec = new QAes(QByteArray("LMUITN25LMUQC436"), 128);

    beatTimer = new QTimer(this);
    connect(beatTimer, SIGNAL(timeout()), this, SLOT(heartBeat()));
}

tcpServer::~tcpServer()
{
    delete aesCodec;
}

bool tcpServer::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    regId = config->getCabinetId();
    if(regId == NULL)
        needReg = true;

    QString strAddr = config->getServerAddress();
    getServerAddr(strAddr.remove("http://"));

    return true;
}

void tcpServer::replyCheck(QNetworkReply *reply)
{
    if(reply != NULL)
        reply->deleteLater();
}

int tcpServer::pushTcpReq(QByteArray qba)
{
    QTimer::singleShot(2000, this, SLOT(tcpReqTimeout()));
    return socket->write(qba);
}
#include <QRegExp>
QString tcpServer::getBarCode(QString str)
{
//    QRegExp rx("[0-9a-zA-Z-]+");
    QRegExp rx("定数包(.+)已经回库");
    int index = rx.indexIn(str);
    qDebug()<<index;
    if(index == -1)
        return QString();

    qDebug()<<rx.capturedTexts();
    return rx.cap(1);
}

bool tcpServer::packageIsComplete(QByteArray qba)
{
    int leftCount = qba.count('{');
    int rightCount = qba.count('}');
    qDebug()<<"packageIsComplete"<<leftCount<<rightCount;
    if(leftCount == rightCount)
        return true;
    else
        return false;
}

void tcpServer::parCabInfo(cJSON *json)
{
    QString col_map = QString(cJSON_GetObjectItem(json, "col_map")->valuestring);
    QString layout = QString(cJSON_GetObjectItem(json, "layout")->valuestring);

    cJSON* hospital = cJSON_GetObjectItem(json, "hospital");
    QString hospital_department_name = QString(cJSON_GetObjectItem(hospital, "hospital_department_name")->valuestring);
    QString hospital_name = QString(cJSON_GetObjectItem(hospital, "hospital_name")->valuestring);

    cJSON* screen_pos = cJSON_GetObjectItem(json, "screen_pos");
    QPoint pos;
    pos.setX(cJSON_GetObjectItem(screen_pos, "col")->valueint);
    pos.setY(cJSON_GetObjectItem(screen_pos, "row")->valueint);

    cabManager->setCabLayout(layout);
    cabManager->setCabMap(col_map);
    cabManager->setDepartName(hospital_department_name);
    cabManager->setHospitalName(hospital_name);
    cabManager->setScrPos(pos);

    if(needClone)
    {
        config->creatCabinetConfig(layout.split('#', QString::SkipEmptyParts), pos);
        config->readCabinetConfig();
        emit cloneResult(true, "克隆成功");
        needClone = false;
    }
    if(syncFLag)
    {
        config->setCabLayout(cabManager->getCabLayout());
        config->setScreenPos(cabManager->getScrPos().x(), cabManager->getScrPos().y());
    }
}

void tcpServer::parUserInfo(cJSON *json)
{
    int userCount = cJSON_GetArraySize(json);
    for(int i=0; i<userCount; i++)
    {
        cJSON* jInfo = cJSON_GetArrayItem(json, i);
        NUserInfo* uInfo = parOneUser(jInfo);
        userManager->setUserInfo(uInfo);
        delete uInfo;
    }
}

void tcpServer::parGoodsInfo(cJSON *json)
{
    int userCount = cJSON_GetArraySize(json);
    if(syncFLag)
    {
        syncFLag = false;
        config->clearGoodsConfig();
        config->readCabinetConfig();
        emit cabPanelChanged();
        emit cabSyncResult(true);
    }

    for(int i=0; i<userCount; i++)
    {
        cJSON* item = cJSON_GetArrayItem(json, i);
        GoodsInfo* info = new GoodsInfo();
        int row = cJSON_GetObjectItem(item, "row")->valueint;
        int col = cJSON_GetObjectItem(item, "col")->valueint;
        info->abbName = QString(cJSON_GetObjectItem(item, "shortening")->valuestring);
        info->name = QString(cJSON_GetObjectItem(item, "goods_name")->valuestring);
        info->num = cJSON_GetObjectItem(item,"store_num")->valueint;
        info->outNum = 0;
        info->id = QString(cJSON_GetObjectItem(item, "goods_id")->valuestring);
        info->goodsType = cJSON_GetObjectItem(item, "package_type")->valueint;
        info->unit = QString(cJSON_GetObjectItem(item, "unit")->valuestring);
        info->Py = config->getPyCh(info->name);//qDebug()<<"[PY]"<<info->Py;
        info->packageId = info->id;

        if(info->goodsType<10)
            info->packageId += "-0"+QString::number(info->goodsType);
        else
            info->packageId += "-"+QString::number(info->goodsType);

        if(info->abbName.isEmpty())
//            info->abbName = getAbbName(info->name);
            info->abbName = info->name;

        qDebug()<<"[newGoods]"<<row<<col<<info->name<<info->abbName<<info->id<<info->packageId<<info->num<<info->unit;
        config->syncGoods(info, row, col);
    }
}

Goods *tcpServer::parGoods(cJSON *json)
{
    Goods* ret = new Goods();
    ret->name = QString(cJSON_GetObjectItem(json, "goods_name")->valuestring);
    ret->abbName = QString(cJSON_GetObjectItem(json, "shortening")->valuestring);
    ret->size = QString(cJSON_GetObjectItem(json, "specifications")->valuestring);
    ret->unit = QString(cJSON_GetObjectItem(json, "unit")->valuestring);
    ret->goodsId = QString(cJSON_GetObjectItem(json, "goods_id")->valuestring);
    ret->packageType = cJSON_GetObjectItem(json, "package_type")->valueint;
    ret->takeCount = cJSON_GetObjectItem(json,"count")->valueint;//packageCount
    ret->totalNum = ret->takeCount;
    if(ret->packageType<10)
        ret->packageBarcode = ret->goodsId + "-0" + QString::number(ret->packageType);
    else
        ret->packageBarcode = ret->goodsId + "-" + QString::number(ret->packageType);

    cJSON* codeList = cJSON_GetObjectItem(json, "package_codes");
    int listSize = cJSON_GetArraySize(codeList);
    cJSON* position = cJSON_GetObjectItem(json, "position");
    ret->pos.setX(cJSON_GetObjectItem(position,"col")->valueint);
    ret->pos.setY(cJSON_GetObjectItem(position,"row")->valueint);

    for(int i=0; i<listSize; i++)
    {
        cJSON* item = cJSON_GetArrayItem(codeList, i);
        ret->codes<<QString(cJSON_GetObjectItem(item, "package_code")->valuestring);
    }
    qDebug()<<ret->name<<ret->codes;

    return ret;
}

CheckTableInfo *tcpServer::parCheckTableInfo(cJSON *json)
{
    if(json == NULL)
        return NULL;

    CheckTableInfo* ret = new CheckTableInfo;

    ret->id = QString::number(cJSON_GetObjectItem(json, "id")->valueint);
    ret->sTime = QDateTime::fromMSecsSinceEpoch(cJSON_GetObjectItem(json, "start_time")->valuedouble*1000).toString("yyyyMMddhhmmss");
    ret->eTime = QDateTime::fromMSecsSinceEpoch(cJSON_GetObjectItem(json, "end_time")->valuedouble*1000).toString("yyyyMMddhhmmss");

    return ret;
}

GoodsCheckInfo* tcpServer::parGoodsCheckInfo(cJSON *json)
{
    if(json == NULL)
        return NULL;

    GoodsCheckInfo* ret = new GoodsCheckInfo;
    ret->id = QString(cJSON_GetObjectItem(json, "goods_id")->valuestring);
    ret->name = QString(cJSON_GetObjectItem(json, "goods_name")->valuestring);
    ret->goodsSize = QString(cJSON_GetObjectItem(json, "specifications")->valuestring);
    ret->num_in = cJSON_GetObjectItem(json, "in_count")->valueint;
    ret->num_out = cJSON_GetObjectItem(json, "take_consume_count")->valueint;
    ret->num_back = cJSON_GetObjectItem(json, "refund_num")->valueint;
    ret->num_cur = cJSON_GetObjectItem(json, "need_consume_count")->valueint;

    return ret;
}

GoodsCheckInfo* tcpServer::parGoodsApplyInfo(cJSON *json)
{
    if(json == NULL)
        return NULL;

    GoodsCheckInfo* ret = new GoodsCheckInfo;
    ret->id = QString(cJSON_GetObjectItem(json, "goods_id")->valuestring);
    ret->name = QString(cJSON_GetObjectItem(json, "goods_name")->valuestring);
    ret->goodsSize = QString(cJSON_GetObjectItem(json, "specifications")->valuestring);
    ret->type = cJSON_GetObjectItem(json, "package_type")->valueint;
    ret->num_cur = cJSON_GetObjectItem(json, "store_num")->valueint;
    ret->num_max = cJSON_GetObjectItem(json, "max_alert_threshold")->valueint;
    ret->num_min = cJSON_GetObjectItem(json, "min_alert_threshold")->valueint;
    ret->producerName = QString(cJSON_GetObjectItem(json, "producer_name")->valuestring);
    if(ret->type<10)
        ret->packageBarCode = ret->id + "-0" + QString::number(ret->type);
    else
        ret->packageBarCode = ret->id + "-" + QString::number(ret->type);

    return ret;
}

void tcpServer::parApp(cJSON *json)
{
    app_id = QString(cJSON_GetObjectItem(json,"app_id")->valuestring);
    app_secret = QString(cJSON_GetObjectItem(json,"app_secret")->valuestring);
    //    qDebug()<<"parApp"<<app_secret;
}

QString tcpServer::getPackageId(QString goodsId, int goodsType)
{
    QString ret;

    if(goodsType<10)
        ret = goodsId + "-0" + QString::number(goodsType);
    else
        ret = goodsId + "-" + QString::number(goodsType);

    return ret;
}

NUserInfo *tcpServer::parOneUser(cJSON *json)
{
    cJSON* jInfo = json;
    NUserInfo* uInfo = new NUserInfo();
    uInfo->card_no = QString(cJSON_GetObjectItem(jInfo, "card_no")->valuestring);
    uInfo->real_name = QString(cJSON_GetObjectItem(jInfo, "real_name")->valuestring);
    uInfo->role_id = cJSON_GetObjectItem(jInfo, "role_id")->valueint;
    uInfo->role_name = QString(cJSON_GetObjectItem(jInfo, "role_name")->valuestring);
    return uInfo;
}
//UserInfo:0 超级管理员, 1 仓库员工, 2 医院管理, 3 护士长, 4 护士
//NUserInfo:1 仓库管理，2 普通管理， 3 护士长， 4 护士
UserInfo *tcpServer::nUserToUser(NUserInfo *nInfo)
{
    UserInfo* ret = new UserInfo;
    ret->cardId = nInfo->card_no;
    ret->name = nInfo->real_name;
    ret->power = (nInfo->role_id==1)?0:(nInfo->role_id);
    return ret;
//    nInfo->role_name;
}

void tcpServer::setServer(QHostAddress _address, quint16 _port)
{
    address = _address;
    port = _port;
    socket->connectToHost(address, port);
}

void tcpServer::getTimeStamp()
{
    waitTimeRst = true;
    tcpState = checkTimeState;
    pushTcpReq(QByteArray("{\"c\": \"timestamp\"}"));
    QTimer::singleShot(10000, this, SLOT(tcpReqTimeout()));
}

void tcpServer::regist()
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    regId = QString::number(qrand()%1000000);

    while(regId.length() < 6)
    {
        regId.insert(0,'0');
    }
    QString aesId = QString(aesCodec->aes_ecb(regId.toLocal8Bit()).toBase64());
    qDebug()<<"[regist]"<<regId;
    tcpState = regState;
    pushTcpReq(jRegist(regId, aesId));
}

void tcpServer::login()
{
    QString aesId = QString(aesCodec->aes_ecb(regId.toLocal8Bit()).toBase64());
    qDebug()<<"[login]"<<regId;
    tcpState = logState;
    pushTcpReq(jLogin(regId, aesId, 1));
}

QByteArray tcpServer::jLogin(QString id, QString aesId, int jType)
{
    return QString("{\
                \"c\": \"verify\",\
                \"data\": {\
                    \"device_id\": \"%1\",\
                    \"aes_device_id\": \"%2\",\
                    \"type\": %3\
                }\
}").arg(id).arg(aesId).arg(jType).toLocal8Bit();
}

QByteArray tcpServer::jUpdate(QString layout, QString col_map, QPoint scrPos)
{
    return QString("{\
                \"c\": \"update\",\
                \"data\": {\
                    \"layout\": \"%1\",\
                    \"col_map\": \"%2\",\
                    \"screen_pos\":{\
                        \"row\":%3,\
                        \"col\":%4\
                    }\
                }\
            }").arg(layout).arg(col_map).arg(scrPos.y()).arg(scrPos.x()).toLocal8Bit();
}

/*
    cabinet_type: 1(cabinet) | 2(rfid cabinet)
*/
QByteArray tcpServer::jRegist(QString id, QString aesId)
{
    QString device_id = id;
    QString aes_device_id = aesId;
    QString layout = config->getCabinetLayout();
    QString col_map = config->getCabinetColMap();
    QPoint screenPos = config->getScreenPos();


    QString retJ = QString("{\
            \"c\": \"verify\",\
            \"data\": {\
                \"device_id\": \"%1\",\
                \"aes_device_id\": \"%2\",\
                \"type\": 2,\
                \"cabinet_type\": 1,\
                \"layout\": \"%3\",\
                \"col_map\": \"%4\",\
                \"screen_pos\": {\
                    \"row\": %5,\
                    \"col\": %6\
                }\
            }\
        }").arg(device_id).arg(aes_device_id).arg(layout).arg(col_map).arg(screenPos.x()).arg(screenPos.y());

//    qDebug()<<"jRegist:"<<retJ;
return retJ.toLocal8Bit();
}

QByteArray tcpServer::apiJson(QStringList params, QString secret)
{
    params<<"sign="+apiSign(params, secret);
    QString jsonStr = "{\"" + params.join("\",\"") + "\"}";
    jsonStr.replace("=","\":\"");
    jsonStr.replace("\"[","[");
    jsonStr.replace("]\"","]");
//    qDebug()<<"[apiJson]"<<jsonStr;
    return jsonStr.toLocal8Bit();
}

//QByteArray tcpServer::jsonStr2Int(QByteArray qba, QString key)
//{
//    int pos_base = qba.indexOf("key");
//    int
//}

QString tcpServer::apiString(QStringList params, QString secret)
{
    params<<"sign="+apiSign(params, secret);
    return params.join("&");
}

QString tcpServer::apiSign(QStringList params, QString secret)
{
    params.sort();
    QString stringA = params.join("&");
    QString stringSignTemp = stringA + "&app_secret=" + secret;
    QByteArray qba = stringSignTemp.toLocal8Bit();
    QString stringSha1 = QCryptographicHash::hash(qba, QCryptographicHash::Sha1).toHex();
//    qDebug()<<"[apiSign]"<<qba<<stringSha1.toUpper();
    return stringSha1.toUpper();
}

void tcpServer::apiPost(QString uil, QNetworkReply **reply, QByteArray data, QObject* receiver,const char *slot)
{
    QString nUrl = config->getServerAddress()+uil;
    qDebug()<<"[apiPost]"<<nUrl<<data;

    QNetworkRequest request;
    request.setUrl(nUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept","application/vnd.spd.cabinet+json");
    replyCheck(*reply);

    *reply = manager->post(request, data);
    connect(*reply, SIGNAL(finished()), receiver, slot);
}

void tcpServer::apiPut(QString uil, QNetworkReply **reply, QByteArray data, QObject* receiver,const char *slot)
{
    QString nUrl = config->getServerAddress()+uil;
    qDebug()<<"[apiPut]"<<nUrl<<data;

    QNetworkRequest request;
    request.setUrl(nUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept","application/vnd.spd.cabinet+json");
    replyCheck(*reply);

    *reply = manager->put(request, data);
    connect(*reply, SIGNAL(finished()), receiver, slot);
}

void tcpServer::apiDelete(QString uil, QNetworkReply **reply, QString data, QObject* receiver,const char *slot)
{
    QString nUrl = config->getServerAddress()+uil+"?"+data;
    qDebug()<<"[apiDelete]"<<nUrl<<data;

    QNetworkRequest request;
    request.setUrl(nUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept","application/vnd.spd.cabinet+json");
    *reply = manager->deleteResource(request);
    connect(*reply, SIGNAL(finished()), receiver, slot);
}

void tcpServer::apiGet(QString uil, QNetworkReply **reply, QString data, QObject *receiver, const char *slot)
{
    QString nUrl = config->getServerAddress()+uil+"?"+data;
    qDebug()<<"[apiGet]"<<nUrl;

    QNetworkRequest request;
    request.setUrl(nUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept","application/vnd.spd.cabinet+json");
    replyCheck(*reply);

    *reply = manager->get(request);
    connect(*reply, SIGNAL(finished()), receiver, slot);
}

QStringList tcpServer::paramsBase()
{
    QStringList ret;
    ret<<QString("%1=%2").arg("app_id").arg(app_id);
    ret<<QString("%1=%2").arg("nonce_str").arg(nonceString());
    ret<<QString("%1=%2").arg("timestamp").arg(timeStamp());
    ret<<QString("%1=%2").arg("card_no").arg(userId);
    return ret;
}

QString s = QString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789");

QString tcpServer::nonceString(int len)
{
    QString ret = QString();
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    for(int i=0; i<len; i++)
    {
        ret.append(s.at(qrand()%s.length()));
    }
    qDebug()<<"[nonceString]"<<ret;;
    return ret;
}

qint64 tcpServer::timeStamp()
{
    return QDateTime::currentMSecsSinceEpoch()/1000;
}

void tcpServer::readData()
{
    QByteArray qba = socket->readAll();
    qDebug()<<"[TCP DATA]:"<<qba;
    tcpCache.append(qba);
    if(!packageIsComplete(tcpCache))
    {
        return;
    }

    cJSON* json = cJSON_Parse(tcpCache.data());
    tcpCache.clear();
    if(json == NULL)
        return;

    QString cr = QString(cJSON_GetObjectItem(json, "cr")->valuestring);
    if(cr == "verify")//登录和注册
    {
        int code = cJSON_GetObjectItem(json, "code")->valueint;
        if(code == 1111)
        {
            if(tcpState == regState)//reg success
            {
                needReg = false;
                config->setCabinetId(regId);
                qDebug()<<"[Cabinet register]:success"<<regId;
                emit idUpdate();
                emit regResult(true);
            }
            tcpState = noState;
            qDebug()<<"[verify success]";
//            getTimeStamp();
            beatTimer->start(5000);
            cJSON* cabJson = cJSON_GetObjectItem(json, "data");
            cJSON* usrJson = cJSON_GetObjectItem(cabJson, "users");
            cJSON* goodsJson = cJSON_GetObjectItem(cabJson, "goods_packages");
            cJSON* appJson = cJSON_GetObjectItem(cabJson, "app");
            parCabInfo(cabJson);
            parUserInfo(usrJson);
            parGoodsInfo(goodsJson);
            parApp(appJson);
        }
        else
        {
            if(tcpState == regState)
            {
                emit regResult(false);
                tcpState = noState;
            }
            qDebug()<<"[verify failed]";
        }
    }
    else if(cr == "update")
    {
        int code = cJSON_GetObjectItem(json, "code")->valueint;
        if(code == 3002)
        {
            emit insertRst(true);
            config->setCabLayout(cabManager->getCabLayout());
            config->setScreenPos(cabManager->getScrPos().x(), cabManager->getScrPos().y());
            syncFLag = true;
            login();
        }
        else
        {
            emit insertRst(false);
        }
    }
    else if(cr == "timestamp")
    {
        tcpState = noState;
        if(cJSON_GetObjectItem(json, "code")->valuedouble == 3002)//get timestamp success
        {
            cJSON* data = cJSON_GetObjectItem(json, "data");
            qint64 timestamp = cJSON_GetObjectItem(data,"timestamp")->valuedouble*1000;
            checkSysTime(QDateTime::fromMSecsSinceEpoch(timestamp));
            qDebug()<<"[timestamp]"<<QDateTime::fromMSecsSinceEpoch(timestamp);
            if((!needReg) || needClone)
            {
                login();
            }
        }
    }

    cJSON_Delete(json);
}

void tcpServer::checkSysTime(QDateTime _time)
{
    QProcess pro;
    QString cmd = QString("date -s \"%1\"").arg(_time.toString("yyyy-MM-dd hh:mm:ss"));
    qDebug()<<"[checkSysTime]"<<cmd;
    waitTimeRst = false;
    pro.start(cmd);
    pro.waitForFinished(1000);
    emit timeUpdate();
//    pro.start("clock -w");
    //    pro.waitForFinished(1000);
}

void tcpServer::recvDateTimeError()
{
    qDebug()<<"[recvDateTimeError]";
    config->clearConfig();
    waitTimeRst = false;
}

void tcpServer::recvUserLogin()
{
    QByteArray qba = reply_login->readAll();
    qDebug()<<"[recvUserLogin]"<<qba;

    int statusCode = reply_login->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply_login->deleteLater();
    reply_login = NULL;

    if(statusCode == 200)
    {
        cJSON* json = cJSON_Parse(qba.data());
        if(json == NULL)
            return;

        cJSON* data = cJSON_GetObjectItem(json, "data");
        if(data == NULL)
            return;

        NUserInfo* nInfo = parOneUser(data);
        UserInfo* uInfo = nUserToUser(nInfo);
        qDebug()<<"pow"<<uInfo->power;
        delete nInfo;
        emit loginRst(uInfo);
        config->addUser(uInfo);
        config->wakeUp(TIMEOUT_BASE);
    }
}

void tcpServer::recvListCheck()
{
    QByteArray qba = reply_check_store_list->readAll();
    qDebug()<<"[recvListCheck]"<<qba;

    int statusCode = reply_check_store_list->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply_check_store_list->deleteLater();
    reply_check_store_list = NULL;

    if(statusCode == 200)
    {
        cJSON* json = cJSON_Parse(qba.data());
        cJSON* data = cJSON_GetObjectItem(json, "data");
        int itemNum = cJSON_GetArraySize(data);
        GoodsList* list = new GoodsList;
        Goods* info;
        for(int i=0; i<itemNum; i++)
        {
            cJSON* item = cJSON_GetArrayItem(data, i);
            info = parGoods(item);
            list->addGoods(info);
        }
        emit listRst(list);
        cJSON_Delete(json);
    }
    else
    {
        emit listRst(new GoodsList);
    }
}

void tcpServer::recvGoodsRefund()
{
    QByteArray qba = reply_refund->readAll();
    qDebug()<<"[recvGoodsRefund]"<<qba;
    int statusCode = reply_refund->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply_refund->deleteLater();
    reply_refund = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    if(json == NULL)
        return;

    if(statusCode == 200)
    {
        cJSON* data = cJSON_GetObjectItem(json, "data");
        QString goodsId = QString(cJSON_GetObjectItem(data, "goods_id")->valuestring);
        int goodsType = cJSON_GetObjectItem(data, "package_type")->valueint;
        QString code = QString(cJSON_GetObjectItem(data, "package_code")->valuestring);
        goodsId = getPackageId(goodsId, goodsType);
        goodsManager->removeCode(code);
        emit goodsNumChanged(goodsId, -1);
    }
    else
    {
        QString msg = QString(cJSON_GetObjectItem(json, "message")->valuestring);
        QString code = getBarCode(msg);
        if(!code.isEmpty())
        {
            qDebug()<<"[remove BarCode]"<<code;
            goodsManager->removeCode(code);
        }
        emit accessFailed(msg);
    }

    cJSON_Delete(json);

    if(!accessList.isEmpty())
    {
        listAccess(accessList, 3);
    }
}

void tcpServer::recvGoodsAccess()
{
    QByteArray qba = reply_goods_access->readAll();
    qDebug()<<"[recvGoodsAccess]"<<qba;
    int statusCode = reply_goods_access->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply_goods_access->deleteLater();
    reply_goods_access = NULL;
    cJSON* json = cJSON_Parse(qba.data());
    if(json == NULL)
        return;

    if(statusCode == 200)
    {
        cJSON* data = cJSON_GetObjectItem(json, "data");
        QString goodsId = QString(cJSON_GetObjectItem(data, "goods_id")->valuestring);
        int goodsType = cJSON_GetObjectItem(data, "package_type")->valueint;
        QString code = QString(cJSON_GetObjectItem(data, "package_code")->valuestring);
        goodsId = getPackageId(goodsId, goodsType);
        goodsManager->removeCode(code);
        emit goodsNumChanged(goodsId, -1);
    }
    else
    {
        emit accessFailed(QString(cJSON_GetObjectItem(json, "message")->valuestring));
    }

    cJSON_Delete(json);
}

void tcpServer::recvGoodsStoreList()
{
    QByteArray qba = reply_goods_access->readAll();
    qDebug()<<"[recvGoodsStoreList]"<<qba;
    int statusCode = reply_goods_access->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply_goods_access->deleteLater();
    reply_goods_access = NULL;

    cJSON* json = cJSON_Parse(qba.data());

    if(statusCode == 200)
    {
//        login();
        QList<QString> list_id;
        QList<int> list_num;
        foreach(CabinetStoreListItem* item, storeList)
        {
            list_id<<item->itemId();
            list_num<<item->itemNum();
        }

        int i=0;
        foreach(QString id, list_id)
        {
            emit goodsNumChanged(id, list_num.at(i));
            i++;
        }
    }
    else
    {
        emit accessFailed(QString(cJSON_GetObjectItem(json, "message")->valuestring));
    }
}

void tcpServer::recvRebindCase()
{
    QByteArray qba = reply_bind_case->readAll();
    qDebug()<<"[recvRebindCase]"<<qba;
    int statusCode = reply_bind_case->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply_bind_case->deleteLater();
    reply_bind_case = NULL;

    if(statusCode == 200)
    {
        config->clearGoodsConfig();
        login();
    }
}

void tcpServer::recvBindCase()
{
    QByteArray qba = reply_bind_case->readAll();
    qDebug()<<"[recvBindCase]"<<qba;
    int statusCode = reply_bind_case->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply_bind_case->deleteLater();
    reply_bind_case = NULL;

    if(statusCode == 200)
    {
        emit bindRst(true);
    }
    else
    {
        emit bindRst(false);
    }
}

void tcpServer::recvCheckCreate()
{
    QByteArray qba = reply_check->readAll();
    qDebug()<<"[recvCheckCreate]"<<qba;
    int statusCode = reply_check->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply_check->deleteLater();
    reply_check = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    if(json == NULL)
        return;

    if(statusCode == 200)
    {
        emit checkCreatRst(true);
    }
    else
    {
        QString msg = QString(cJSON_GetObjectItem(json, "message")->valuestring);
        QRegExp rx("上一次(.+)开始的盘点还未结束");
        int index = rx.indexIn(msg);
        if(index == -1)
            emit checkCreatRst(false, msg);
        else
            emit checkCreatRst(true);
    }
    cJSON_Delete(json);
}

void tcpServer::recvGoodsCheck()
{
    QByteArray qba = reply_check->readAll();
    qDebug()<<"[recvGoodsCheck]"<<qba;
//    int statusCode = reply_check->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply_check->deleteLater();
    reply_check = NULL;
    cJSON* json = cJSON_Parse(qba.data());
    if(json == NULL)
        return;

    QString msg = QString(cJSON_GetObjectItem(json, "message")->valuestring);

    emit goodsCheckRst(msg);
}

void tcpServer::recvCheckFinish()
{
    QByteArray qba = reply_check->readAll();
    qDebug()<<"[recvCheckFinish]"<<qba;
    int statusCode = reply_check->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply_check->deleteLater();
    reply_check = NULL;
    cJSON* json = cJSON_Parse(qba.data());
    if(json == NULL)
        return;

    if(statusCode == 200)
    {
        emit checkFinishRst(true);
    }
    else
    {
        emit checkFinishRst(false, QString(cJSON_GetObjectItem(json, "message")->valuestring));
    }
    cJSON_Delete(json);
}

void tcpServer::recvCheckHistory()
{
    QByteArray qba = reply_check->readAll();
    qDebug()<<"[recvCheckHistory]"<<qba;
    int statusCode = reply_check->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply_check->deleteLater();
    reply_check = NULL;
    cJSON* json = cJSON_Parse(qba.data());
    if(json == NULL)
        return;

    if(statusCode == 200)
    {
        QList<CheckTableInfo*> l;
        cJSON* data = cJSON_GetObjectItem(json, "data");
        int arraySize = cJSON_GetArraySize(json);
        for(int i=0; i<arraySize; i++)
        {
            cJSON* item = cJSON_GetArrayItem(data, i);
            CheckTableInfo* info = parCheckTableInfo(item);
            if(info == NULL)
                return;

            l<<info;
        }
        emit checkTables(l);
    }
}

void tcpServer::recvCheckTable()
{
    QByteArray qba = reply_check->readAll();
    qDebug()<<"[recvCheckTable]"<<qba;
    int statusCode = reply_check->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply_check->deleteLater();
    reply_check = NULL;
    cJSON* json = cJSON_Parse(qba.data());
    if(json == NULL)
        return;

    if(statusCode == 200)
    {
        if(checkList != NULL)
            delete checkList;
        checkList = new CheckList();

        cJSON* data = cJSON_GetObjectItem(json, "data");
        int arraySize = cJSON_GetArraySize(json);
        for(int i=0; i<arraySize; i++)
        {
            cJSON* item = cJSON_GetArrayItem(data, i);
            GoodsCheckInfo* info = parGoodsCheckInfo(item);
            if(info == NULL)
                return;

            checkList->addInfo(info);
        }
        emit curCheckList(checkList);
    }
}

void tcpServer::recvSpellReq()
{
    QByteArray qba = reply_spell->readAll();
    qDebug()<<"[recvSpellReq]"<<qba;
    int statusCode = reply_spell->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply_spell->deleteLater();
    reply_spell = NULL;
    cJSON* json = cJSON_Parse(qba.data());
    if(json == NULL)
        return;

    if(statusCode == 200)
    {
        if(checkList != NULL)
            delete checkList;
        checkList = new CheckList();

        cJSON* data = cJSON_GetObjectItem(json, "data");
        int arraySize = cJSON_GetArraySize(data);
        for(int i=0; i<arraySize; i++)
        {
            cJSON* item = cJSON_GetArrayItem(data, i);
            GoodsCheckInfo* info = parGoodsApplyInfo(item);
            if(info == NULL)
                return;

            checkList->addInfo(info);
        }
        emit curSearchList(checkList);
    }
}

void tcpServer::recvApplyRst()
{
    QByteArray qba = reply_apply->readAll();
    qDebug()<<"[recvApplyRst]"<<qba;
    int statusCode = reply_apply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply_apply->deleteLater();
    reply_apply = NULL;
    cJSON* json = cJSON_Parse(qba.data());
    if(json == NULL)
        return;

    QString msg = QString(cJSON_GetObjectItem(json, "message")->valuestring);

    if(statusCode == 200)
    {
        emit goodsReplyRst(true, msg);
    }
    else
    {
        emit goodsReplyRst(false, msg);
    }
}

void tcpServer::tcpReqTimeout()
{
    if(tcpState == noState)
        return;

    qDebug()<<"[tcpReqTimeout]"<<tcpState;
    switch(tcpState)
    {
    case regState:
        break;
    case logState:
        break;
    case checkTimeState:
//        config->clearConfig();
        break;
    default:
        break;
    }
}

void tcpServer::connectChanged(QAbstractSocket::SocketState state)
{
    qDebug()<<"[connectChanged]:"<<state;
    if(state == QAbstractSocket::ConnectedState)
    {
        getTimeStamp();
        emit netState(true);
    }
    else if(state == QAbstractSocket::UnconnectedState)
    {
        QTimer::singleShot(10000, this, SLOT(reconnect()));
        beatTimer->stop();
        emit netState(false);
    }
}

void tcpServer::heartBeat()
{

    QByteArray qba = QByteArray("{\
                                \"c\": \"heartbeat\",\
                                \"data\": {\
                                    \"packets \": \"i am coming\"\
                                }\
                               }");
    socket->write(qba);
    qDebug()<<"[heartBeat]";
}

void tcpServer::reconnect()
{
    socket->connectToHost(address, port);
}

void tcpServer::cabRegister()
{
    regist();
}

void tcpServer::getServerAddr(QString addr)
{
    qDebug()<<"getServerAddr"<<addr;
    if(addr.indexOf(":") == -1)
        return;

    QStringList listAddr = addr.split(":", QString::SkipEmptyParts);
    if(listAddr.count() != 2)
        return;

    address = QHostAddress(listAddr.at(0));
    port = TCP_SERVER_PORT;
    needSaveAddress = true;
    setServer(address, port);//connect & checktime
}

void tcpServer::userLogin(QString id)
{
    userId = id;
    QStringList params = paramsBase();
    QString param = apiString(params, app_secret);

    qDebug()<<"[userLogin]";
    apiGet(API_LOGIN, &reply_login, param, this, SLOT(recvUserLogin()));
}

void tcpServer::listCheck(QString listCode)
{
    QStringList params = paramsBase();
    params<<"delivery_note_no="+listCode;
    storeListCode = listCode;
    QString param = apiString(params, app_secret);

    qDebug()<<"[listCheck]";
    apiGet(API_CHECK_STORE_LIST, &reply_check_store_list, param, this, SLOT(recvListCheck()));
}

void tcpServer::cabInfoUpload()
{

}

void tcpServer::cabInfoReq()
{

}

void tcpServer::cabCloneReq(QString oldCabinetId)
{
    config->setCabinetId(oldCabinetId);
    regId = oldCabinetId;
    needClone = true;
    login();
}

void tcpServer::cabInfoSync()
{
    syncFLag = true;
    login();
}

void tcpServer::cabColInsert(int pos, QString layout)
{
    cabManager->insertCol(pos, layout);

    QByteArray qba = jUpdate(cabManager->cabLayout, cabManager->cabMap, cabManager->scrPos);
    qDebug()<<"[cabColInsert]"<<qba;
    pushTcpReq(qba);
}

void tcpServer::cabinetBind(int col, int row, QString goodsId)
{
    QStringList params = paramsBase();
    QStringList idInfo = goodsId.split("-", QString::SkipEmptyParts);
    QString goods_id = idInfo.at(0);
    int package_type = idInfo.at(1).toInt();
    params<<QString("goods_id=%1").arg(goods_id);
    params<<QString("package_type=%1").arg(package_type);
    params<<QString("col=%1").arg(col);
    params<<QString("row=%1").arg(row);
    QByteArray param = apiJson(params, app_secret);
    qDebug()<<"[cabinetBind]";
    apiPost(API_BIND_CASE, &reply_bind_case, param, this, SLOT(recvBindCase()));
}

void tcpServer::cabinetRebind(int col, int row, QString goodsId)
{
    QStringList params = paramsBase();
    QStringList idInfo = goodsId.split("-", QString::SkipEmptyParts);
    QString goods_id = idInfo.at(0);
    int package_type = idInfo.at(1).toInt();
    params<<QString("goods_id=%1").arg(goods_id);
    params<<QString("package_type=%1").arg(package_type);
    params<<QString("col=%1").arg(col);
    params<<QString("row=%1").arg(row);
    QByteArray param = apiJson(params, app_secret);
    qDebug()<<"[cabinetRebind]";
    apiPut(API_REBIND_CASE, &reply_bind_case, param, this, SLOT(recvRebindCase()));
}

void tcpServer::goodsAccess(CaseAddress, QString goodsCode, int, int optType)
{
    if(optType == 1)
    {
        goodsFetch(goodsCode);
    }
    else if(optType == 3)
    {
        goodsRefund(goodsCode);
    }
}

void tcpServer::goodsFetch(QString goodsCode)
{
    QStringList params = paramsBase();
    params<<QString("package_code=%1").arg(goodsCode);

    QString param = apiString(params, app_secret);
    qDebug()<<"[goodsFetch]";
    apiDelete(API_FETCH, &reply_goods_access, param, this, SLOT(recvGoodsAccess()));
}

void tcpServer::goodsRefund(QString goodsCode)
{
    QStringList params = paramsBase();
    params<<QString("package_code=%1").arg(goodsCode);

    QByteArray param = apiJson(params, app_secret);
    qDebug()<<"[goodsRefund]";
    apiPost(API_REFUND, &reply_refund, param, this, SLOT(recvGoodsRefund()));
}
#include <QApplication>
void tcpServer::listAccess(QStringList list, int optType)
{
    if(list.isEmpty())
        return;

    accessList = list;

    if(optType == 3)//refund
    {
        QString refundCode = accessList.takeFirst();
        goodsAccess(CaseAddress(), refundCode, 1, optType);
    }
}

void tcpServer::goodsCheckReq()
{
    QString secondCard = config->getSecondUser();
    if(secondCard.isEmpty())
        return;
    QStringList params = paramsBase();
    params<<QString("secondary_card_no=%1").arg(secondCard);
    QByteArray param = apiJson(params, app_secret);
    qDebug()<<"[CheckCreate]";
    apiPost(API_CHECK_CREAT, &reply_check, param, this, SLOT(recvCheckCreate()));
}

void tcpServer::goodsCheckFinish()
{
    QString secondCard = config->getSecondUser();
    if(secondCard.isEmpty())
        return;
    QStringList params = paramsBase();
    params<<QString("secondary_card_no=%1").arg(secondCard);
    QByteArray param = apiJson(params, app_secret);
    qDebug()<<"[CheckFinish]";
    apiPut(API_CHECK_FINISH, &reply_check, param, this, SLOT(recvCheckFinish()));
}

void tcpServer::goodsBack(QString)
{

}

void tcpServer::goodsCheck(QList<CabinetCheckItem *> l, CaseAddress addr)
{
    CabinetCheckItem* item;
    cJSON* json_package_codes = cJSON_CreateArray();
    int i = 0;

    for(i=0; i<l.count(); i++)
    {
        item = l.at(i);

        int j = 0;
        for(j=0; j<item->list_fullId.count(); j++)
        {
            QByteArray qba = item->list_fullId.at(j).toLocal8Bit();
            cJSON_AddItemToArray(json_package_codes, cJSON_CreateString(qba.data()));
        }
    }
    QString package_codes = QString(cJSON_Print(json_package_codes)).remove(' ');
    cJSON_Delete(json_package_codes);
    QStringList params = paramsBase();
    params<<QString("package_codes=%1").arg(package_codes);
    params<<QString("col=%1").arg(addr.cabinetSeqNum);
    params<<QString("row=%1").arg(addr.caseIndex);
    QByteArray param = apiJson(params, app_secret);
    qDebug()<<"[goodsCheck]";
    apiPost(API_CHECK, &reply_check, param, this, SLOT(recvGoodsCheck()));
}

void tcpServer::goodsCheck(QStringList l, CaseAddress)//未使用
{
    cJSON* package_codes = cJSON_CreateArray();
    int i = 0;

    for(i=0; i<l.count(); i++)
    {
        QByteArray qba = l.at(i).toLocal8Bit();
        cJSON_AddItemToArray(package_codes, cJSON_CreateString(qba.data()));
    }
    qDebug()<<QString(cJSON_Print(package_codes)).remove(' ');

    cJSON_Delete(package_codes);
}

void tcpServer::goodsListStore(QList<CabinetStoreListItem *> l)
{
    if(storeListCode.isEmpty())
        return;

    storeList = l;
    QStringList params = paramsBase();
    params<<QString("delivery_note_no=%1").arg(storeListCode);
    QByteArray param = apiJson(params, app_secret);
    qDebug()<<"[goodsListStore]";
    apiPost(API_STORE_LIST, &reply_goods_access, param, this, SLOT(recvGoodsStoreList()));
}

void tcpServer::goodsCarScan()
{

}

void tcpServer::sysTimeout()
{

}

void tcpServer::updateAddress()
{

}

void tcpServer::requireCheckTables(QDate start, QDate finish)
{
    QDateTime d_start = QDateTime(start);
    d_start.setTime(QTime(0,0));
    QDateTime d_finish = QDateTime(finish);
    d_finish.setTime(QTime(23,59));

    QStringList params = paramsBase();
    params<<QString("start_time=%1").arg(d_start.toMSecsSinceEpoch()/1000);
    params<<QString("end_time=%1").arg(d_finish.toMSecsSinceEpoch()/1000);
    QString param = apiString(params, app_secret);

    qDebug()<<"[get CheckTable]";
    apiGet(API_CHECK_HISTORY, &reply_check, param, this, SLOT(recvCheckHistory()));
}

void tcpServer::searchSpell(QString spell)
{
    QStringList params = paramsBase();
    params<<QString("keyword=%1").arg(spell);
    QString param = apiString(params, app_secret);

    qDebug()<<"[searchSpell]";
    apiGet(API_APPLY_REQ, &reply_spell, param, this, SLOT(recvSpellReq()));
}

void tcpServer::replyRequire(QList<GoodsCheckInfo *> replyList)
{
    if(replyList.isEmpty())
        return;

    cJSON* json = cJSON_CreateObject();
    cJSON* data = cJSON_CreateArray();

    foreach(GoodsCheckInfo* info, replyList)
    {
        cJSON* item = cJSON_CreateObject();
        cJSON_AddItemToObject(item, "goods_id", cJSON_CreateString(info->id.toLocal8Bit()));
        cJSON_AddItemToObject(item, "package_type", cJSON_CreateNumber(info->type));
        cJSON_AddItemToObject(item, "package_num", cJSON_CreateNumber(info->num_pack));
        cJSON_AddItemToArray(data, item);
    }
//    cJSON_AddItemToObject(json, "outstorage_goods", data);
    QString strOutstorage = QString(cJSON_Print(data));
    cJSON_Delete(json);
    QStringList params = paramsBase();
    params<<QString("outstorage_goods=%1").arg(strOutstorage);
    QByteArray param = apiJson(params, app_secret);
    qDebug()<<"[replyRequire]";
    apiPost(API_APPLY_PUSH, &reply_apply, param, this, SLOT(recvApplyRst()));
}

void tcpServer::requireCheckTableInfo(QString id)
{
    qDebug()<<"requireCheckTableInfo"<<id;

    QStringList params = paramsBase();
    params<<QString("stocktaking_id=%1").arg(id);
    QString param = apiString(params, app_secret);

    qDebug()<<"[get CheckTableInfo]";
    apiGet(API_CHECK_REQ, &reply_check, param, this, SLOT(recvCheckTable()));
}
