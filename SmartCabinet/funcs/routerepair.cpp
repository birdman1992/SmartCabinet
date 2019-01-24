#include "routerepair.h"
#include <QDebug>

RouteRepair::RouteRepair(QObject *parent) : QObject(parent)
{
    process = NULL;
}

void RouteRepair::repairStart(bool state)
{
    if(state)
        return;

    if(process == NULL)
    {
        process = new QProcess();
        connect(process, SIGNAL(readyRead()), this, SLOT(recvCheckRst()));
    }
    process->start("route");
}

QString RouteRepair::getDefaultGateway()
{
    QSettings settings("/home/config/network.ini", QSettings::IniFormat);
    return settings.value("gateway", QString()).toString();
}

void RouteRepair::recvCheckRst()
{
    QByteArray qba = process->readAll();
    QString route = QString(qba);
    QString gateway = getDefaultGateway();
    qDebug()<<"[route]"<<route;
    QRegExp defExp("default");
    QRegExp gwExp(gateway);

    int def1 = defExp.indexIn(route);
    if(def1 < 0)
        return;

    int offset = def1 + defExp.matchedLength();
    qDebug()<<"[offset]"<<offset;
    int def2 = defExp.indexIn(route, offset);
    if(def2 < 0)
    {
        qDebug()<<"[RouteRepair]:gateway correct,exit repair.";
        return;
    }

    int gwIndex = gwExp.indexIn(route);
    qDebug()<<"[RouteRepair]"<<"def1:"<<def1;
    qDebug()<<"[RouteRepair]"<<"def2:"<<def2;
    qDebug()<<"[RouteRepair]"<<"gwIndex:"<<gwIndex;
    if(gwIndex < def2)
    {
        qDebug()<<"[RouteRepair]:gateway correct,exit repair.";
        return;
    }

    qDebug()<<"[RouteRepair]:start repair..";
    QProcess pro;
    pro.start(QString("route del default gw %1").arg(gateway));
    pro.waitForFinished(300000);
    pro.start(QString("route add default gw %1").arg(gateway));
    pro.waitForFinished(300000);
    qDebug()<<"[RouteRepair]:repair finish";
}
