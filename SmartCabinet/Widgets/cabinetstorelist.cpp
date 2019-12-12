#include "cabinetstorelist.h"
#include "ui_cabinetstorelist.h"
#include <QPainter>
#include <qscrollbar.h>
#include <QDebug>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <qlayout.h>

CabinetStoreList::CabinetStoreList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetStoreList)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    bindItem = NULL;
    list_store = NULL;
    clearList();
    manager = GoodsManager::manager();
    storeManager = StoreListManager::manager();
    loginState = false;
    QFile qssScrollbar(":/stylesheet/styleSheet/ScrollBar.qss");
    qssScrollbar.open(QIODevice::ReadOnly);
    QString style = QString(qssScrollbar.readAll());
    ui->storeTable->verticalScrollBar()->setStyleSheet(style);
    qssScrollbar.close();
//    time_test.start(100);
//    connect(&time_test, SIGNAL(timeout()), this, SLOT(timeOut()));
}

CabinetStoreList::~CabinetStoreList()
{
    delete ui;
}

void CabinetStoreList::show()
{
    this->showFullScreen();
    if(bindItem != NULL)
        return;
    needScanAll = config->getStoreMode();
    if(!needScanAll)
    {
        emit requireScanState(false);
        ui->stackedWidget->setCurrentIndex(0);
        return;
    }

    //恢复存货单缓存
    recoverStoreCache();
    ui->stackedWidget->setCurrentIndex(1);
    ui->layout_list->setStretch(0, 1);
    ui->layout_list->setStretch(1, 0);
    ui->layout_list->setStretch(2, 3);
}

void CabinetStoreList::recvStoreTraceRst(bool success, QString msg, QString goodsCode)
{
    QString goodsId = scanDataTrans(goodsCode);
    QString showMsg = QString("%1 %2").arg(goodsCode).arg(msg);
    newMsg(showMsg);

    if(success)
    {
        msg = "存入成功";
        setStateMsg(STATE_PASS, msg);
        newMsg(msg);
        if(!storeManager->storeGoodsCode(goodsCode))
        {
            QString errorMsg = QString("%1 %2").arg(goodsCode).arg(storeManager->getErrorMsg());
            newMsg(errorMsg);
            qWarning()<<"[storeManager] store failed."<<errorMsg;
        }
        else
        {
            qDebug()<<"[storeManager] store success."<<goodsCode;
            storeSuccess(goodsId);
        }
    }
    else
    {
        setStateMsg(STATE_ERROR, msg);
    }

    updateScanPanel(goodsId);
}

bool sortByPos(CabinetStoreListItem* item1, CabinetStoreListItem* item2)
{
    return (item1->itemPos().x()*100+item1->itemPos().y()) < (item2->itemPos().x()*100+item2->itemPos().y());
}

void CabinetStoreList::storeStart(GoodsList *l)
{
    clearList();
    if(list_store != NULL)
        delete list_store;

    list_store = l;
    ui->cur_list->setText(list_store->barcode);
    storeManager->creatStoreCache(list_store);
    CabinetStoreListItem* item;
    int i = 0;

    ui->ok->setEnabled(true);

    for(i=0; i<list_store->list_goods.count(); i++)
    {
        Goods* goods = l->list_goods.at(i);

#ifdef TCP_API
        CaseAddress addr;
        addr.setAddress(goods->pos);
#else
//        CaseAddress addr = config->checkCabinetByBarCode(goods->packageBarcode);
//        goods->pos = QPoint(addr.cabinetSeqNum, addr.caseIndex);
        goods->pos = SqlManager::getGoodsPos(goods->packageId);
#endif
        qDebug()<<"storeStart"<<goods->abbName<<goods->pos;
        item = new CabinetStoreListItem(goods, goods->pos);
        connect(item, SIGNAL(requireBind(Goods*,CabinetStoreListItem*)), this, SLOT(itemBind(Goods*,CabinetStoreListItem*)));
        connect(item, SIGNAL(requireOpenCase(int,int)), this, SIGNAL(requireOpenCase(int,int)));
        addItem(item);
    }
    qSort(list_item.begin(), list_item.end(), sortByPos);
    updateStoreList(list_item);
    ui->stackedWidget->setCurrentIndex(0);
}

void CabinetStoreList::listError(QString msg)
{
    ui->stackedWidget->setCurrentIndex(0);
    newMsg(msg);
}

