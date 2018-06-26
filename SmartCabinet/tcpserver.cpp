#include "tcpserver.h"
#include <QDebug>
#include <QTime>
#include <QCryptographicHash>

#include "Json/cJSON.h"
#define TCP_SERVER_PORT 8888

#define API_LOGIN "/api/card"

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
    reply_login = NULL;
    cabManager = CabinetManager::manager();
    userManager = UserManager::manager();
    manager = new QNetworkAccessManager(this);
    socket = new QTcpSocket();
    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(connectChanged(QAbstractSocket::SocketState)));
    aesCodec = new QAes(QByteArray("LMUITN25LMUQC436"), 128);

    beatTimer = new QTimer(this);
    connect(beatTimer, SIGNAL(timeout()), this, SLOT(heartBeat()));
//    qDebug()<<"[aesCodec]"<<aesCodec->aes_ecb(QByteArray("1234567")).toBase64();
    QStringList params;
    params<<"delivery_note_no=CK20180417000001";
    params<<"app_id=dc52853b3264e67f7237263927266613";
    params<<"card_no=11111";
    params<<"nonce_str=ibuaiVcKdpRxkhJA";
    params<<"timestamp=1529391684";
    apiJson(params, "8f3eedb6e9155bddcd3f5b945b09b61c");
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
}

void tcpServer::parUserInfo(cJSON *json)
{
    int userCount = cJSON_GetArraySize(json);
    for(int i=0; i<userCount; i++)
    {
        cJSON* jInfo = cJSON_GetArrayItem(json, i);
        NUserInfo* uInfo = new NUserInfo();
        uInfo->card_no = QString(cJSON_GetObjectItem(jInfo, "card_no")->valuestring);
        uInfo->real_name = QString(cJSON_GetObjectItem(jInfo, "real_name")->valuestring);
        uInfo->role_id = QString(cJSON_GetObjectItem(jInfo, "role_id")->valuestring);
        uInfo->role_name = QString(cJSON_GetObjectItem(jInfo, "role_name")->valuestring);
        userManager->setUserInfo(uInfo);
        delete uInfo;
    }
}

void tcpServer::parGoodsInfo(cJSON *json)
{
    int userCount = cJSON_GetArraySize(json);
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

void tcpServer::parApp(cJSON *json)
{
    app_id = QString(cJSON_GetObjectItem(json,"app_id")->valuestring);
    app_secret = QString(cJSON_GetObjectItem(json,"app_secret")->valuestring);
    qDebug()<<"parApp"<<app_secret;
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
    qDebug()<<"[apiSign]"<<qba<<stringSha1.toUpper();
    return stringSha1.toUpper();
}

void tcpServer::apiPost(QString uil, QNetworkReply **reply, QByteArray data, QObject* receiver,const char *slot)
{
    QString nUrl = config->getServerAddress()+uil;
    qDebug()<<"[apiSend]"<<nUrl<<data;

    QNetworkRequest request;
    request.setUrl(nUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("Accept","application/vnd.spd.cabinet+json");
    replyCheck(*reply);

    *reply = manager->post(request, data);
    connect(*reply, SIGNAL(finished()), receiver, slot);
}

#include <QNetworkRequest>
void tcpServer::apiGet(QString uil, QNetworkReply **reply, QString data, QObject *receiver, const char *slot)
{
    QString nUrl = config->getServerAddress()+uil+"?"+data;
    qDebug()<<"[apiSend]"<<nUrl;

    QNetworkRequest request;
    request.setUrl(nUrl);
//    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("Accept","application/vnd.spd.cabinet+json");
    qDebug()<<request.rawHeader("Accep");
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
    cJSON* json = cJSON_Parse(qba.data());
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
    else if(cr == "timestamp")
    {
        tcpState = noState;
        if(cJSON_GetObjectItem(json, "code")->valuedouble == 3002)//get timestamp success
        {
            cJSON* data = cJSON_GetObjectItem(json, "data");
            qint64 timestamp = cJSON_GetObjectItem(data,"timestamp")->valuedouble*1000;
            checkSysTime(QDateTime::fromMSecsSinceEpoch(timestamp));
            qDebug()<<"[timestamp]"<<QDateTime::fromMSecsSinceEpoch(timestamp);
            if(!needReg)
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
    qDebug()<<"[recvUserLogin]"<<reply_login->readAll();
    int statusCode = reply_login->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply_login->deleteLater();
    reply_login = NULL;
    if(statusCode == 200)
    {

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
        config->clearConfig();
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
    }
    else if(state == QAbstractSocket::UnconnectedState)
    {
        QTimer::singleShot(10000, this, SLOT(reconnect()));
        beatTimer->stop();
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
    QString nUrl = config->getServerAddress()+QString(API_LOGIN);
    QStringList params = paramsBase();
    QString param = apiString(params, app_secret);

    qDebug()<<"[userLogin]";
    apiGet(API_LOGIN, &reply_login, param, this, SLOT(recvUserLogin()));
}

void tcpServer::listCheck(QString)
{

}

void tcpServer::cabInfoUpload()
{

}

void tcpServer::cabInfoReq()
{

}

void tcpServer::cabCloneReq(QString oldCabinetId)
{

}

void tcpServer::cabInfoSync()
{

}

void tcpServer::cabColInsert(int pos, int num)
{

}

void tcpServer::cabinetBind(int, int, QString)
{

}

void tcpServer::goodsAccess(CaseAddress, QString, int, int optType)
{

}

void tcpServer::listAccess(QStringList list, int optType)
{

}

void tcpServer::goodsCheckReq()
{

}

void tcpServer::goodsCheckFinish()
{

}

void tcpServer::goodsBack(QString)
{

}

void tcpServer::goodsCheck(QList<CabinetCheckItem *> l, CaseAddress addr)
{

}

void tcpServer::goodsCheck(QStringList l, CaseAddress)
{

}

void tcpServer::goodsListStore(QList<CabinetStoreListItem *> l)
{

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

}

void tcpServer::requireCheckTableInfo(QString id)
{

}
