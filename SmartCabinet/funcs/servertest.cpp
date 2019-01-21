#include "servertest.h"
#include <QDebug>
#include "Json/cJSON.h"

ServerTest::ServerTest(QString apiAddress, QByteArray postData, QObject *parent, QNetworkAccessManager *m) : QObject(parent)
{
    pAddress = apiAddress;
    pData = postData;
    serverAddress = getIpAddress(pAddress);
    reply = NULL;
    if(m == NULL)
    {
        manager = new QNetworkAccessManager(this);
        tProcess = new QProcess(this);
        connect(tProcess, SIGNAL(readyRead()), this, SLOT(recvPingRst()));
        tProcess->start(QString("ping %1").arg(serverAddress));
    }
}

QNetworkAccessManager *ServerTest::getManager()
{
    return manager;
}

void ServerTest::testStart()
{
    if(reply != NULL)
        reply->deleteLater();

    QNetworkRequest request;
    QString nUrl = pAddress;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(nUrl);
    reply = manager->post(request, pData.toBase64());

    connect(reply, SIGNAL(finished()), this, SLOT(recvApiRst()));
}

void ServerTest::testFinish()
{
    if(tProcess != NULL)
        tProcess->kill();
}

QString ServerTest::getIpAddress(QString addr)
{
    QRegExp regExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    int index = regExp.indexIn(addr);
    if(index < 0)
        return QString();
    qDebug()<<"[getIpAddress]"<<regExp.cap(0);
    return regExp.cap(0);
}

void ServerTest::apiTest()
{

}

void ServerTest::recvApiRst()
{
    QByteArray qba = QByteArray::fromBase64(reply->readAll());
    reply->deleteLater();
    reply = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvApiRst]"<<qba<<cJSON_Print(json);

    if(!json)
        return;

    cJSON* rst = cJSON_GetObjectItem(json, "success");

    if(rst->type != cJSON_True)
    {
        qDebug("[check time] failed");
        return;
    }

    rst = cJSON_GetObjectItem(json, "data");

    QString str(rst->valuestring);
    emit apiMsg(str);
    qDebug()<<"apiMsg"<<str;

    cJSON_Delete(json);
}

void ServerTest::recvPingRst()
{
    QByteArray qba = tProcess->readAll();
    qDebug()<<qba;
    qba.remove(qba.size()-1, 1);
    emit pingMsg(QString(qba));
}
