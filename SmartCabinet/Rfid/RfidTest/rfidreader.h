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

public slots:
    void sendCmd(QByteArray data);

signals:
    void reportEpc(QString epc, int seq, int ant);
private:
    int readerSeq;
    int curAnt;
    bool flagInit;
    bool flagConnect;
    QTcpSocket* skt;
    RfidResponse response;
    CabinetConfig* config;

private slots:
    void connectStateChanged(QAbstractSocket::SocketState state);
    void recvData();
    void dataParse(char mid, QByteArray paramData);
    void parseEpc(QByteArray epcData);
    void parseAnt(QByteArray antData);
};

#endif // RFIDREADER_H
