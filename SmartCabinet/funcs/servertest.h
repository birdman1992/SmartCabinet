#ifndef SERVERTEST_H
#define SERVERTEST_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDateTime>
#include <QProcess>
#include <QTimer>
#include <QStringList>

class ServerTest : public QObject
{
    Q_OBJECT
public:
    explicit ServerTest(QString apiAddress, QByteArray postData, QObject *parent = 0, QNetworkAccessManager* m = NULL);
    QNetworkAccessManager* getManager();
    void testStart();
    void testFinish();

private:
    QNetworkAccessManager* manager;
    QNetworkReply* reply;
    QTimer* tTimer;
    QTime apiTime;
    QProcess* tProcess;
    QByteArray pData;
    QString pAddress;
    QString serverAddress;
    int testState;

    QString getIpAddress(QString addr);

signals:
    void apiMsg(QString);
    void pingMsg(QString);
    void responseTime(QString ms);

public slots:

private slots:
    void apiTest();
    void recvApiRst();
    void recvPingRst();
    void recvApiError(QNetworkReply::NetworkError);
};

#endif // SERVERTEST_H
