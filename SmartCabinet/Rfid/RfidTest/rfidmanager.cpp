#include "rfidmanager.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QDateTime>
#include <QTimer>
#include <QSettings>

#include "manager/signalmanager.h"

RfidManager::RfidManager(QObject *parent) : QObject(parent)
{
    table_in = NULL;
    table_out = NULL;
    table_back = NULL;
    table_con = NULL;
    flagCorct = false;
    flagInit = false;
    flagScan = false;
    clsTime = 2000;
    doorState = 0;
    insideAnt = 0x00fe;
    outsideAnt = 0x0001;

    config = CabinetConfig::config();

    SignalManager* sigManager = SignalManager::manager();
    connect(this, SIGNAL(epcAccess(QStringList, int)), sigManager, SIGNAL(epcAccess(QStringList, int)));
    connect(sigManager, SIGNAL(doorState(int, bool)), this, SLOT(doorStateChanged(int, bool)));

    QHostAddress serverAddr = QHostAddress("192.168.0.8");
    testReader = new RfidReader(serverAddr, 8888, 0, this);
    testReader->scanStop();
    connect(testReader, SIGNAL(reportEpc(QString,int)), this, SLOT(updateEpc(QString, int)));
//    connect(testReader, SIGNAL(reportEpc(QString,int)), this, SLOT(testUpdateEpc(QString, int)));
    QTimer::singleShot(1000, this, SLOT(initEpc()));
    initColName();
}

void RfidManager::initEpc()
{
    //[epc_code|goods_code|time_stamp|state]
    QSqlQuery query = SqlManager::getRfidTable();
//    qDebug()<<query.lastQuery();

    while(query.next())
    {
        EpcInfo* info = new EpcInfo(query.value(0).toString(), query.value(1).toString());
//        qDebug()<<info->epcId;
        info->lastStamp = query.value(2).toUInt();
        info->state = (EpcState)query.value(3).toInt();
        map_rfid.insert(info->epcId, info);
//        qDebug()<<map_rfid.count();
//        emit updateEpcInfo(info);
    }
    flagInit = true;
    qDebug()<<"flagInit";
}

//GoodsInfo:物品信息表 [package_id|goods_id|package_type|name|abbname|size|unit|cab_col|cab_row|single_price]
//EpcInfo:RFID标签表 [epc_code|goods_code|goods_id|time_stamp|opt_id|state|row|col]
//[物品|条码|RFID|规格|操作人|时间]
void RfidManager::initColName()
{
    map_col_name.insert("epc_code", "RFID");
    map_col_name.insert("goods_code", "条码");
    map_col_name.insert("name", "物品");
    map_col_name.insert("size", "规格");
    map_col_name.insert("opt_id", "操作人");
    map_col_name.insert("time_stamp", "时间");
}

void RfidManager::startScan()
{
//    testReader->scanStop();
    testReader->scanStart(insideAnt, 1);
    flagScan = true;
}

void RfidManager::stopScan()
{
    testReader->scanStart(outsideAnt|insideAnt, 1);
    clsStamp = QDateTime::currentMSecsSinceEpoch();
    clsTimeOut();
//    QTimer::singleShot(clsTime, this, SLOT(clsTimeOut()));
}

//store:1  fetch:2 refund:3 back:16
void RfidManager::clsFinish()
{
//    if(!list_new.isEmpty())
//        emit epcAccess(list_new, 1);
//    if(!list_out.isEmpty())
//        emit epcAccess(list_out, 2);
//    if(!list_back.isEmpty())
//        emit epcAccess(list_back, 16);

    foreach(QString epc, list_out)
    {
        map_rfid[epc]->state = epc_out;
    }
    foreach(QString epc, list_new)
    {
        map_rfid[epc]->state = epc_in;
    }
    foreach(QString epc, list_back)
    {
        map_rfid[epc]->state = epc_in;
    }

    epcSync();
    recordClear();
    flagCorct = false;
    testReader->scanStop();
    flagScan = false;
}

void RfidManager::doorStateChanged(int id, bool isOpen)
{
    qDebug()<<"doorStateChanged"<<id<<isOpen;
    if(config->getOptId().isEmpty())
        return;
    if(isOpen)
    {
        doorState |= (1<<id);
        if(!flagScan)
            startScan();
    }
    else
    {
        doorState &= ~(1<<id);
        if(flagScan && (!doorState))//扫描状态且柜门全关
            stopScan();
    }
}

void RfidManager::listShow(QStringList epcs, QTableWidget *table, TableMark mark)
{
    if(!epcs.isEmpty())
    {
        QSqlQuery query = SqlManager::checkRfid(epcs);
//        qDebug()<<"mark"<<mark;
        qDebug()<<"listShow query:"<<query.lastQuery();
        queryShow(query, table);
        tabMark |= mark;
    }
    else
    {
        table->clearContents();
        tabMark &= ~mark;
    }
}