void CabinetStoreList::storeFinish()
{
    if(list_store != NULL)
        storeManager->removeStoreCache(list_store->barcode);

    saveList();
    clearList();
}

void CabinetStoreList::recvScanCode(QString scanCode)
{
    if(!needScanAll)//无需扫描全部物品，忽略扫描输入
    {
        Q_UNUSED(scanCode);
        return;
    }
    if(!loginState)
    {
        newMsg("系统超时锁定，请刷卡再继续操作");
    }
    int curStack = ui->stackedWidget->currentIndex();//当前stack页面
    if(curStack == 0)//存物品界面
    {
        storeScan(scanCode);
    }
    else if(curStack == 1)//存货单界面
    {
        //检查存货单是否有缓存
        int idx = list_store_cache.indexOf(scanCode);
        if(idx == -1)//是新的存货单
        {
            emit requireGoodsListCheck(scanCode);
        }
        else//旧的存货单，进行恢复操作
        {
            if(list_store != NULL)
                delete list_store;

            emit newStoreBarCode(scanCode);
            list_store = storeManager->recoverGoodsList(scanCode);
            storeContinue(list_store);
        }
    }
}

void CabinetStoreList::storeScan(QString scanCode)
{
    QString goodsId = scanDataTrans(scanCode);
    Goods* scanGoods = list_store->map_goods.value(goodsId, NULL);
    if(scanGoods == NULL)
    {
        QString msg = QString("%1 %2").arg(scanCode).arg("unknow goods.");
        qDebug()<<QString("[storeScan]:%1").arg(msg);
        newMsg(msg);
        return;
    }
    if(scanGoods->codes.indexOf(scanCode) != -1)
    {
        QString msg = QString("%1 %2").arg(scanCode).arg("重复扫描的物品");
        qDebug()<<QString("[storeScan]:%1").arg(msg);
        newMsg(msg);
        return;
    }
    updateScanPanel(goodsId);
    setStateMsg(STATE_WAIT, "已扫描，请等待");
    emit reportTraceId(scanCode);
    QTimer::singleShot(5000, this, SLOT(goodsTraceTimeout()));
}

void CabinetStoreList::bindRst(CaseAddress addr)
{
    if(bindItem == NULL)
        return;

    bindItem->bindRst(addr);
    bindItem = NULL;
}

QString CabinetStoreList::scanDataTrans(QString code)
{
    QStringList strList = code.split("-", QString::SkipEmptyParts);
    if(strList.count() < 4)
        return QString();

    strList.removeLast();
    strList = strList.mid(strList.count()-2, 2);
    QString ret = strList.join("-");
    return ret;
}

QString CabinetStoreList::stateStyleSheet(CabinetStoreList::GOODS_STATE state)
{
    QString strColor = "rgb(255, 255, 255)";
    switch(state)
    {
    case STATE_NULL:strColor = "rgb(255, 255, 255)"; break;
    case STATE_WAIT:strColor = "rgb(245, 121, 0)"; break;
    case STATE_PASS:strColor = "rgb(78, 154, 6)"; break;
    case STATE_ERROR:strColor = "rgb(239, 41, 41)"; break;
    default:
        break;
    }

    return QString("font: 75 22px \"WenQuanYi Micro Hei\";color: rgb(255, 255, 255);background-color: %1;").arg(strColor);
}

void CabinetStoreList::storeSuccess(QString goodsId)
{
    CabinetStoreListItem* item = map_item.value(goodsId, NULL);
    if(item == NULL)
        return;
    item->storeOnePac();
}

void CabinetStoreList::recoverStoreCache()
{
    list_store_cache = storeManager->getStoreCacheList();
    ui->listCache->clear();
    ui->listCache->setRowCount(list_store_cache.count());
    ui->listCache->setColumnCount(2);
    ui->listCache->setColumnWidth(0, 200);
    int i = 0;
    foreach (QString listCode, list_store_cache)
    {
        ui->listCache->setItem(i, 0, new QTableWidgetItem(listCode));
        ui->listCache->setItem(i, 1, new QTableWidgetItem("删除"));
        i++;
    }
}

