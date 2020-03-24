#include "rfidmanager.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QDateTime>
#include <QTimer>
#include <QSettings>

#include "manager/signalmanager.h"

RfidManager::RfidManager(EpcModel *model, QObject *parent) : QObject(parent)
{
    eModel = model;
    accessLock = false;
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
    testReader2 = new RfidReader(QHostAddress("192.168.0.9"), 8888, 1, this);
    list_device<<testReader;
    list_device<<testReader2;
//    foreach (RfidReader* reader, list_device)
//    {
//        connect(reader, SIGNAL(reportEpc(QString,int,int)), this, SLOT(updateEpc(QString, int, int)));
//    }
//    connect(testReader, SIGNAL(reportEpc(QString,int)), this, SLOT(testUpdateEpc(QString, int)));
    QTimer::singleShot(1000, this, SLOT(initEpc()));
    initColName();
}

void RfidManager::setCurOptId(QString optId)
{
    if(eModel)
        eModel->setOptId(optId);
}

void RfidManager::initEpc()
{
    eModel->syncDownload();
//    emit optFinish();
    //[epc_code|goods_code|time_stamp|state]
//    QSqlQuery query = SqlManager::getRfidTable();

//    while(query.next())
//    {
//        EpcInfo* info = new EpcInfo(query.value(0).toString(), query.value(1).toString());
//        info->lastStamp = query.value(2).toLongLong();
//        info->state = (EpcState)query.value(3).toInt();
//        map_rfid.insert(info->epcId, info);
//    }
//    flagInit = true;
//    qDebug()<<"flagInit";
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
    map_col_name.insert("pro_name", "生产商");
    map_col_name.insert("sup_name", "供应商");
}

void RfidManager::startScan()
{
//    testReader->scanStop();
    eModel->clearEpcMark();
    qDebug()<<"[startScan]";
    foreach(RfidReader* reader, list_device)
    {
        connect(reader, SIGNAL(reportEpc(QString,int,int)), this, SLOT(updateEpc(QString, int, int)));
        reader->scanStart(insideAnt, 1);
    }
    flagScan = true;
    timerStart();
}

void RfidManager::doorCloseScan()
{
    qDebug()<<"[stopScan]";
    foreach(RfidReader* reader, list_device)
    {
        reader->scanStart(outsideAnt|insideAnt, 1);
    }
    clsStamp = QDateTime::currentMSecsSinceEpoch();//关门时间
//    clsTimeOut();
//    QTimer::singleShot(clsTime, this, SLOT(clsTimeOut()));
}

//store:1  fetch:2 refund:3 back:16
void RfidManager::clsFinish()
{
    foreach (RfidReader* reader, list_device)
    {
        reader->scanStop();
        disconnect(reader, SIGNAL(reportEpc(QString,int,int)), this, SLOT(updateEpc(QString, int, int)));
    }
    flagScan = false;
    eModel->syncUpload();
    accessLock = false;
}

//open: mark_in -> mark_checked
//close:mark_checked -> mark_out
void RfidManager::doorStateChanged(int id, bool isOpen)
{
    qDebug()<<"doorStateChanged"<<id<<isOpen;
    if(config->getOptId().isEmpty())
        return;
    if(isOpen)
    {
        //锁定存取的状态下有门被打开,存取锁定状态解除
        //mark_in -> mark_checked
        if(accessLock)
        {
            accessLock = false;
            eModel->transEpcMark(mark_in, mark_checked);
        }
        doorState |= (1<<id);
        if(!flagScan)
        {
            startScan();
        }
    }
    else
    {
        doorState &= ~(1<<id);
        if(flagScan && (!doorState))//扫描状态且柜门全关
        {
            eModel->transEpcMark(mark_checked, mark_out);
            accessLock = true;
            emit optFinish();
            doorCloseScan();
        }
    }
    qDebug()<<"close&scan"<<doorState<<flagScan;
}

//void RfidManager::listShow(QStringList epcs, QTableWidget *table, TableMark mark)
//{
//    if(!epcs.isEmpty())
//    {
//        QSqlQuery query = SqlManager::checkRfid(epcs);
//        queryShow(query, table);
//        tabMark |= mark;
//    }
//    else
//    {
//        table->clearContents();
//        tabMark &= ~mark;
//    }
//}

