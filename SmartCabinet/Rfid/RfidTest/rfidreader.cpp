#include "rfidreader.h"
#include <QDebug>

RfidReader::RfidReader(QTcpSocket *s, int col, QObject *parent) : QObject(parent)
{
    flagInit = false;
    colPos = col;
    skt = s;
    connect(skt, SIGNAL(readyRead()), this, SLOT(recvData()));
    connect(skt, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(connectStateChanged(QAbstractSocket::SocketState)));
}

RfidReader::RfidReader(QHostAddress server, quint16 port, int col, QObject *parent) : QObject(parent)
{
    flagInit = false;
    colPos = col;
    skt = new QTcpSocket();
    connect(skt, SIGNAL(readyRead()), this, SLOT(recvData()));
    connect(skt, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(connectStateChanged(QAbstractSocket::SocketState)));
    skt->connectToHost(server, port);
}

void RfidReader::sendCmd(QByteArray data)
{
    qDebug()<<"[sendCmd]"<<data.toHex();
    skt->write(data);
}

void RfidReader::scanStop()
{
    RfidCmd cmd(0xff);
    sendCmd(cmd.packData());
}

QString RfidReader::readerIp()
{
    return skt->peerAddress().toString();
}

/*
开始扫描 antState:按位使能天线  scanMode:0 扫描1次 1 一直扫描
*/
void RfidReader::scanStart(quint32 antState, quint8 scanMode)
{
    QByteArray cmdParam;
    cmdParam.resize(5);
    char* pos = cmdParam.data();
    antState = htonl(antState);
    MEM_ASSIGN(pos, antState);
    MEM_ASSIGN(pos, scanMode);
    RfidCmd cmd(0x10, cmdParam);
    sendCmd(cmd.packData());
}

void RfidReader::connectStateChanged(QAbstractSocket::SocketState state)
{
    qDebug()<<"[RfidReader]"<<state;
}

void RfidReader::recvData()
{
    QByteArray qba = skt->readAll();
//    qDebug()<<"[recvData]"<<qba.toHex();
    if(!response.appendData(qba))
        return;

//    qDebug()<<response.mid<<response.paramData.toHex();
    dataParse(response.mid, response.paramData);
    while(response.appendData())
    {
        dataParse(response.mid, response.paramData);
    }
}

void RfidReader::dataParse(char mid, QByteArray paramData)
{
    switch(mid)
    {
    case 0x00://EPC info
        parseEpc(paramData);
        break;
    case 0x02://current ant
        parseAnt(paramData);
        break;
    case 0xff:
        flagInit = true;
        break;
    default:
        break;
    }
}

void RfidReader::parseEpc(QByteArray epcData)
{
    if(!flagInit)
        return;

    quint16 len;
    char* pos = epcData.data();
    //读取变长EPC
    MEM_FETCH(len, pos);
    len = ntohs(len);
    QByteArray epc = QByteArray(pos, len);
    pos += len;
//    qDebug()<<"[reportEpc]"<<epc.toHex();
    emit reportEpc(QString(epc.toHex().toUpper()), curAnt);
}

void RfidReader::parseAnt(QByteArray antData)
{
    curAnt = antData[0];
//    qDebug()<<"parseAnt"<<curAnt;
}