void CabinetStoreList::showListPart(GoodsList *l)
{
    if(l == NULL)
        return;

    qDebug()<<"showListPart";
    list_part.clear();
    ui->list_part->clear();
    ui->list_part->setRowCount(l->list_goods.count());
    ui->list_part->setColumnCount(1);

    int i = 0;
    foreach (Goods* goods, l->list_goods)
    {
        QString str = QString("%1[%2](%3/%4)x%5").arg(goods->name).arg(goods->size).arg(goods->codes.count()).arg(goods->totalNum).arg(goods->packageType);
        ui->list_part->setItem(i, 0, new QTableWidgetItem(str));
        list_part<<goods->goodsId;
        qDebug()<<"showListPart"<<goods->goodsId;
        i++;
    }
}

void CabinetStoreList::showStoreCacheCode(QStringList codes)
{
    ui->list_code->clear();
    ui->list_code->setRowCount(codes.count());
    ui->list_code->setColumnCount(1);
    int i = 0;
    foreach (QString code, codes)
    {
        ui->list_code->setItem(i, 0, new QTableWidgetItem(code));
        i++;
    }
}

void CabinetStoreList::bindMsg(QString msg)
{
//    ui->msg->setText(msg);
    newMsg(msg);
}

void CabinetStoreList::storeRst(QString msg, bool success)
{
    if(success)
    {
       ui->ok->setEnabled(false);
       storeFinish();
    }
    else
        ui->ok->setEnabled(true);
//    ui->msg->setText(msg);
    newMsg(msg);
}

void CabinetStoreList::setLoginState(bool login)
{
    loginState = login;
    ui->ok->setVisible(login);
    if(login)
    {
        ui->msg->clear();
    }
    else
    {
        newMsg("有待存送货单，请刷卡登录操作");
    }
}

bool CabinetStoreList::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    return true;
}

void CabinetStoreList::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.fillRect(this->rect(), QColor(22, 52, 73, 200));
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CabinetStoreList::clearList()
{
    if(!map_item.isEmpty())
        map_item.clear();

    if(!list_item.isEmpty())
    {
        qDeleteAll(list_item.begin(), list_item.end());
        list_item.clear();
    }
    ui->storeTable->clear();
    ui->storeTable->setRowCount(0);
}

void CabinetStoreList::closeClear()
{
    ui->list_part->clear();
    ui->list_code->clear();
    ui->msgLog->clear();
    list_msg.clear();
    if(list_store != NULL)
        delete list_store;
    list_store = NULL;
}

void CabinetStoreList::saveList()
{
    if(list_store == NULL)
        return;
    foreach(Goods* goods, list_store->list_goods)
    {
        manager->addGoodsCodes(goods->packageId, goods->codes);
    }
}

void CabinetStoreList::storeContinue(GoodsList *l)
{
    storeManager->creatStoreCache(l);
    CabinetStoreListItem* item;
    int i = 0;

    ui->ok->setEnabled(true);

    for(i=0; i<l->list_goods.count(); i++)
    {
        Goods* goods = l->list_goods.at(i);

#ifdef TCP_API
        CaseAddress addr;
        addr.setAddress(goods->pos);
#else
        QPoint addr = SqlManager::searchByPackageId(goods->packageId);
        goods->pos = QPoint(addr.x(), addr.y());
#endif
        qDebug()<<"storeStart"<<goods->abbName<<goods->pos;
        item = new CabinetStoreListItem(goods, addr);
        connect(item, SIGNAL(requireBind(Goods*,CabinetStoreListItem*)), this, SLOT(itemBind(Goods*,CabinetStoreListItem*)));
        connect(item, SIGNAL(requireOpenCase(int,int)), this, SIGNAL(requireOpenCase(int,int)));
        addItem(item);
    }
    qSort(list_item.begin(), list_item.end(), sortByPos);
    updateStoreList(list_item);
    ui->stackedWidget->setCurrentIndex(0);
}

void CabinetStoreList::newMsg(QString msg)
{
    list_msg.prepend(msg);
    ui->msgLog->setText(list_msg.join("\n"));
    ui->msg->setText(msg);
}

bool CabinetStoreList::checkStoreList()
{
    if(list_store == NULL)
        return false;

    foreach (CabinetStoreListItem* item, list_item)
    {
        if(item->waitNum() != 0)
        {
            return false;
        }
    }
    return true;
}

