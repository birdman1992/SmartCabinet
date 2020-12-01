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
    curOptPow = -1;

    config = CabinetConfig::config();

    SignalManager* sigManager = SignalManager::manager();//单例，不必delete
    connect(model, SIGNAL(epcAccess(QStringList,UserOpt)), sigManager, SIGNAL(epcAccess(QStringList,UserOpt)));
    connect(model, SIGNAL(epcAccess(QStringList,QStringList,QString)), sigManager, SIGNAL(epcAccess(QStringList,QStringList,QString)));
    connect(sigManager, SIGNAL(doorState(int,bool)), this, SLOT(doorStateChanged(int, bool)));
    connect(sigManager, SIGNAL(epcInfoUpdate()), model, SLOT(syncDownload()));
    connect(model, SIGNAL(epcConsumeCheck(QStringList)), sigManager, SIGNAL(epcConsumeCheck(QStringList)));
    connect(sigManager, SIGNAL(epcConsumed(QStringList)), model, SLOT(epcConsume(QStringList)));
    connect(model, SIGNAL(epcStore(QVariantMap)), sigManager, SIGNAL(epcStore(QVariantMap)));

    rfidHub = new RfidDevHub(this);
    connect(rfidHub, SIGNAL(reportEpc(QString,DevAction)), this, SLOT(updateEpc(QString,DevAction)));
//    rfidHub->addDevice(QHostAddress("192.168.0.8"), 8888);
//    rfidHub->addDevice(QHostAddress("192.168.0.9"), 8888);
//    QHostAddress serverAddr = QHostAddress("192.168.0.8");
//    testReader = new RfidReader(serverAddr, 8888, 0, this);
//    testReader2 = new RfidReader(QHostAddress("192.168.0.9"), 8888, 1, this);
//    list_device<<testReader;
//    list_device<<testReader2;
//    foreach (RfidReader* reader, list_device)
//    {
//        connect(reader, SIGNAL(reportEpc(QString,int,int)), this, SLOT(updateEpc(QString, int, int)));
//    }
    QTimer::singleShot(1000, this, SLOT(initEpc()));
    initColName();
    setScanLock(true);
}

void RfidManager::setCurOptId(QString optId)
{
    if(eModel)
        eModel->setCurOpt(optId);
}

void RfidManager::setCurOptPow(int pow)
{
    curOptPow = pow;
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
    setScanLock(false);
    flagScan = true;
    eModel->clearEpcMark();
    qDebug()<<"[startScan]";
    foreach(RfidReader* reader, rfidHub->deviceList())
    {
//        connect(reader, SIGNAL(reportEpc(QString,int,int)), this, SLOT(updateEpc(QString, int, int)));
//        reader->scanStart(RF_REP|RF_FETCH|RF_AUTO|RF_WARNING,1);
        reader->scanStart(RF_REP|RF_AUTO, 1);
    }
    connect(rfidHub, SIGNAL(reportEpc(QString,DevAction)), this, SLOT(updateEpc(QString,DevAction)));
    timerStart();
}

void RfidManager::scanRestart()
{
    setScanLock(false);
    eModel->clearEpcMark();
    qDebug()<<"[RFID scan] restart";
    foreach (RfidReader* reader, rfidHub->deviceList())
    {
        reader->scanStop();
    }
    foreach(RfidReader* reader, rfidHub->deviceList())
    {
//        reader->scanStart(RF_REP|RF_AUTO, 1);
        reader->scanStart(RF_REP|RF_FETCH|RF_AUTO|RF_WARNING, 1);
    }
    timerStart();
    clsStamp = QDateTime::currentMSecsSinceEpoch();//关门时间
}

/**
 * @brief RfidManager::doorCloseScan
 * 关门扫描,仅使用外部天线
 */
void RfidManager::doorCloseScan()
{
    setScanLock(false);
    qDebug()<<"[doorCloseScan]";
    foreach (RfidReader* reader, rfidHub->deviceList())
    {
        reader->scanStop();
    }
    foreach(RfidReader* reader, rfidHub->deviceList())
    {
        reader->scanStart(RF_REP|RF_FETCH|RF_AUTO|RF_WARNING, 1);
    }
    clsStamp = QDateTime::currentMSecsSinceEpoch();//关门时间
//    clsTimeOut();
//    QTimer::singleShot(clsTime, this, SLOT(clsTimeOut()));
}

