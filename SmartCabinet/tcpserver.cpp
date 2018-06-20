#include "tcpserver.h"
#include <QDebug>
#include <QTime>

#include "Json/cJSON.h"
#define TCP_SERVER_PORT 8888

tcpServer::tcpServer(QObject *parent) : QObject(parent)
{
    tcpState = noState;
    needReg = false;
    socket = new QTcpSocket();
    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(connectChanged(QAbstractSocket::SocketState)));
    aesCodec = new QAes(QByteArray("LMUITN25LMUQC436"), 128);

    beatTimer = new QTimer(this);
    connect(beatTimer, SIGNAL(timeout()), this, SLOT(heartBeat()));
//    qDebug()<<"[aesCodec]"<<aesCodec->aes_ecb(QByteArray("1234567")).toBase64();
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

int tcpServer::pushTcpReq(QByteArray qba)
{
    QTimer::singleShot(2000, this, SLOT(tcpReqTimeout()));
    return socket->write(qba);
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

QString tcpServer::apiSign(QStringList params, QString secret)
{

}

QString s = QString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789");

QString tcpServer::nonceString()
{

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

void tcpServer::userLogin(QString)
{

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
