#include "tempdevhub.h"
#include "defines.h"
#include "Json/cJSON.h"

TempDevHub::TempDevHub(QObject *parent) : QObject(parent)
{
    tempMan = TempManager::manager();
    tempMan->configInit();

    server = new QTcpServer(this);
    server->listen(QHostAddress::Any, TEMP_SKT_PORT);
    connect(server, SIGNAL(newConnection()), this, SLOT(newDevice()));
    qDebug()<<"[TempDevHub]:listening.....";

    reportTimerId = startTimer(60000);
    checkTimerId = startTimer(10000);
}

QList<TempCase *> TempDevHub::devList()
{
    return map_devs.values();
}

void TempDevHub::checkTempDev()
{
    foreach (TempCase* dev, map_devs.values())
    {
        dev->checkOverTime();
    }
}

void TempDevHub::tempReport()
{
    if(map_devs.isEmpty())
        return;

    cJSON* reportArray = cJSON_CreateArray();
    foreach (TempCase* dev, map_devs)
    {
        cJSON* devData = cJSON_CreateObject();
        QByteArray devName = dev->devName().toLocal8Bit();
        QByteArray temp = QString::number(dev->getCurTemp(), 'f', 1).toLocal8Bit();
        QByteArray hum = QString::number(dev->getCurHum(), 'f', 1).toLocal8Bit();
        cJSON_AddItemToObject(devData, "deviceName", cJSON_CreateString(devName.data()));
        cJSON_AddItemToObject(devData, "temperature", cJSON_CreateString(temp.data()));
        cJSON_AddItemToObject(devData, "humidity", cJSON_CreateString(hum.data()));
//        cJSON_AddItemToObject(devData, "timeStamp", cJSON_CreateNumber(dev->getCurTemp()));
        cJSON_AddItemToArray(reportArray, devData);
    }
    QByteArray report = QByteArray(cJSON_Print(reportArray));
    cJSON_Delete(reportArray);
    emit tempDevReport(report);
}

void TempDevHub::timerEvent(QTimerEvent *event)
{
//    qDebug()<<"event";
    if(reportTimerId == event->timerId())
    {
        tempReport();
    }
    else if(checkTimerId == event->timerId())
    {
        checkTempDev();
    }
}

void TempDevHub::newDevice()
{
    while(server->hasPendingConnections())
    {
        QTcpSocket* skt = server->nextPendingConnection();
        TempCase* dev = new TempCase;
        qDebug()<<"[TempDevHub]:new connection";
        connect(dev, SIGNAL(caseIdUpdate(TempCase*)), this, SLOT(tempDevIdUpdate(TempCase*)));
        connect(dev, SIGNAL(caseClicked(TempCase*)), this, SIGNAL(tempDevClicked(TempCase*)));
        dev->setSocket(skt);
    }
}

void TempDevHub::tempDevIdUpdate(TempCase *dev)
{
    if(!map_devs.contains(dev->devName()))
        map_devs.insert(dev->devName(), dev);
    else//有同名设备
    {
//        if(dev->devIp() == map_devs[dev->devName()]->devIp())//是同一个设备
//        {
//            dev->deleteLater();
//        }
        TempCase* oldCase = map_devs[dev->devName()];
        if((oldCase != dev) && (oldCase->getCurState() == TempCase::dev_offline))//是不同的设备，且原来的设备离线了
        {
            map_devs.remove(dev->devName());
            oldCase->deleteLater();
            map_devs.insert(dev->devName(), dev);
        }
    }
    emit deviceListUpdate();
}
