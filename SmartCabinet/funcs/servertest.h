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
    QProcess* tProcess;
    QByteArray pData;
    QString pAddress;
    QString serverAddress;

    QString getIpAddress(QString addr);
    void apiTest();

signals:
    void apiMsg(QString);
    void pingMsg(QString);

public slots:

private slots:
    void recvApiRst();
    void recvPingRst();
};

#endif // SERVERTEST_H