void CabinetStoreList::updateStoreList(QList<CabinetStoreListItem *> l)
{
    ui->storeTable->clear();
    ui->storeTable->setRowCount(l.count());
    ui->storeTable->setColumnCount(1);
    int i=0;

    foreach (CabinetStoreListItem* item, l)
    {
        ui->storeTable->setCellWidget(i, 0, item);
        i++;
    }
}

void CabinetStoreList::updateScanGoods(Goods *goods)
{
    ui->goods_id->setText(goods->packageId);
    ui->goods_name->setText(goods->name);
    ui->goods_size->setText(goods->size);
    ui->goods_producer->setText(goods->proName);
    ui->goods_suplier->setText(goods->supName);
    ui->wait_num->setText(QString::number(goods->waitNum));
}

void CabinetStoreList::updateScanPanel(QString goodsId)
{
    CabinetStoreListItem* item = map_item.value(goodsId, NULL);
    if(item == NULL)
        return;

    Goods* goods = item->itemGoods();
    updateScanGoods(goods);
}

void CabinetStoreList::addItem(CabinetStoreListItem *item)
{
    list_item<<item;
    map_item.insert(item->itemId(), item);
}

void CabinetStoreList::setStateMsg(GOODS_STATE state, QString msg)
{
    curState = state;
    ui->goods_state->setStyleSheet(stateStyleSheet(state));
    ui->goods_state->setText(msg);
}

//test function
//void CabinetStoreList::timeOut()
//{
//    qDebug("[timeOut]");

//    if(list_item.count()<3)
//    {
//        Goods* goods = new Goods();
//        CaseAddress addr;
//        CabinetStoreListItem* item = new CabinetStoreListItem(goods, addr);
//        connect(item, SIGNAL(requireBind(Goods*,CabinetStoreListItem*)), this, SLOT(itemBind(Goods*,CabinetStoreListItem*)));
//        connect(item, SIGNAL(requireOpenCase(int,int)), this, SIGNAL(requireOpenCase(int,int)));
//        list_item<<item;
//    }
//    else
//    {
//        clearList();
//    }
//    on_ok_clicked();

//}

void CabinetStoreList::itemBind(Goods* goods, CabinetStoreListItem* item)
{
    bindItem = item;
    emit requireBind(goods);
}

void CabinetStoreList::goodsTraceTimeout()
{
    if(curState == STATE_WAIT)
        setStateMsg(STATE_ERROR, "提交物品信息超时");
}

void CabinetStoreList::on_ok_clicked()
{
    if(!loginState)
    {
        newMsg("系统超时锁定，请刷卡再继续操作");
    }
    if(needScanAll)
    {
        if(checkStoreList())//所有物品都已经扫描
        {
            if(list_store == NULL)
                return;

            SqlManager::listStoreAffirm(list_store->barcode, SqlManager::local_rep);//本地确认存货
            newMsg("已存入本地库存,正在提交..");
            ui->ok->setEnabled(false);
            emit storeList(list_item);
        }
        else
        {
            newMsg("存货单有物品未扫描");
            return;
        }
    }
    else
    {
        if(list_store == NULL)
            return;

        SqlManager::listStoreAffirm(list_store->barcode, SqlManager::local_rep);//本地确认存货
        newMsg("已存入本地库存,正在提交..");
        ui->ok->setEnabled(false);
        emit storeList(list_item);
    }
}

void CabinetStoreList::on_back_clicked()
{
//    storeFinish();
    closeClear();
    this->close();
    emit requireScanState(true);
}

void CabinetStoreList::on_listCache_cellClicked(int row, int column)
{
    QString selList = ui->listCache->item(row, 0)->text();
    if(column == 0)//see particulars
    {
        if(list_store != NULL)
            delete list_store;

        list_store = storeManager->recoverGoodsList(selList);
        showListPart(list_store);
    }
    else if(column == 1)//delete
    {
        storeManager->removeStoreCache(selList);
        recoverStoreCache();
    }
}

void CabinetStoreList::on_back_2_clicked()
{
    this->close();
    closeClear();
}

void CabinetStoreList::on_stackedWidget_currentChanged(int arg1)
{
    qDebug()<<"stackedWidget_currentChanged"<<arg1;
}

void CabinetStoreList::on_list_part_cellClicked(int row, int)
{
    QString selGoodsId = list_part.at(row);
    QStringList codes = storeManager->getStoreCacheCodes(selGoodsId);
    showStoreCacheCode(codes);
}
