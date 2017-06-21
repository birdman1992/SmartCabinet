#include "qnetinterface.h"

QNetInterface::QNetInterface(QString name, QObject *parent) : QObject(parent)
{
    interface = getNetworkInterface(name);

    if(!interface.isValid())
        qDebug()<<"[QNetInterface]"<<name<<"open failed";
    getNetworkInfo();
}

QString QNetInterface::ip()
{
    return netEntry.ip().toString();
}

QString QNetInterface::netmask()
{
    return netEntry.netmask().toString();
}

QString QNetInterface::broadcast()
{
    return netEntry.broadcast().toString();
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
