#ifndef RFIDREADER_H
#define RFIDREADER_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include "rfidpackage.h"
#include "cabinetconfig.h"

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
protected:
    void timerEvent(QTimerEvent*);

private:
    int readerSeq;
    int curAnt;
    int heartBeatTimerId;
    bool flagInit;
    bool flagConnect;
    bool flagWaitBack;
    QTcpSocket* skt;
    RfidResponse response;
    CabinetConfig* config;

    void heartBeat();

private slots:
    void connectStateChanged(QAbstractSocket::SocketState state);
    void recvData();
    void dataParse(char mid, QByteArray paramData);
    void parseEpc(QByteArray epcData);
    void parseAnt(QByteArray antData);
};

#endif // RFIDREADER_H
