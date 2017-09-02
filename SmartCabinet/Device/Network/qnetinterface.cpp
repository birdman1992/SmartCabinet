#include "qnetinterface.h"
#include <QProcess>
#include <QSettings>

QNetInterface::QNetInterface(QString name, QObject *parent) : QObject(parent)
{
    interface = getNetworkInterface(name);
    devName = name;

    if(!interface.isValid())
        qDebug()<<"[QNetInterface]"<<name<<"open failed";
    else
        getNetworkInfo();
}

QString QNetInterface::ip()
{
    if(interface.isValid())
    {
        getNetworkInfo();
        return netEntry.ip().toString();
    }
    else
        return QString();
}

QString QNetInterface::netmask()
{
    if(interface.isValid())
    {
        getNetworkInfo();
        return netEntry.netmask().toString();
    }
    else
        return QString();
}

QString QNetInterface::gateway()
{
    if(interface.isValid())
    {
        QSettings settings("/home/config/network.ini", QSettings::IniFormat);
        QString netGateway = settings.value("gateway", QString()).toString();
        if(!netGateway.isEmpty())
            return netGateway;

        getNetworkInfo();
        QString _ip = netEntry.ip().toString();
        QStringList l = _ip.split('.');
        l.removeLast();
        l.append("1");
        return l.join(".");
    }
    else
        return QString();
}

bool QNetInterface::numPointCheck(QString str)
{
    QStringList l = str.split(".");
    if(l.count() != 4)
        return false;

    foreach(QString s, l)
    {
        if(s.toInt()<0 || s.toInt()>255)
            return false;
    }

    return true;
}

bool QNetInterface::setIp(QString _ip)
{
    if(!numPointCheck(_ip))
        return false;

    QProcess p;
    QString cmd = QString("ifconfig %1 %2").arg(devName).arg(_ip);
    qDebug()<<"[setIp]"<<cmd;
    p.start(cmd);
    p.waitForFinished();

    return true;
}

bool QNetInterface::setNetmask(QString _netmask)
{
    if(!numPointCheck(_netmask))
        return false;

    QProcess p;
    QString cmd = QString("ifconfig %1 netmask %2").arg(devName).arg(_netmask);
    qDebug()<<"[setNetmask]"<<cmd;
    p.start(cmd);
    p.waitForFinished();

    return true;
}

bool QNetInterface::setGateway(QString _gateway)
{
    if(!numPointCheck(_gateway))
        return false;

    devGateway = _gateway;
    QProcess p;
    QString cmd = QString("route add default gw %1").arg(_gateway);
    qDebug()<<"[setGateway]"<<cmd;
    p.start(cmd);
    p.waitForFinished();

    return true;
}

void QNetInterface::saveNetwork()
{
    QSettings settings("/home/config/network.ini", QSettings::IniFormat);
    settings.setValue("ip", QVariant(ip()));
    settings.setValue("gateway", QVariant(devGateway));
    settings.setValue("netmask", QVariant(netmask()));
    settings.sync();
}

void QNetInterface::initNetwork()
{
    QSettings settings("/home/config/network.ini", QSettings::IniFormat);
    QString netIp = settings.value("ip", QString()).toString();
    QString netNetmask = settings.value("netmask", QString()).toString();
    QString netGateway = settings.value("gateway", QString()).toString();

    setIp(netIp);
    setNetmask(netNetmask);
    setGateway(netGateway);
}

bool QNetInterface::isValid()
{
    return interface.isValid();
}

QNetworkInterface QNetInterface::getNetworkInterface(QString name)
{
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();
//    int i = 0;
    foreach(QNetworkInterface ni,nets)
    {
//        i++;
//        qDebug()<<i<<ni.name()<<ni.hardwareAddress()<<ni.humanReadableName();
        if(ni.name() == name)
            return ni;
    }
//qDebug("getNetworkInterface");
    return QNetworkInterface();
}

void QNetInterface::getNetworkInfo()
{
    interface = getNetworkInterface(devName);
    if(!interface.isValid())
        return;

    QList<QNetworkAddressEntry> entryList = interface.addressEntries();
    if(entryList.isEmpty())
        return;
    netEntry = entryList.at(0);

    foreach(QNetworkAddressEntry en, entryList)
    {
        if(en.ip().toString().length()<16)
        {
            netEntry = en;
//            qDebug()<<netEntry.ip()<<netEntry.broadcast()<<netEntry.netmask();
            return;
        }
    }
}