void RfidManager::epcCheck(int row, int col)
{

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

void RfidManager::newRfidMark(QString epc, QString goodsCode, QString goodsId)
{
    if(map_rfid.contains(epc))
        return;

    EpcInfo* info = new EpcInfo(epc, goodsCode);
    map_rfid.insert(epc, info);
    SqlManager::insertRfidMark(epc, goodsCode, goodsId);
}

//void RfidManager::queryShow(QSqlQuery query, QTableWidget *table)
//{
//    QSqlQueryModel model;
//    model.setQuery(query);
//    if((table == NULL) || (model.rowCount() <= 0))
//    {
//        qWarning()<<"[queryShow]"<<"NULL table or empty query.";
//        return;
//    }

//    table->clear();
//    table->setColumnCount(query.record().count());
//    table->setRowCount(model.rowCount());
//    QStringList labs;
//    for(int i=0; i<query.record().count(); i++)
//    {
//        labs<<map_col_name.value(query.record().fieldName(i), query.record().fieldName(i));
//    }
//    table->setHorizontalHeaderLabels(labs);
//    int i=0;
//    query.seek(0);
//    do
//    {
//        for(int j=0; j<table->columnCount(); j++)
//        {
//            table->setItem(i, j, new QTableWidgetItem(query.value(j).toString()));
//        }
//        table->item(i, 5)->setText(config->getOptId());
//        if(map_rfid.contains(query.value(1).toString()))
//            table->item(i, 6)->setText(QDateTime::fromMSecsSinceEpoch(map_rfid[query.value(1).toString()]->lastStamp).toString("yyyy-MM-dd hh:mm:ss"));
//        i++;
//    }while(query.next());
//    QScrollBar* bar = table->verticalScrollBar();
//    bar->setValue(bar->maximum());
//    table->resizeColumnsToContents();
//}

void RfidManager::recordClear()
{
    list_ign.clear();
    list_con.clear();
    list_epc.clear();
    list_new.clear();
    list_out.clear();
    list_back.clear();
//    emit updateCount(list_new.count());
}

void RfidManager::timerStart()
{
    scanTimer.start();
    upTimer.start(1000);
    connect(&upTimer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
}

void RfidManager::timerClear()
{
    scanTimer.start();
}

void RfidManager::timerStop()
{
    upTimer.stop();
    disconnect(&upTimer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    timerUpdate();
}

void RfidManager::updateEpc(QString epc, int seq, int ant)
{
    Q_UNUSED(seq);
    EpcInfo* info = eModel->getEpcInfo(epc);

    if(info == NULL)
        return;

//    bool needUpdateOutList = false;

    if((1<<(ant-1)) & insideAnt)//内部天线
    {
        switch(info->state)
        {
        case epc_no://入库标签
            eModel->setEpcMark(epc, mark_new);
            break;
        case epc_out://还回标签
            eModel->setEpcMark(epc, mark_back);
            break;
        case epc_in://刷新时间戳
            if(accessLock)
                eModel->setEpcMark(epc, mark_in);
            else
            {
                eModel->setEpcMark(epc, mark_checked);
            }
            break;
        case epc_consume://已消耗标签
            return;
        default:
            break;
        }
    }
    else if(((1<<(ant-1)) & outsideAnt))//外部天线扫描到
    {
        eModel->setEpcMark(epc, mark_out);
        eModel->lockEpcMark(epc);
//        list_ign<<epc;
//        qDebug()<<"[ign count]"<<list_ign.count();
        switch(info->state)
        {
        case epc_in:
            eModel->setEpcMark(epc, mark_out);
            eModel->lockEpcMark(epc);
            break;
        case epc_no:
            eModel->setEpcMark(epc, mark_out);
            eModel->lockEpcMark(epc);
            break;
        default:
            break;
        }
    }
}

void RfidManager::testUpdateEpc(QString epc, int seq, int ant)
{
    if(!map_rfid.contains(epc))
    {
        newRfidMark(epc, "D00002", "TEST-0001");
    }
    updateEpc(epc, seq, ant);
}

void RfidManager::clsTimeOut()
{
//    eModel->checkEpc();
//    epcCheck(clsStamp);
}

void RfidManager::timerUpdate()
{
    emit updateTimer(scanTimer.elapsed());
}

EpcInfo::EpcInfo(QString id, QString _goodsCode)
{
    epcId = id;
    goodsCode = _goodsCode;
    lastStamp = 0;
    state = epc_no;
    mark = mark_no;
    markLock = false;
    rowPos = 0;
    colPos = 0;
}
