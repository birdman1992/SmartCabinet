#include "rfidreader.h"
#include <QDebug>

RfidReader::RfidReader(QTcpSocket *s, int seq, QObject *parent) : QObject(parent)
{
    flagInit = false;
    flagConnect = false;
    flagWaitBack = false;
    serverAddr = QHostAddress();
    serverPort = 0;
    readerSeq = seq;
    config = CabinetConfig::config();
    skt = s;
    connect(skt, SIGNAL(readyRead()), this, SLOT(recvData()));
    connect(skt, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(connectStateChanged(QAbstractSocket::SocketState)));
    heartBeatTimerId = startTimer(10000);
}

RfidReader::RfidReader(QHostAddress server, quint16 port, int seq, QObject *parent) : QObject(parent)
{

    flagInit = false;
    flagConnect = false;
    flagWaitBack = false;
    config = CabinetConfig::config();
    readerSeq = seq;
    serverAddr = server;
    serverPort = port;
    skt = new QTcpSocket();
    connect(skt, SIGNAL(readyRead()), this, SLOT(recvData()));
    connect(skt, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(connectStateChanged(QAbstractSocket::SocketState)));
    skt->connectToHost(server, port);
    heartBeatTimerId = startTimer(10000);
}

void RfidReader::sendCmd(QByteArray data, bool printFlag)
{
//    qDebug()<<"flagConnect"<<flagConnect;
    if(flagConnect)
    {
        if(printFlag)
            qDebug()<<"[sendCmd]"<<data.toHex();
        skt->write(data);
    }
    else
    {
        qDebug()<<"[sendCmd] send failed,dev offline:"<<data.toHex();
//        skt->write(data);
    }
}

void RfidReader::timerEvent(QTimerEvent * e)
{
    if(e->timerId() == heartBeatTimerId)
    {
        if(flagWaitBack)//disconnected
        {
            qDebug()<<"[RfidReader] heartbeat"<<skt->peerAddress().toString()<<"disconnected";
            flagConnect = false;
            devReconnect();
        }
        else
        {
            flagConnect = true;
//            qDebug()<<skt->peerAddress()<<"connected";
        }
        flagWaitBack = true;
        heartBeat();
    }
}

void RfidReader::heartBeat()
{
    RfidCmd cmd(0x12, QByteArray::fromHex("00000000"));
    sendCmd(cmd.packData(),false);
}

void RfidReader::devReconnect()
{
    if(serverAddr.isNull())
        return;
    if(serverPort == 0)
        return;

    skt->disconnectFromHost();
    skt->connectToHost(serverAddr, serverPort);
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

bool RfidReader::isConnected()
{
    return flagConnect;
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
    qDebug()<<"[RfidReader]"<<skt->peerAddress().toString()<<state;
    switch(state)
    {
    case QAbstractSocket::ConnectedState:
        flagConnect = true;

        scanStop();
        break;

    case QAbstractSocket::UnconnectedState:
        flagConnect = false;

        break;

    default:
        break;
    }
}

void RfidReader::recvData()
{
    QByteArray qba = skt->readAll();
    flagWaitBack = false;
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
    case 0x12://heart beat pac
        flagWaitBack = false;
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
//    qDebug()<<"[reportEpc]"<<epc.toHex()<<readerSeq<<curAnt;
    emit reportEpc(QString(epc.toHex().toUpper()), readerSeq, curAnt);
}

void RfidReader::parseAnt(QByteArray antData)
{
    curAnt = antData[0];
//    qDebug()<<"parseAnt"<<curAnt;
}
