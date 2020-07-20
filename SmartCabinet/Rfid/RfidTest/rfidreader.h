#ifndef RFIDREADER_H
#define RFIDREADER_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMap>
#include "rfidpackage.h"
#include "cabinetconfig.h"
#include <QtDebug>
#include <QDateTime>

class SigInfo
{
public:
    SigInfo()
    {
        clearInfo();
    }
    void clearInfo(){
        scanTimes = 0;
        signalIntensity = 0;
        clearStamp = QDateTime::currentMSecsSinceEpoch();
    }
    float sigUpdate(){
        scanTimes++;
        signalIntensity = qRound((float)scanTimes*1000/(QDateTime::currentMSecsSinceEpoch()-clearStamp)*100)/100;
        //qDebug()<<"[sigUpdate]"<<scanTimes<<(QDateTime::currentMSecsSinceEpoch()-clearStamp)<<signalIntensity;
        return signalIntensity;
    }
    qint64 clearStamp;
    quint32 scanTimes;//扫描次数
    float signalIntensity;//扫描强度:每秒扫描次数
};

class RfidReader : public QObject
{
    Q_OBJECT
public:
    explicit RfidReader(QTcpSocket* s,int seq, QObject *parent = 0);
    explicit RfidReader(QHostAddress server, quint16 port, int seq, QObject *parent = 0);
    void scanStart(quint32 antState, quint8 scanMode);
    void scanStop();
    QString readerIp();
    bool isConnected();

public slots:
    void sendCmd(QByteArray data, bool printFlag=true);

signals:
    void reportEpc(QString epc, int seq, int ant);
    void stateChanged();//连接状态变化

protected:
    void timerEvent(QTimerEvent*);

private:
    int readerSeq;
    int curAnt;
    int heartBeatTimerId;
    int speedCalTimerId;
    bool flagInit;
    bool flagConnect;
    bool flagWaitBack;
    bool flagScan;
    QTcpSocket* skt;
    RfidResponse response;
    CabinetConfig* config;
    QHostAddress serverAddr;
    quint16 serverPort;
    quint64 recvCount;
    quint32 recvEpcCount;
    QMap<QString, SigInfo*> sigMap;
    QByteArray confIntens;//天线置信强度
    QByteArray antPowConfig;

    void heartBeat();
    void devReconnect();
    void epcScaned(QString epc);

private slots:
    void connectStateChanged(QAbstractSocket::SocketState state);
    void recvData();
    void dataParse(char mid, QByteArray paramData);
    void parseEpc(QByteArray epcData);
    void parseAnt(QByteArray antData);
};

#endif // RFIDREADER_H
