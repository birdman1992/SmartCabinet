#ifndef RFIDREADER_H
#define RFIDREADER_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMap>
#include "rfidpackage.h"
#include "cabinetconfig.h"
#include "manager/rfreaderconfig.h"
#include <QtDebug>
#include <QDateTime>

enum RfidAction{
    RF_REP,//库存
    RF_FETCH,//取出
    RF_WARNING,//警报
};

class SigInfo
{
public:
    SigInfo(QString _epc)
    {
        findFlag = false;
        epc = _epc;
        clearInfo();
    }
    void clearInfo(){
        memset(scanTimes, 0, sizeof(scanTimes));
        signalIntensity = 0;
        clearStamp = QDateTime::currentMSecsSinceEpoch();
    }
    bool sigUpdate(int curAnt ,float sigThe){
        scanTimes[curAnt]++;
        qint64 durTime = QDateTime::currentMSecsSinceEpoch()-clearStamp;
        if(durTime<1000)
            return 0.0;

        float sigInt = qRound((float)scanTimes[curAnt]*5000/(durTime)*100)/100;//5秒钟扫描次数
        findFlag = sigInt>sigThe;
        if(findFlag)
        {
            signalIntensity = sigInt;
            findAnt = curAnt;
        }
//        qDebug()<<"[sigUpdate]"<<scanTimes[curAnt]<<(QDateTime::currentMSecsSinceEpoch()-clearStamp)<<signalIntensity;
        return findFlag;
    }
    bool findFlag;
    QString epc;
    int findAnt;
    qint64 clearStamp;
    quint32 scanTimes[8];//扫描次数
    float signalIntensity;//扫描强度:每秒扫描次数

};

class RfidReader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray confIntens READ confIntens WRITE setConfIntens)//置信强度
    Q_PROPERTY(QByteArray antPowConfig READ antPowConfig WRITE setAntPowConfig)//天线强度
    Q_PROPERTY(int gradientThreshold READ gradientThreshold WRITE setGradientThreshold)//梯度阈值
    Q_PROPERTY(bool outsideDev READ outsideDev WRITE setOutsideDev)//设备类型

public:
    enum DevType{
        inside=1,
        outside=2,
        all=3,
    };

public:
    explicit RfidReader(QTcpSocket* s,int seq, QObject *parent = 0, DevType _type=inside);
    explicit RfidReader(QHostAddress server, quint16 port, int seq, QObject *parent = 0, DevType _type=inside);
    void scanStart(DevType _type,quint8 scanMode);
    void scanStop();
    QString readerIp();//dev addr
    QString readerState();//dev state
    QString readerType();//dev type
    bool isConnected();
    void setFlagConnect(bool flag);
    int gradientThreshold() const;
    void setGradientThreshold(int gradientThreshold);
    QByteArray confIntens() const;
    QByteArray antPowConfig() const;
    bool outsideDev() const;

public slots:
    void sendCmd(QByteArray data, bool printFlag=true);
    void setConfIntens(QByteArray confIntens);
    void setAntPowConfig(QByteArray antPowConfig);
    void setOutsideDev(bool outsideDev);

signals:
    void reportEpc(QString epc, RfidAction rfidAct);
    void reportEpc(QString epc, bool isOutside);
    void stateChanged();//连接状态变化
    void deviceChanged();//设备发生变化

//    void gradientPercentChanged(int gradientPercent);

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
    DevType devType;
    QTcpSocket* skt;
    RfidResponse response;
    CabinetConfig* config;
    QString serverAddr;
    quint16 serverPort;
    quint64 recvCount;
    quint32 recvEpcCount;
    QMap<QString, SigInfo*> sigMap;
//    QByteArray confIntens;//天线置信强度
//    QByteArray antPowConfig;
    QStringList existList;//盘存列表

    void heartBeat();
    void devReconnect();
    void epcScaned(QString epc);
    void epcExist(SigInfo *info);//EPC加入盘存列表

    int m_gradientThreshold;//梯度阈值
    QByteArray m_confIntens;
    QByteArray m_antPowConfig;

    bool m_outsideDev;

    void initPorpertys();
private slots:
    void connectStateChanged(QAbstractSocket::SocketState state);
    void recvData();
    void dataParse(char mid, QByteArray paramData);
    void parseEpc(QByteArray epcData);
    void parseAnt(QByteArray antData);
};

#endif // RFIDREADER_H
