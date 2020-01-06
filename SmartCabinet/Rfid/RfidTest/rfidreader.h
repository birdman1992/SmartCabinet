#ifndef RFIDREADER_H
#define RFIDREADER_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include "rfidpackage.h"

class RfidReader : public QObject
{
    Q_OBJECT
public:
    explicit RfidReader(QTcpSocket* s,int col, QObject *parent = 0);
    explicit RfidReader(QHostAddress server, quint16 port, int col, QObject *parent = 0);
    void scanStart(quint32 antState, quint8 scanMode);
    void scanStop();
    QString readerIp();

public slots:
    void sendCmd(QByteArray data);

signals:
    void reportEpc(QString epc, int ant);
private:
    int colPos;
    int curAnt;
    bool flagInit;
    QTcpSocket* skt;
    RfidResponse response;

private slots:
    void connectStateChanged(QAbstractSocket::SocketState state);
    void recvData();
    void dataParse(char mid, QByteArray paramData);
    void parseEpc(QByteArray epcData);
    void parseAnt(QByteArray antData);
};

#endif // RFIDREADER_H
