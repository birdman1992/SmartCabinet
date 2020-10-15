#include "rfiddevhub.h"
#include "defines.h"

RfidDevHub::RfidDevHub(QObject *parent) : QAbstractTableModel(parent)
{
    sev = new QTcpServer(this);
    sev->listen(QHostAddress::Any, RFID_SKT_PORT);
    list_device.clear();
    connect(sev, SIGNAL(newConnection()), this, SLOT(newConnection()));
    initDevices();
    headerList<<"设备地址"<<"设备状态"<<"设备类型"<<"操作";
}

void RfidDevHub::addDevice(QString addr, quint16 port, QString type)
{
    if(list_device.contains(addr))
    {
        qDebug()<<"[RfidDevHub::addDevice] device repeat.";
        return;
    }

    qDebug()<<"[RFID] addDevice:"<<addr<<port<<type;
    RfidReader::DevType dev_type = (type == "outside")?RfidReader::outside:RfidReader::inside;
    RfidReader* reader = new RfidReader(QHostAddress(addr), port, list_device.count(), NULL, dev_type);
    connect(reader, SIGNAL(reportEpc(QString, bool)), this, SIGNAL(reportEpc(QString, bool)));
    connect(reader, SIGNAL(deviceChanged()), this, SLOT(devStateChanged()));

    RfReaderConfig::instance().createDevice(addr, port, type);
    list_device.insert(addr ,reader);
//    updateDevInfo();
}

void RfidDevHub::delDevice(QString devIp)
{
    RfidReader* reader = list_device.value(devIp);
    list_device.remove(devIp);
    reader->deleteLater();
    reader = NULL;
    RfReaderConfig::instance().delDevice(devIp);
    devStateChanged();
}
QList<RfidReader *> RfidDevHub::deviceList()
{
    return list_device.values();
}

RfidReader *RfidDevHub::device(QString devIp)
{
    return list_device.value(devIp, NULL);
}

int RfidDevHub::rowCount(const QModelIndex&) const
{
    return list_device.count();
}

int RfidDevHub::columnCount(const QModelIndex&) const
{
    return dev_col_count;
}

QVariant RfidDevHub::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    RfidReader* reader = list_device.values()[index.row()];

    if(role == Qt::DisplayRole)
    {
        switch(index.column())
        {
        case dev_addr:
            return reader->readerIp();
            break;
        case dev_state:
            return reader->readerState();
            break;
        case dev_type:
            return reader->readerType();
            break;
        case dev_del:
            return QString("删除");
            break;
        default:
            return QVariant();
            break;
        }
    }

    if(role == Qt::TextAlignmentRole)//居中
        return Qt::AlignCenter;

    return QVariant();
}

QVariant RfidDevHub::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return QVariant(headerList.at(section));

    if(role == Qt::DisplayRole && orientation == Qt::Vertical)
        return QVariant(section+1);

    return QAbstractTableModel::headerData(section, orientation, role);
}

void RfidDevHub::newConnection()
{
    while(sev->hasPendingConnections())
    {
        QTcpSocket* skt = sev->nextPendingConnection();
        RfidReader* reader = new RfidReader(skt, list_device.count(), this);
        list_device.insert(skt->peerAddress().toString() ,reader);
        connect(reader, SIGNAL(reportEpc(QString,int,int)), this, SIGNAL(reportEpc(QString,int,int)));
//        updateDevInfo();
    }
}

void RfidDevHub::devStateChanged()
{
    beginResetModel();
    endResetModel();
}

void RfidDevHub::initDevices()
{
    QStringList devs = RfReaderConfig::instance().getConfigGroups();

    foreach (QString devIp, devs)
    {
        quint16 devPort = RfReaderConfig::instance().getDevicePort(devIp);
        QString devType = RfReaderConfig::instance().getDeviceType(devIp);

        addDevice(devIp,devPort,devType);
    }
}
