#include "qnetinterface.h"
#include <QProcess>

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

void QNetInterface::setIp(QString _ip)
{
    QProcess p;
    QString cmd = QString("ifconfig %1 %2").arg(devName).arg(_ip);
    qDebug()<<"[setIp]"<<cmd;
    p.start(cmd);
    p.waitForFinished();
}

void QNetInterface::setNetmask(QString _netmask)
{
    QProcess p;
    QString cmd = QString("ifconfig %1 netmask %2").arg(devName).arg(_netmask);
    qDebug()<<"[setNetmask]"<<cmd;
    p.start(cmd);
    p.waitForFinished();
}

void QNetInterface::setGateway(QString _gateway)
{
    QProcess p;
    QString cmd = QString("route add default gw %1").arg(_gateway);
    qDebug()<<"[setGateway]"<<cmd;
    p.start(cmd);
    p.waitForFinished();
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
