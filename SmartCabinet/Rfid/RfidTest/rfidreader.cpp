#include "rfidreader.h"
#include <QDebug>


RfidReader::RfidReader(QTcpSocket *s, int seq, QObject *parent, DevAction act) : QObject(parent)
{
    flagScan = false;
    flagInit = false;
    flagConnect = false;
    flagWaitBack = false;
    setdevAct(act);
    serverAddr = s->peerAddress().toString();
    serverPort = 0;
    readerSeq = seq;
    config = CabinetConfig::config();
    skt = s;
    connect(skt, SIGNAL(readyRead()), this, SLOT(recvData()));
    connect(skt, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(connectStateChanged(QAbstractSocket::SocketState)));
    heartBeatTimerId = startTimer(10000);
    initPorpertys();
}

RfidReader::RfidReader(QHostAddress server, quint16 port, int seq, QObject *parent, DevAction act) : QObject(parent)
{
    flagScan = false;
    flagInit = false;
    flagConnect = false;
    flagWaitBack = true;
    setdevAct(act);
    config = CabinetConfig::config();
    readerSeq = seq;
    serverAddr = server.toString();
    serverPort = port;
    skt = new QTcpSocket();
    connect(skt, SIGNAL(readyRead()), this, SLOT(recvData()));
    connect(skt, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(connectStateChanged(QAbstractSocket::SocketState)));
    skt->connectToHost(server, port);
    heartBeatTimerId = startTimer(10000);
    speedCalTimerId = startTimer(8000);
    recvCount = 0;
    recvEpcCount = 0;
    initPorpertys();
}

void RfidReader::initPorpertys()
{
    m_confIntens = RfReaderConfig::instance().getConfIntens(serverAddr);
    m_antPowConfig = RfReaderConfig::instance().getAntPower(serverAddr);
    m_gradientThreshold = RfReaderConfig::instance().getGrandThreshold(serverAddr);
    m_devAct = RfReaderConfig::instance().getDeviceAction(serverAddr);
    m_antState = RfReaderConfig::instance().getAntState(serverAddr);
    qDebug()<<"initPorpertys"<<serverAddr<<m_confIntens.toHex()<<m_antPowConfig.toHex()<<m_gradientThreshold<<m_devAct<<m_antState;
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
//        qDebug()<<"[sendCmd] send failed,dev offline:"<<data.toHex();
//        skt->write(data);
    }
}

void RfidReader::setConfIntens(QByteArray confIntens)
{
    m_confIntens = confIntens;
    RfReaderConfig::instance().setConfIntens(serverAddr, m_confIntens);
}

void RfidReader::setAntPowConfig(QByteArray antPowConfig)
{
    m_antPowConfig = antPowConfig;
    RfReaderConfig::instance().setAntPower(serverAddr, m_antPowConfig);

}

void RfidReader::setdevAct(int devAct)
{
//    qDebug()<<"setdevAct:"<<devAct;
    m_devAct = devAct;
    RfReaderConfig::instance().setDeviceAction(serverAddr, DevAction(m_devAct));
}

void RfidReader::setAntState(QBitArray antState)
{
    m_antState = antState;
    RfReaderConfig::instance().setAntState(serverAddr, antState);
}

bool sigIntLessThan(SigInfo* S1, SigInfo* S2)
{
    return S1->signalIntensity>S2->signalIntensity;
}

void RfidReader::timerEvent(QTimerEvent * e)
{
    if(e->timerId() == heartBeatTimerId)
    {
        if(flagScan)
            return;

        if(flagWaitBack)//disconnected
        {
            if(flagConnect)
            {
                qDebug()<<"[RfidReader] heartbeat"<<skt->peerAddress().toString()<<"disconnected";
                setFlagConnect(false);
            }
//            flagConnect = false;
            devReconnect();
        }
        else
        {
            setFlagConnect(true);
//            flagConnect = true;
//            qDebug()<<skt->peerAddress()<<"connected";
        }
        heartBeat();
    }
    else if(e->timerId() == speedCalTimerId)
    {
        return;
//        qDebug()<<"Rfid speed:"<<recvCount/1000.0<<"kb/s"<<"epc count:"<<recvEpcCount;
        QList<SigInfo*> vals = sigMap.values();
        if(vals.isEmpty())
            return;

        qSort(vals.begin(), vals.end(), sigIntLessThan);
        qreal grandThre = (qreal)RfReaderConfig::instance().getGrandThreshold(serverAddr)/100;//获取梯度阈值
        int judgeThre = vals[0]->signalIntensity * (1 - grandThre);//判断阈值

        foreach (SigInfo* sig, vals)
        {
            if(sig->signalIntensity > judgeThre)//信号强度满足阈值
            {
                epcExist(sig);//存在判断
                qDebug()<<sig->epc<<sig->signalIntensity;
            }
        }
    }
}

void RfidReader::heartBeat()
{
    flagWaitBack = true;
    RfidCmd cmd(0x12, QByteArray::fromHex("00000000"));
    sendCmd(cmd.packData(), false);
}

void RfidReader::devReconnect()
{
    if(serverAddr.isNull())
        return;
    if(serverPort == 0)
        return;

    if(skt->state() == QAbstractSocket::UnconnectedState)
    {
//        skt->disconnectFromHost();
        skt->connectToHost(serverAddr, serverPort);
        return;
    }
}

