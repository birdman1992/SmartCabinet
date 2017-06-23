#include "qnetinterface.h"

QNetInterface::QNetInterface(QString name, QObject *parent) : QObject(parent)
{
    interface = getNetworkInterface(name);

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

QString QNetInterface::broadcast()
{
    if(interface.isValid())
    {
        getNetworkInfo();
        return netEntry.broadcast().toString();
    }
    else
        return QString();
}

bool QNetInterface::isValid()
{
    return interface.isValid();
}

QNetworkInterface QNetInterface::getNetworkInterface(QString name)
{
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();
    int i = 0;
    foreach(QNetworkInterface ni,nets)
    {
        i++;
//        qDebug()<<i<<ni.name()<<ni.hardwareAddress()<<ni.humanReadableName();
        if(ni.name() == name)
            return ni;
    }

    return QNetworkInterface();
}

void QNetInterface::getNetworkInfo()
{
    if(!interface.isValid())
        return;

    QList<QNetworkAddressEntry> entryList = interface.addressEntries();
    netEntry = entryList.at(0);

    foreach(QNetworkAddressEntry en, entryList)
    {
        if(en.ip().toString().length()<16)
        {
            netEntry = en;
            return;
        }
    }
}
