#include "rfiddevhub.h"
#include "defines.h"

RfidDevHub::RfidDevHub(QObject *parent) : QObject(parent)
{
    sev = new QTcpServer(this);
    sev->listen(QHostAddress::Any, RFID_SKT_PORT);
    list_device.clear();
    connect(sev, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void RfidDevHub::addDevice(QHostAddress addr, quint16 port)
{
    RfidReader* reader = new RfidReader(addr, port, list_device.count());
    connect(reader, SIGNAL(reportEpc(QString,int,int)), this, SIGNAL(reportEpc(QString,int,int)));
    updateDevInfo();
    list_device<<reader;
}

QList<RfidReader *> RfidDevHub::deviceList()
{
    return list_device;
}

void RfidDevHub::newConnection()
{
    while(sev->hasPendingConnections())
    {
        QTcpSocket* skt = sev->nextPendingConnection();
        RfidReader* reader = new RfidReader(skt, list_device.count(), this);
        list_device<<reader;
        connect(reader, SIGNAL(reportEpc(QString,int,int)), this, SIGNAL(reportEpc(QString,int,int)));
        updateDevInfo();
    }
}

//更新设备信息
void RfidDevHub::updateDevInfo()
{
    list_dev_info.clear();
    foreach (RfidReader* reader, list_device)
    {

    }
    emit devInfoChanged(list_dev_info);
}