void RfidReader::epcScaned(QString epc)
{
    if(!sigMap.contains(epc))
        sigMap.insert(epc, new SigInfo(epc));

//    qDebug()<<"epc:"<<epc;

    if(sigMap[epc]->sigUpdate(curAnt ,(float)m_confIntens[curAnt-1]))
    {
//        qDebug()<<"epcScaned:"<<epc<<sigMap[epc]->signalIntensity;
        emit reportEpc(epc, getdevAct());
    }
}

void RfidReader::epcExist(SigInfo* info)
{
    if(existList.contains(info->epc))
        return;

    existList.append(info->epc);
    emit reportEpc(info->epc, RF_FETCH);
}

void RfidReader::scanStop()
{
    flagScan = false;
    qDeleteAll(sigMap.begin(), sigMap.end());
    sigMap.clear();
    RfidCmd cmd(0xff);
    sendCmd(cmd.packData());
}

QString RfidReader::readerIp()
{
    return serverAddr;
}

QString RfidReader::readerState()
{
    if(flagConnect)
    {
        return QString("在线");
    }
    else
    {
        return QString("离线");
    }
}

QString RfidReader::readerType()
{
    if(m_devAct == RF_REP)
    {
        return QString("存入模式");
    }
    else if(m_devAct == RF_FETCH)
    {
        return QString("取出模式");
    }
    else if(m_devAct == RF_WARNING)
    {
        return QString("警报模式");
    }
    else if(m_devAct == RF_AUTO)
    {
        return QString("智能模式");
    }
    else if(m_devAct == RF_RANGE_SMALL)
    {
        return QString("聚焦模式");
    }

    return QString("警报模式");
}

bool RfidReader::isConnected()
{
    return flagConnect;
}

void RfidReader::setFlagConnect(bool flag)
{
    flagConnect = flag;
    emit deviceChanged();
}

int RfidReader::gradientThreshold() const
{
    if(!skt)
        return 20;

    return RfReaderConfig::instance().getGrandThreshold(serverAddr);
}

void RfidReader::setGradientThreshold(int gradientThreshold)
{
    if(!skt)
        return;
    m_gradientThreshold = gradientThreshold;
    RfReaderConfig::instance().setGrandThreshold(serverAddr, gradientThreshold);
}

QByteArray RfidReader::confIntens() const
{
    return m_confIntens;
}

QByteArray RfidReader::antPowConfig() const
{
    return m_antPowConfig;
}

DevAction RfidReader::getdevAct() const
{
    return DevAction(m_devAct);
}

QBitArray RfidReader::antState() const
{
    return m_antState;
}

quint32 RfidReader::bit2int(QBitArray b)
{
    quint32 ret = 0;
    for(int i=0; i<32 && i<b.count(); i++)
    {
        ret |= b.at(i)<<i;
    }
    return ret;
}

/*
开始扫描 antState:按位使能天线  scanMode:0 扫描1次 1 一直扫描
*/
void RfidReader::scanStart(int actMode, quint8 scanMode)
{
    if(!(actMode & m_devAct))
        return;

    qDeleteAll(sigMap.begin(), sigMap.end());
    sigMap.clear();

    quint32 antWord;
    antWord = bit2int(antState());

//    if(m_devAct == RF_FETCH)
//        antWord = 0x0001;
//    else
//        antWord = 0x00ff;

    flagScan = true;
    QByteArray cmdParam;
    cmdParam.resize(5);
    char* pos = cmdParam.data();
    antWord = htonl(antWord);
    MEM_ASSIGN(pos, antWord);
    MEM_ASSIGN(pos, scanMode);
    RfidCmd cmd(0x10, cmdParam);
    sendCmd(cmd.packData());
}

void RfidReader::connectStateChanged(QAbstractSocket::SocketState state)
{
    switch(state)
    {
    case QAbstractSocket::ConnectedState:
        if(flagConnect == false)
            qDebug()<<"[RfidReader]"<<skt->peerAddress().toString()<<state;

        setFlagConnect(true);
//        flagConnect = true;
//        initPorpertys();
        scanStop();
        break;

    case QAbstractSocket::UnconnectedState:
        if(flagConnect == true)
            qDebug()<<"[RfidReader]"<<skt->peerAddress().toString()<<state;

        setFlagConnect(false);
//        flagConnect = false;

        break;

    default:
        break;
    }
}

void RfidReader::recvData()
{
    QByteArray qba = skt->readAll();
    flagWaitBack = false;
    recvCount += qba.size();
//    qDebug()<<"[recvData]"<<qba.toHex();
    if(!response.appendData(qba))
        return;

//    qDebug()<<"[recv pack]"<<response.mid<<response.paramData.toHex();
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
//    if(len != epcData.size()-7)
//    {
//        qDebug()<<"[unknow]"<<epcData.toHex()<<len<<epcData.size();
//        return;
//    }

    QByteArray epc = QByteArray(pos, len);
    pos += len;
//    qDebug()<<"[reportEpc]"<<epc.toHex()<<readerSeq<<curAnt;
    recvEpcCount+=1;
    epcScaned(QString(epc.toHex().toUpper()));
}

void RfidReader::parseAnt(QByteArray antData)
{
    curAnt = antData[0];
//    qDebug()<<"parseAnt"<<curAnt;
}