void RfidManager::epcCheck(int row, int col)
{
    Q_UNUSED(row);
    Q_UNUSED(col);
    flagCorct = true;
    tabMark = 0;
    scanTimer.start();

    foreach(EpcInfo* info, map_rfid)
    {
        if((info->lastStamp < clsStamp) && (info->state == epc_in))
        {
            list_out<<info->epcId;
            info->lastOpt = config->getOptId();
//            info->state = epc_out;
//            qDebug()<<"epcCheck"<<list_out.count()<<info->state;
        }
    }

    //取出物品列表
    listShow(list_out, table_out, tab_out);

    //还回物品列表
    listShow(list_back, table_back, tab_back);

    //存入物品列表
    listShow(list_new, table_in, tab_in);

    emit epcStateChanged((TableMark)tabMark);
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

void RfidManager::newRfidMark(QString epc, QString goodsCode, QString goodsId)
{
    if(map_rfid.contains(epc))
        return;

    EpcInfo* info = new EpcInfo(epc, goodsCode);
    map_rfid.insert(epc, info);
    SqlManager::insertRfidMark(epc, goodsCode, goodsId);
}

void RfidManager::queryShow(QSqlQuery query, QTableWidget *table)
{
    QSqlQueryModel model;
    model.setQuery(query);
//    qDebug()<<table<<model.rowCount();
    if((table == NULL) || (model.rowCount() <= 0))
    {
        qWarning()<<"[queryShow]"<<"NULL table or empty query.";
        return;
    }

    table->clear();
    table->setColumnCount(query.record().count());
    table->setRowCount(model.rowCount());
//    qDebug()<<table->rowCount()<<table->columnCount();
    QStringList labs;
    for(int i=0; i<query.record().count(); i++)
    {
        labs<<map_col_name.value(query.record().fieldName(i), query.record().fieldName(i));
    }
    table->setHorizontalHeaderLabels(labs);
    int i=0;
    query.seek(0);
    do
    {
        for(int j=0; j<table->columnCount(); j++)
        {
            table->setItem(i, j, new QTableWidgetItem(query.value(j).toString()));
//            qDebug()<<query.value(j).toString();
        }
        table->item(i, 4)->setText(config->getOptId());
        i++;
    }while(query.next());
    QScrollBar* bar = table->verticalScrollBar();
//    bar->setMaximum(100);
//    bar->setValue(100);
//    qDebug()<<"max"<<bar->maximum();
    bar->setValue(bar->maximum());
}

void RfidManager::recordClear()
{
    list_con.clear();
    list_epc.clear();
    list_new.clear();
    list_out.clear();
    list_back.clear();
}

void RfidManager::updateEpc(QString epc, int ant)
{
    EpcInfo* info = map_rfid.value(epc, NULL);

    if(info == NULL)
        return;

    bool needUpdateOutList = false;

    if((1<<(ant-1)) & insideAnt)//内部天线
    {
        switch(info->state)
        {
        case epc_no://入库标签
            info->lastStamp = QDateTime::currentMSecsSinceEpoch();
//            info->state = epc_in;
            info->colPos = ant;
            info->lastOpt = config->getOptId();
            if(list_new.indexOf(epc) < 0)
            {
                list_new<<epc;
                listShow(list_new, table_in, tab_in);
            }
//            emit updateEpcInfo(info);
            break;
        case epc_out://还回标签
            info->lastStamp = QDateTime::currentMSecsSinceEpoch();
//            info->state = epc_in;
            info->colPos = ant;
            info->lastOpt = config->getOptId();
            list_back<<epc;
//            emit updateEpcInfo(info);
            break;
        case epc_in://刷新时间戳
            info->lastStamp = QDateTime::currentMSecsSinceEpoch();
            if(flagCorct && (list_out.indexOf(epc) >= 0))//校准标志下，如果标签在取出表中，应移除
            {
                list_out.removeOne(epc);
//                info->state = epc_in;
                info->lastOpt = QString();
                listShow(list_out, table_out, tab_out);
                qDebug()<<"remove"<<epc<<list_out.count();
                if(list_out.isEmpty())
                {
                    qDebug()<<scanTimer.elapsed()<<"ms";

                    emit epcStateChanged((TableMark)tabMark);
                }
            }
            break;
        case epc_consume://已消耗标签
            return;
        default:
            break;
        }
    }
    else if((1<<(ant-1)) & outsideAnt)
    {
        switch(info->state)
        {
        case epc_in:
            info->lastOpt = config->getOptId();
            list_out<<epc;
            qDebug()<<"outside ant fetch:"<<list_out.count();
            needUpdateOutList = !(tabMark & tab_out);//原本未显示取出清单
            listShow(list_out, table_out, tab_out);
            if(needUpdateOutList)//原本未显示取出清单
                emit epcStateChanged((TableMark)tabMark);
            break;

        default:
            break;
        }
    }
}

void RfidManager::testUpdateEpc(QString epc, int ant)
{
    if(!map_rfid.contains(epc))
    {
        newRfidMark(epc, "D00002", "TEST-0001");
    }
    updateEpc(epc, ant);
}

void RfidManager::clsTimeOut()
{
    epcCheck(clsStamp);
}

EpcInfo::EpcInfo(QString id, QString _goodsCode)
{
    epcId = id;
    goodsCode = _goodsCode;
    lastStamp = 0;
    state = epc_no;
    rowPos = 0;
    colPos = 0;
}