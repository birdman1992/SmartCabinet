#ifndef CABINETTCP_H
#define CABINETTCP_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QDateTime>
#include <QProcess>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#include "cabinetconfig.h"
#include "Json/cJSON.h"

class CabinetTcp : public QObject
{
    Q_OBJECT
public:
    explicit CabinetTcp(QObject *parent = 0);

public slots:

signals:
    void serverDelay(int ms);//服务延迟,0为超时
    void syncRequire();//柜子同步请求

private:
    QTcpSocket* socket;
    CabinetConfig* config;
    QTimer* beatTimer;
    bool beatWait;//等待心跳返回，不发送下一个心跳包
    QByteArray tcpCache;

private:
    qint64 timeStamp();
    qint64 timeDelay(QString session);//根据session计算延迟
    bool packageIsComplete(QByteArray qba);
    void parHeartBeat(cJSON* json);
    void parCabSync(cJSON *json);
private slots:
    void connectChanged(QAbstractSocket::SocketState state);
    void heartBeat();
    void reconnect();
    void readData();
};

#endif // CABINETTCP_H
