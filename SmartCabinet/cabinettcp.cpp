#include "cabinettcp.h"

#define TCP_PORT 9999

CabinetTcp::CabinetTcp(QObject *parent) : QObject(parent)
{
    beatWait = false;
    beatTimer = new QTimer(this);
    connect(beatTimer, SIGNAL(timeout()), this, SLOT(heartBeat()));

    config = CabinetConfig::config();
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(connectChanged(QAbstractSocket::SocketState)));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
    socket->connectToHost(config->getServerIp(), TCP_PORT);
}

qint64 CabinetTcp::timeStamp()
{
    return QDateTime::currentMSecsSinceEpoch();
}

qint64 CabinetTcp::timeDelay(QString session)
{
    qint64 sessionTime = session.toLongLong();
    return timeStamp() - sessionTime;
}

bool CabinetTcp::packageIsComplete(QByteArray qba)
{
    int leftCount = qba.count('{');
    int rightCount = qba.count('}');
    qDebug()<<"packageIsComplete"<<leftCount<<rightCount;
    if(leftCount == rightCount)
        return true;
    else
        return false;
}

void CabinetTcp::connectChanged(QAbstractSocket::SocketState state)
{
//    qDebug()<<"[connectChanged]:"<<state;
    if(state == QAbstractSocket::ConnectedState)
    {
        qDebug()<<"[connectChanged]:"<<state;
        beatTimer->start(10000);//10s
        heartBeat();
    }
    else if(state == QAbstractSocket::UnconnectedState)
    {
        qDebug()<<"[connectChanged]:"<<state;
        QTimer::singleShot(2000, this, SLOT(reconnect()));
        beatTimer->stop();
        emit serverDelay(0);
    }
}

void CabinetTcp::reconnect()
{
    socket->connectToHost(config->getServerIp(), TCP_PORT);
}

void CabinetTcp::readData()
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

    int type = cJSON_GetObjectItem(json, "type")->valueint;

    switch(type)
    {
    case 1://heartBeat
        parHeartBeat(json);
        break;
    case 2://sync
        parCabSync(json);
        break;
    case 3:
        parLockCtrl(json);
        break;
    default:
        break;
    }
}

void CabinetTcp::heartBeat()
{
    if(beatWait)//心跳超时
    {
        emit serverDelay(0);
    }
    QByteArray qba = QString("{\
                                \"opt\": \"heartBeat\",\
                                \"session\": \"%1\",\
                                \"type\": \"1\",\
                                \"data\": \"\",\
                                \"code\": \"%2\"\
                               }\n").arg(timeStamp()).arg(config->getCabinetId()).toLocal8Bit();
    socket->write(qba);
    beatWait = true;
    qDebug()<<"[heartBeat]";
}

void CabinetTcp::parHeartBeat(cJSON* json)
{
    beatWait = false;
    QString session = QString(cJSON_GetObjectItem(json, "session")->valuestring);
    qint64 delay = timeDelay(session);
    qDebug()<<"[delay]:"<<delay<<"ms";
    emit serverDelay(delay);
}

void CabinetTcp::parCabSync(cJSON* json)
{
    cJSON_ReplaceItemInObject(json, "opt", cJSON_CreateString("back"));
    char* retData = cJSON_Print(json);
    socket->write(retData);
    emit syncRequire();
    qDebug()<<"syncRequire";
}

void CabinetTcp::parLockCtrl(cJSON* json)
{
    cJSON_ReplaceItemInObject(json, "opt", cJSON_CreateString("back"));
    int col = cJSON_GetObjectItem(json, "col")->valueint;
    int row = cJSON_GetObjectItem(json, "row")->valueint;
    emit requireOpenCase(col, row);
    char* retData = cJSON_Print(json);
    socket->write(retData);
    qDebug()<<"OpenCaseRequire";
}
