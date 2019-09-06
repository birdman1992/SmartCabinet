#include "servertest.h"
#include <QDebug>
#include "Json/cJSON.h"
#include <QNetworkReply>

ServerTest::ServerTest(QString apiAddress, QByteArray postData, QObject *parent, QNetworkAccessManager *m) : QObject(parent)
{
    pAddress = apiAddress;
    pData = postData;
    serverAddress = getIpAddress(pAddress);
    reply = NULL;

    if(m == NULL)
    {
        manager = new QNetworkAccessManager(this);
    }
    tProcess = new QProcess(this);
    connect(tProcess, SIGNAL(readyRead()), this, SLOT(recvPingRst()));
    tProcess->start(QString("ping %1").arg(serverAddress));
//    qDebug()<<"cmd"<<QString("ping %1").arg(serverAddress);
}

QNetworkAccessManager *ServerTest::getManager()
{
    return manager;
}

void ServerTest::testStart()
{
    testState = 3600;
    apiTest();
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
    if(reply != NULL)
        reply->deleteLater();

    if(testState <= 0)
    {
        testFinish();
        return;
    }
    testState--;

    QNetworkRequest request;
    QString nUrl = pAddress;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(nUrl);
    reply = manager->post(request, pData.toBase64());
    apiTime.start();

    connect(reply, SIGNAL(finished()), this, SLOT(recvApiRst()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(recvApiError(QNetworkReply::NetworkError)));
}

void ServerTest::recvApiRst()
{
    QByteArray qba = QByteArray::fromBase64(reply->readAll());
    reply->deleteLater();
    reply = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvApiRst]"<<qba<<cJSON_Print(json);
    int response = apiTime.elapsed();
    qDebug()<<"[API response]:"<<response;
    emit responseTime(QString("%1 ms").arg(response));
    QTimer::singleShot(1000, this, SLOT(apiTest()));

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

void ServerTest::recvApiError(QNetworkReply::NetworkError e)
{
    int response = apiTime.elapsed();
    QTimer::singleShot(1000, this, SLOT(apiTest()));
    qDebug()<<"[API response]:"<<response;
    emit responseTime(QString("%1 ms").arg(response));
    qDebug()<<e;
//    emit apiMsg(e.errorString());
}
