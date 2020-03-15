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
    eModel = NULL;
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
    testReader2 = new RfidReader(QHostAddress("192.168.0.9"), 8888, 1, this);
    list_device<<testReader;
    list_device<<testReader2;
    foreach (RfidReader* reader, list_device)
    {
        connect(reader, SIGNAL(reportEpc(QString,int,int)), this, SLOT(updateEpc(QString, int, int)));
    }
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
    qDebug()<<"[startScan]";
    foreach(RfidReader* reader, list_device)
    {
        reader->scanStart(insideAnt, 1);
    }
    flagScan = true;

}

void RfidManager::stopScan()
{
    qDebug()<<"[stopScan]";
    foreach(RfidReader* reader, list_device)
    {
        reader->scanStart(outsideAnt|insideAnt, 1);
    }
    clsStamp = QDateTime::currentMSecsSinceEpoch();
    clsTimeOut();
//    QTimer::singleShot(clsTime, this, SLOT(clsTimeOut()));
}

//store:1  fetch:2 refund:3 back:16
void RfidManager::clsFinish()
{

//    foreach(QString epc, list_out)
//    {
//        map_rfid[epc]->state = epc_out;
//    }
//    foreach(QString epc, list_new)
//    {
//        map_rfid[epc]->state = epc_in;
//    }
//    foreach(QString epc, list_back)
//    {
//        map_rfid[epc]->state = epc_in;
//    }

//    epcSync();
//    recordClear();
    eModel->syncUpload();
    flagCorct = false;

    foreach (RfidReader* reader, list_device)
    {
        reader->scanStop();
    }
    flagScan = false;
}

void RfidManager::doorStateChanged(int id, bool isOpen)
{
    qDebug()<<"doorStateChanged"<<id<<isOpen;
    if(config->getOptId().isEmpty())
        return;
    if(isOpen)
    {
        flagCorct = false;
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
            flagCorct = true;
            stopScan();
        }
    }
    qDebug()<<"close&scan"<<doorState<<flagScan;
}

void RfidManager::listShow(QStringList epcs, QTableWidget *table, TableMark mark)
{
    if(!epcs.isEmpty())
    {
        QSqlQuery query = SqlManager::checkRfid(epcs);
//        qDebug()<<"mark"<<mark;
//        qDebug()<<"listShow query:"<<query.lastQuery();
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
//    flagCorct = true;
    tabMark = 0;

    timerStart();

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

    list_ign.clear();

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

void RfidManager::initTableViews(QTableView *in, QTableView *out, QTableView *back, QTableView *con, QTableView *check)
{
//    table_in = in;
//    table_out = out;
//    table_back = back;
//    table_con = con;
//    table_check = check;
    model_in = new QSortFilterProxyModel();
    model_con = new QSortFilterProxyModel();
    model_out = new QSortFilterProxyModel();
    model_back = new QSortFilterProxyModel();
    model_check = new QSortFilterProxyModel();

    eModel = new EpcModel();
    model_in->setSourceModel(eModel);
    model_con->setSourceModel(eModel);
    model_out->setSourceModel(eModel);
    model_back->setSourceModel(eModel);
    model_check->setSourceModel(eModel);

    in->setModel(model_in);
    out->setModel(model_out);
    back->setModel(model_back);
    con->setModel(model_con);
    check->setModel(model_check);
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
        table->item(i, 5)->setText(config->getOptId());
        if(map_rfid.contains(query.value(1).toString()))
            table->item(i, 6)->setText(QDateTime::fromMSecsSinceEpoch(map_rfid[query.value(1).toString()]->lastStamp).toString("yyyy-MM-dd hh:mm:ss"));
        i++;
    }while(query.next());
    QScrollBar* bar = table->verticalScrollBar();
//    bar->setMaximum(100);
//    bar->setValue(100);
//    qDebug()<<"max"<<bar->maximum();
    bar->setValue(bar->maximum());
    table->resizeColumnsToContents();
}

void RfidManager::recordClear()
{
    list_ign.clear();
    list_con.clear();
    list_epc.clear();
    list_new.clear();
    list_out.clear();
    list_back.clear();
    emit updateCount(list_new.count());
}

void RfidManager::timerStart()
{
    scanTimer.start();
    upTimer.start(1000);
    connect(&upTimer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
}

void RfidManager::timerStop()
{
    upTimer.stop();
    disconnect(&upTimer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
}

void RfidManager::updateEpc(QString epc, int seq, int ant)
{
    Q_UNUSED(seq);
    EpcInfo* info = eModel[epc];

    if(info == NULL)
        return;

    bool needUpdateOutList = false;

    if((1<<(ant-1)) & insideAnt)//内部天线
    {
        switch(info->state)
        {
        case epc_no://入库标签
//            info->lastStamp = QDateTime::currentMSecsSinceEpoch();
//            info->state = epc_in;
//            info->colPos = ant;
//            info->lastOpt = config->getOptId();
            eModel->setEpcMark(epc, mark_new);
//            if(list_new.indexOf(epc) < 0)
//            {
//                list_new<<epc;
//                emit updateCount(list_new.count());
//                qDebug()<<"[in count]:"<<list_new.count();
//                listShow(list_new, table_in, tab_in);
//            }
//            if(list_new.count() == map_rfid.count())
//            if(eModel->markInfoCompleted())
//            {
//                qDebug()<<scanTimer.elapsed()<<"ms";
//                emit updateTimer(scanTimer.elapsed());
//            }
            break;
        case epc_out://还回标签
            eModel->setEpcMark(epc, mark_back);
//            if(list_back.indexOf(epc) < 0)
//            {
//                info->lastStamp = QDateTime::currentMSecsSinceEpoch();
//                info->colPos = ant;
//                info->lastOpt = config->getOptId();
//                list_back<<epc;
//                qDebug()<<"[back count]:"<<list_back.count();
//            }
            break;
        case epc_in://刷新时间戳
            if(flagCorct)
                eModel->setEpcMark(epc, mark_in);
            else
            {
                eModel->setEpcMark(epc, mark_checked);
            }

//            if(flagCorct && (list_out.indexOf(epc) >= 0))//校准标志下，如果标签在取出表中，应移除
//            {
//                list_out.removeOne(epc);
//                info->lastOpt = QString();
//                listShow(list_out, table_out, tab_out);
//                qDebug()<<"remove"<<epc<<"[out count]:"<<list_out.count()<<"[ign count]:"<<list_ign.count();
//                if(list_out.isEmpty())
//                {
//                    qDebug()<<scanTimer.elapsed()<<"ms";
//                    emit updateTimer(scanTimer.elapsed());
//                }
//            }
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
    epcCheck(clsStamp);
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