//store:1  fetch:2 refund:3 back:16
void RfidManager::clsFinish()
{
    qDebug()<<"[stop Scan]";
    foreach (RfidReader* reader, rfidHub->deviceList())
    {
        reader->scanStop();
//        disconnect(reader, SIGNAL(reportEpc(QString,int,int)), this, SLOT(updateEpc(QString, int, int)));
    }
    disconnect(rfidHub, SIGNAL(reportEpc(QString,DevAction)), this, SLOT(updateEpc(QString,DevAction)));
    flagScan = false;
    eModel->syncUpload();
    accessLock = false;
}

void RfidManager::clsGiveUp()
{
    qDebug()<<"[stop Scan]";
    foreach (RfidReader* reader, rfidHub->deviceList())
    {
        reader->scanStop();
//        disconnect(reader, SIGNAL(reportEpc(QString,int,int)), this, SLOT(updateEpc(QString, int, int)));
    }
    disconnect(rfidHub, SIGNAL(reportEpc(QString,DevAction)), this, SLOT(updateEpc(QString,DevAction)));
    flagScan = false;
    accessLock = false;
    setScanLock(true);
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
            accessLock = true;
            eModel->transEpcMark(mark_checked, mark_no);
            emit optFinish();
            doorCloseScan();
        }
    }
    qDebug()<<"doorState:"<<doorState<<"flagScan:"<<flagScan<<"accessLock:"<<accessLock;
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
    Q_UNUSED(row);
    Q_UNUSED(col);
}

void RfidManager::epcSync()
{
    SqlManager::begin();
    foreach (EpcInfo* info, map_rfid)
    {
        SqlManager::updateRfidsSingle(info->epcId, info->lastStamp, info->lastOpt, info->state, info->rowPos, info->colPos);
    }
    SqlManager::commit();
}

void RfidManager::setScanLock(bool lock)
{
    scanLock = lock;
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

bool RfidManager::accessIsLock()
{
    return accessLock;
}

RfidDevHub *RfidManager::rfidReaderModel()
{
    return rfidHub;
}

void RfidManager::timerStop()
{
    upTimer.stop();
    disconnect(&upTimer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    timerUpdate();
}

void RfidManager::updateEpc(QString epc, DevAction rfAct)
{
    if(scanLock)
        return;

    EpcInfo* info = eModel->getEpcInfo(epc);

    if(info == NULL)
    {
        eModel->unknowEpc(epc);
        return;
    }

//    qDebug()<<"updateEpc"<<epc<<rfAct;
//        qDebug()<<"[updateEpc]"<<epc<<isOutside;
//    bool needUpdateOutList = false;
    if(rfAct == RF_REP)//内部天线
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
    else if(rfAct == RF_FETCH)//外部天线扫描到
    {
//        eModel->setEpcMark(epc, mark_out);
//        eModel->lockEpcMark(epc);
        switch(info->state)
        {
        case epc_in:
            eModel->setEpcMark(epc, mark_out);
            eModel->lockEpcMark(epc);
            break;
//        case epc_no://无状态的标签也会被锁定
//            eModel->setEpcMark(epc, mark_no);
//            eModel->lockEpcMark(epc);
//            break;
        default:
            break;
        }
    }
    else if(rfAct == RF_AUTO)
    {
        switch(info->state)
        {
        case epc_no://入库标签
            if(curOptPow==1 || curOptPow==2 || curOptPow==4)//护士，护士长，医院员工不关注存货
                break;

            eModel->setEpcMark(epc, mark_new);
            break;
        case epc_out://还回标签
            eModel->setEpcMark(epc, mark_back);
            break;
        case epc_in://取出标签
            if(curOptPow==3)//仓管不关注取货
                break;

            eModel->setEpcMark(epc, mark_out);
//            eModel->lockEpcMark(epc);
            break;
//        case epc_consume://已消耗标签
//            return;
        default:
            break;
        }
    }
}

//void RfidManager::testUpdateEpc(QString epc, int seq, int ant)
//{
//    if(!map_rfid.contains(epc))
//    {
//        newRfidMark(epc, "D00002", "TEST-0001");
//    }
//    updateEpc(epc, seq, ant);
//}

void RfidManager::clsTimeOut()
{
//    eModel->checkEpc();
//    epcCheck(clsStamp);
}

void RfidManager::timerUpdate()
{
    emit updateTimer(scanTimer.elapsed());
}

