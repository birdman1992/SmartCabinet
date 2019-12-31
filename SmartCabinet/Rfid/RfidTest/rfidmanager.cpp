#include "rfidmanager.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QDateTime>
#include <QTimer>
#include <QSettings>

RfidManager::RfidManager(QObject *parent) : QObject(parent)
{
    table_in = NULL;
    table_out = NULL;
    table_back = NULL;
    table_con = NULL;

    QHostAddress serverAddr = QHostAddress("192.168.0.7");
    testReader = new RfidReader(serverAddr, 1111, 3, this);
    connect(testReader, SIGNAL(reportEpc(QString,int)), this, SLOT(testUpdateEpc(QString)));
    initEpc();
}

void RfidManager::initEpc()
{
    //[epc_code|goods_code|time_stamp|state]
    QSqlQuery query = SqlManager::getRfidTable();
    while(query.next())
    {
        EpcInfo* info = new EpcInfo(query.value(0).toString(), query.value(1).toString());
        info->lastStamp = query.value(2).toUInt();
        info->state = (EpcState)query.value(3).toInt();
        map_rfid.insert(info->epcId, info);
        emit updateEpcInfo(info);
    }
}

void RfidManager::startScan()
{
//    testReader->scanStop();
    testReader->scanStart(0x0f, 0);
}

void RfidManager::stopScan()
{
    testReader->scanStop();
}

void RfidManager::epcCheck(quint32 cutOffStamp, int row, int col)
{
    QSqlQuery query = SqlManager::checkRfid(cutOffStamp, row, col);

}

void RfidManager::epcSync()
{
    SqlManager::updateRfidsStart();
    foreach (EpcInfo* info, map_rfid)
    {
        SqlManager::updateRfidsSingle(info->epcId, info->lastStamp, info->lastOpt, info->state, info->rowPos, info->colPos);
    }
    SqlManager::updateRfidsFinish();
}

void RfidManager::initTableViews(QTableWidget *in, QTableWidget *out, QTableWidget *back, QTableWidget *con)
{
    table_in = in;
    table_out = out;
    table_back = back;
    table_con = con;
}

void RfidManager::newRfidMark(QString epc, QString goodsCode)
{
    if(map_rfid.contains(epc))
        return;

    EpcInfo* info = new EpcInfo(epc, goodsCode);
    map_rfid.insert(epc, info);
}

void RfidManager::updateEpc(QString epc, int colPos)
{
    EpcInfo* info = map_rfid.value(epc, NULL);
    if(info == NULL)
        return;

    switch(info->state)
    {
    case epc_no://入库标签
        info->lastStamp = QDateTime::currentMSecsSinceEpoch();
        info->state = epc_in;
        info->colPos = colPos;
        list_new<<epc;
        emit updateEpcInfo(info);
        break;
    case epc_out://还回标签
        info->lastStamp = QDateTime::currentMSecsSinceEpoch();
        info->state = epc_in;
        info->colPos = colPos;
        list_back<<epc;
        emit updateEpcInfo(info);
        break;
    case epc_in://刷新时间戳
        info->lastStamp = QDateTime::currentMSecsSinceEpoch();
        break;
    case epc_consume://已消耗标签
        return;
    default:
        break;
    }
}

void RfidManager::testUpdateEpc(QString epc, int colPos)
{
    if(!map_rfid.contains(epc))
    {
        newRfidMark(epc, "TEST-0001");
    }
    updateEpc(epc, colPos);
}

EpcInfo::EpcInfo(QString id, QString _goodsCode)
{
    epcId = id;
    goodsCode = _goodsCode;
    lastStamp = 0;
    state = epc_no;
}
