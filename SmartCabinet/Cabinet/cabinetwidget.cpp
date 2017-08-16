#include "cabinetwidget.h"
#include "ui_cabinetwidget.h"
#include <QDebug>
#include <QDateTime>
#include "defines.h"
#include "Device/controldevice.h"

//提示信息
#define MSG_EMPTY ""
#define MSG_SCAN_LIST "请扫描送货单条码"
#define MSG_LIST_ERROR "无效的送货单"
#define MSG_STORE "请扫描待存放物品条形码"
#define MSG_STORE_SELECT "请选择绑定位置"
#define MSG_STORE_SELECT_REPEAT "选择的位置被占用 请重新选择"
#define MSG_FETCH "请选择要取出的物品 柜门打开后请扫描条形码取出"
#define MSG_FETCH_SCAN "请扫描条形码取出物品 取用完毕请点击此处并关闭柜门"
#define MSG_FETCH_EMPTY "没有库存了 请关好柜门 点击此处退出"
#define MSG_CHECK  "请点击柜格开始盘点"
#define MSG_REFUND  "点击柜格扫码退货"
#define MSG_OFFLINE  "当前为离线状态，请点击 切换 按钮批量取货"

CabinetWidget::CabinetWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetWidget)
{
    ui->setupUi(this);
    storeNum = 0;
    clickLock = true;
    waitForCodeScan = false;
    waitForGoodsListCode = false;
    waitForServer = false;
    waitForCardReader = true;
    waitForInit = true;
    loginState = false;
    curStoreList = NULL;
    msgBox = NULL;
    selectCase = -1;
    selectCab = -1;
    win_access = new CabinetAccess();
    win_cab_list_view = new CabinetListView();
    win_check = new CabinetCheck();
    win_store_list = new CabinetStoreList();
    win_refund = new CabinetRefund();

    initSearchBtns();
    connect(win_access, SIGNAL(saveStore(Goods*,int)), this, SLOT(saveStore(Goods*,int)));
    connect(win_access, SIGNAL(saveFetch(QString,int)), this, SLOT(saveFetch(QString,int)));
    connect(this, SIGNAL(goodsNumChanged(int)), win_access, SLOT(recvOptGoodsNum(int)));

    connect(win_cab_list_view, SIGNAL(requireAccessList(QStringList,int)), this, SIGNAL(requireAccessList(QStringList,int)));
    connect(win_cab_list_view, SIGNAL(requireOpenCase(int,int)), this, SIGNAL(requireOpenCase(int,int)));

    connect(win_check, SIGNAL(checkCase(QList<CabinetCheckItem*>,CaseAddress)), this, SLOT(checkOneCase(QList<CabinetCheckItem*>,CaseAddress)));

    connect(win_store_list, SIGNAL(requireBind(Goods*)), this, SLOT(cabinetBind(Goods*)));
    connect(win_store_list, SIGNAL(requireOpenCase(int,int)), this, SIGNAL(requireOpenCase(int,int)));
    connect(win_store_list, SIGNAL(storeList(QList<CabinetStoreListItem*>)), this, SIGNAL(storeList(QList<CabinetStoreListItem*>)));

    connect(win_refund, SIGNAL(refundCase(QStringList,int)), this, SIGNAL(requireAccessList(QStringList,int)));
//    optUser = QString();
    ui->store->hide();
    ui->refund->hide();
    ui->service->hide();
    ui->cut->hide();
    ui->check->hide();
    ui->search->hide();
    ui->menuWidget->setCurrentIndex(0);

#ifndef SIMULATE_ON
    ui->msk1->hide();
    ui->msk2->hide();
#endif
}

CabinetWidget::~CabinetWidget()
{
    delete win_access;
    delete win_refund;
    delete win_cab_list_view;
    delete win_check;
    delete win_store_list;
    delete ui;
}

void CabinetWidget::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CabinetWidget::cabLock()
{
    optUser = NULL;
    ui->userInfo->setText("请刷卡使用");
    storeNum = 0;
    loginState = false;
    win_store_list->setLoginState(loginState);
    clickLock = true;
    waitForCodeScan = false;
    waitForGoodsListCode = false;
    waitForCardReader = true;
    curStoreList = NULL;
    msgBox = NULL;
    config->list_cabinet[0]->showMsg(MSG_EMPTY,false);
    ui->store->hide();
    ui->service->hide();
    ui->cut->hide();
    ui->refund->hide();
    ui->check->hide();
    ui->search->hide();
    curStoreList = NULL;
    config->state = STATE_FETCH;
    config->clearSearch();
}

void CabinetWidget::cabInfoBind(int seq, int index, GoodsInfo info)
{
    qDebug()<<"bind"<<info.id;
    info.goodsType = config->getGoodsType(info.packageId);
    qDebug()<<info.goodsType;
    config->list_cabinet[seq]->setCaseName(info, index);
    emit requireCaseBind(seq, index, info.packageId);
}

void CabinetWidget::initAccessState()
{
    ui->store->setChecked(false);
    config->state = STATE_FETCH;
}

void CabinetWidget::initSearchBtns()
{
    groupBtn.addButton(ui->btn_0, 0);
    groupBtn.addButton(ui->btn_1, 1);
    groupBtn.addButton(ui->btn_2, 2);
    groupBtn.addButton(ui->btn_3, 3);
    groupBtn.addButton(ui->btn_4, 4);
    groupBtn.addButton(ui->btn_5, 5);
    groupBtn.addButton(ui->btn_6, 6);
    groupBtn.addButton(ui->btn_7, 7);
    groupBtn.addButton(ui->btn_8, 8);
    groupBtn.addButton(ui->btn_9, 9);
    groupBtn.addButton(ui->btn_10, 10);
    groupBtn.addButton(ui->btn_11, 11);
    groupBtn.addButton(ui->btn_12, 12);
    groupBtn.addButton(ui->btn_13, 13);
    groupBtn.addButton(ui->btn_14, 14);
    groupBtn.addButton(ui->btn_15, 15);
    groupBtn.addButton(ui->btn_16, 16);
    groupBtn.addButton(ui->btn_17, 17);
    groupBtn.addButton(ui->btn_18, 18);
    groupBtn.addButton(ui->btn_19, 19);
    groupBtn.addButton(ui->btn_20, 20);
    groupBtn.addButton(ui->btn_21, 21);
    groupBtn.addButton(ui->btn_22, 22);
    groupBtn.addButton(ui->btn_23, 23);
    groupBtn.addButton(ui->btn_24, 24);
    groupBtn.addButton(ui->btn_25, 25);
    groupBtn.setExclusive(true);

    connect(&groupBtn, SIGNAL(buttonClicked(int)), this, SLOT(pinyinSearch(int)));
}

bool CabinetWidget::needWaitForServer()
{
    if(waitForServer)
        return true;
    else
    {
        waitForServer = true;
//        QTimer::singleShot(5000, this, SLOT(wait_timeout()));
        return false;
    }
}

void CabinetWidget::showCurrentTime(QString curTime)
{
    ui->time->setText(curTime);
}

void CabinetWidget::clearCheckState()
{


}

void CabinetWidget::clearMenuState()
{
    ui->store->setChecked(false);
    ui->refund->setChecked(false);
    ui->check->setChecked(false);
    ui->service->setChecked(false);
}

QByteArray CabinetWidget::scanDataTrans(QByteArray code)
{
    int index = code.indexOf("-");
    if(index == -1)
        return code;

    code = code.right(code.size()-index-1);

    index = code.lastIndexOf("-");
    if(index == -1)
        return code;

    return code.left(index);
}

//初始化药柜界面
void CabinetWidget::panel_init(QList<Cabinet *> cabinets)
{
    if(!waitForInit)
        return;
    qDebug("[panel_init]");
    waitForInit = false;
    int index = cabinets.count()-1;
    qDebug()<<"index"<<index;

    ui->cabinet_layout->addStretch();

    for(; index>0; index--)
    {
        if(cabinets.at(index)->isInLeft())
            ui->cabinet_layout->addWidget(cabinets.at(index));
    }
    ui->cabinet_layout->addWidget(cabinets.at(0));
    connect(cabinets.at(0), SIGNAL(logoClicked()), this, SLOT(logoClicked()));

    index = 1;
    for(; index<(cabinets.count()); index++)
    {
        if(!cabinets.at(index)->isInLeft())
            ui->cabinet_layout->addWidget(cabinets.at(index));
    }
    ui->cabinet_layout->addStretch();

    for(index=0; index<cabinets.count(); index++)
    {
        connect(cabinets.at(index), SIGNAL(caseSelect(int,int)), this, SLOT(caseClicked(int,int)));
    }
    win_cab_list_view->setCabView(ui->frame_6);
}

void CabinetWidget::caseClicked(int caseIndex, int cabSeqNum)
{
    qDebug()<<caseIndex<<cabSeqNum;
    qDebug()<<clickLock;
//    qDebug()<<config->getCabinetId();
//    emit requireOpenCase(cabSeqNum, caseIndex);
    if((cabSeqNum == 0) && (caseIndex == 1))
        return;
    if(clickLock)//锁定状态下点击无效
    {
        if((caseIndex==selectCase) && (cabSeqNum == selectCab))
            return;
        else
            config->list_cabinet.at(cabSeqNum)->clearSelectState(caseIndex);
        return;
    }

//    bool clickRepeat = false;
//    if((caseIndex==selectCase) && (cabSeqNum == selectCab))
//        clickRepeat = true;//标记为重复点击
//    if(!config->list_cabinet[cabSeqNum]->list_case[caseIndex]->name.isEmpty())
//        clickRepeat = true;

    clickLock = true;
    selectCab = cabSeqNum;
    selectCase = caseIndex;

    if(config->state == STATE_STORE)
    {
//        if(clickRepeat)
//        {
//            config->list_cabinet[0]->showMsg(MSG_STORE_SELECT_REPEAT, false);
//            clickLock = false;
//            return;
//        }
        if(curGoods == NULL)
            return;
        GoodsInfo info;
        info.abbName = curGoods->abbName;
        info.name = curGoods->name;
        info.id = curGoods->goodsId;
        info.packageId = curGoods->packageBarcode;
        info.unit = curGoods->unit;
        info.num = 0;
        info.Py = config->getPyCh(info.name);
        qDebug()<<"[pinyin]"<<info.Py;
        cabInfoBind(selectCab, selectCase, info);
        config->list_cabinet[0]->showMsg(MSG_EMPTY,0);
//        config->list_cabinet[selectCab]->setCaseName(info, selectCase);
//        config->list_cabinet[selectCab]->consumableIn(selectCase);
//        config->list_cabinet[0]->showMsg(MSG_STORE, false);
//        win_access->clickOpen(curGoods->goodsId);
    }
    else if(config->state == STATE_FETCH)
    {
//        if(!clickRepeat)//如果该柜格没有存放药品
//        {qDebug()<<"empty";
//            selectCab = -1;
//            selectCase = -1;
//            clickLock = false;
//            return;
//        }
        //打开对应柜门
        if(!ui->netState->isChecked())//离线状态只能批量取货
        {
            config->list_cabinet[0]->showMsg(MSG_OFFLINE,0);
            return;
        }

        qDebug()<<"[CabinetWidget]"<<"[open]"<<cabSeqNum<<caseIndex;
        emit requireOpenCase(cabSeqNum, caseIndex);

        waitForCodeScan = true;
        clickLock = false;
        scanInfo = QString();
        CabinetInfo* info = config->list_cabinet[cabSeqNum]->list_case[caseIndex];
        win_access->setAccessModel(false);
        win_access->clickOpen(info);
//        config->list_cabinet[0]->showMsg(MSG_FETCH_SCAN, false);
    }
    else if(config->state == STATE_REFUN)
    {
        casePos.cabinetSeqNUM = cabSeqNum;
        casePos.caseIndex = caseIndex;
        win_refund->refundStart(casePos);
        clickLock = false;
//        emit requireOpenCase(cabSeqNum, caseIndex);

//        waitForCodeScan = true;
//        clickLock = true;
//        scanInfo = QString();
//        CabinetInfo* info = config->list_cabinet[cabSeqNum]->list_case[caseIndex];
//        win_access->setAccessModel(false);
//        win_access->clickOpen(info);
    }
    else if(config->state == STATE_CHECK)
    {
        casePos.cabinetSeqNUM = cabSeqNum;
        casePos.caseIndex = caseIndex;
        win_check->checkStart(casePos);
        config->list_cabinet[cabSeqNum]->checkCase(caseIndex);
        clickLock = false;
    }
}

void CabinetWidget::recvScanData(QByteArray qba)
{//qDebug()<<"recvScanData"<<qba;
    if(!waitForCodeScan)
    {
        qDebug()<<"[CabinetWidget]"<<"scan data not need";
        return;
    }
    if(waitForGoodsListCode)
    {
        emit requireGoodsListCheck(QString(qba));
        return;
    }

    bool newStore = false;
    QByteArray code = scanDataTrans(qba);//截取去掉唯一码,xxx-xxxxxxx-xx-xxxx  ->  xxxxxxx-xx

    if(scanInfo != QString(code))
    {
        newStore = true;
    }
    scanInfo = QString(code);
    fullScanInfo = QString(qba);
    qDebug()<<scanInfo<<fullScanInfo;

    if(config->state == STATE_STORE)
    {
        return;
        curGoods = curStoreList->getGoodsById(scanInfo);
        if(curGoods == NULL)
        {
            qDebug()<<"[recvScanData]"<<"scan goods id not find";
            return;
        }
        else
            qDebug()<<"[recvScanData]"<<"scan goods id find";
        //根据物品名搜索柜格位置
        CaseAddress pos = config->checkCabinetByBarCode(curGoods->packageBarcode);

        if(pos.cabinetSeqNUM == -1)//没有搜索到药品对应的柜格
        {
            clickLock = false;
            win_access->save();
            win_access->hide();
            config->list_cabinet[0]->showMsg(MSG_STORE_SELECT, false);
        }
        else
        {
            //打开对应柜门
            qDebug()<<"[CabinetWidget]"<<"[open]"<<pos.cabinetSeqNUM<<pos.caseIndex;
            if(newStore)
                emit requireOpenCase(pos.cabinetSeqNUM, pos.caseIndex);


            if(curGoods->curNum < curGoods->totalNum && (!needWaitForServer()))
            {
                win_access->scanOpen(curGoods->packageBarcode);
                CaseAddress addr = config->checkCabinetByBarCode(scanInfo);
                emit goodsAccess(addr,fullScanInfo, 1, 2);
            }
//            storeNum++;
//            config->list_cabinet[0]->showMsg(MSG_STORE+
//                                             QString("\n已放入\n%1 ×%2").arg(config->list_cabinet[casePos.cabinetSeqNUM]->list_case[casePos.caseIndex]->name).arg(storeNum), false);
//            config->list_cabinet[casePos.cabinetSeqNUM]->consumableIn(casePos.caseIndex);
        }
    }
    else if(config->state == STATE_FETCH)
    {/*qDebug("fetch");*/
        CaseAddress addr = config->checkCabinetByBarCode(scanInfo);
        if(addr.cabinetSeqNUM == -1)
        {
            qDebug()<<"[fetch]"<<"scan data not find";
            return;
        }
        if(config->list_cabinet[addr.cabinetSeqNUM]->list_case[addr.caseIndex]->list_goods[addr.goodsIndex]->num>0)//物品未取完
        {
            if(!needWaitForServer())
            {
                win_access->scanOpen(scanInfo);
                emit goodsAccess(addr,fullScanInfo, 1, 1);
            }
        }
    }
    else if(config->state == STATE_REFUN)
    {
        win_refund->refundScan(scanInfo,fullScanInfo);
//        CaseAddress addr = config->checkCabinetByBarCode(scanInfo);
//        if(addr.cabinetSeqNUM == -1)
//        {
//            qDebug()<<"[refun]"<<"scan data not find";
//            return;
//        }

//        if(!needWaitForServer())
//        {
//            win_access->scanOpen(scanInfo);
//            emit goodsAccess(addr, fullScanInfo,1, 3);
//        }

    }
    else if(config->state == STATE_LIST)
    {
        win_cab_list_view->recvScanData(qba);
    }
    else if(config->state == STATE_CHECK)
    {
        win_check->checkScan(scanInfo,fullScanInfo);
    }

}

void CabinetWidget::logoClicked()
{//qDebug("logoClicked1");
    return;
    if(config->state == STATE_STORE)
    {
        config->state = STATE_NO;
        waitForCodeScan = false;
        storeNum = 0;
//        config->list_cabinet.at(selectCab)->clearSelectState(selectCase);
        emit winSwitch(INDEX_STANDBY);
    }
    else if(config->state == STATE_FETCH)
    {
        config->state = STATE_NO;
        waitForCodeScan = false;
        storeNum = 0;
//        config->list_cabinet.at(selectCab)->clearSelectState(selectCase);
        emit winSwitch(INDEX_STANDBY);
    }
//    qDebug("logoClicked2");
}

void CabinetWidget::cabinetInit()
{
    panel_init(config->list_cabinet);
}

void CabinetWidget::showEvent(QShowEvent *)
{
    ui->cabId->setText(QString("设备终端NO:%1").arg(config->getCabinetId()));

    if(config->state == STATE_STORE)
    {
        waitForCodeScan = true;
        config->list_cabinet[0]->showMsg(MSG_STORE, false);
    }
    else if(config->state == STATE_FETCH)
    {
        clickLock = false;
        selectCab = -1;
        selectCase = -1;
        config->list_cabinet[0]->showMsg(MSG_FETCH, false);
    }
}

bool CabinetWidget::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    win_access->installGlobalConfig(config);
    win_cab_list_view->installGlobalConfig(config);
    win_check->installGlobalConfig(config);
    win_store_list->installGlobalConfig(config);
    win_refund->installGlobalConfig(config);
    ui->cabId->setText(QString("设备终端NO:%1").arg(config->getCabinetId()));
    return true;
}

void CabinetWidget::caseLock()
{
    clickLock = true;
}

void CabinetWidget::caseUnlock()
{
    clickLock = false;
}

//void CabinetWidget::on_fetch_clicked()
//{
//    waitForCardReader = true;
//    config->state = STATE_FETCH;
//    msgShow("身份验证", "请刷卡验证身份",false);
//    win_access->setAccessModel(false);
//    msgBox = new QMessageBox(QMessageBox::NoIcon, "身份验证", "请刷卡验证身份",QMessageBox::Ok,NULL,
//           Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
//    msgBox->setModal(false);
//    msgBox->show();
//    QTimer::singleShot(10000,this, SLOT(wait_timeout()));
//}

//void CabinetWidget::on_fetch_toggled(bool checked)
//{
//    if(checked)
//    {qDebug("fetch");
//        clickLock = false;
//        config->state = STATE_FETCH;
//        win_access->setAccessModel(false);
//        waitForCodeScan = false;
//    }
//    else
//    {
//        cabLock();
//        initAccessState();
//    }
//}

void CabinetWidget::on_service_clicked(bool checked)
{
    if(checked)
    {
        emit winSwitch(INDEX_CAB_SERVICE);
        clearMenuState();
        ui->service->setChecked(true);
        config->state = STATE_FETCH;
        config->list_cabinet[0]->showMsg(MSG_EMPTY,false);
    }
    else
    {
        cabLock();
    }
}

void CabinetWidget::on_refund_clicked(bool checked)//退货模式
{
    if(checked)
    {
        config->state = STATE_REFUN;
        config->list_cabinet[0]->showMsg(MSG_REFUND,false);
        clearMenuState();
        ui->refund->setChecked(true);
        waitForCodeScan = true;
    }
    else
    {
        cabLock();
    }
}

void CabinetWidget::on_store_clicked(bool checked)
{
    if(checked)
    {
        waitForCardReader = false;
        clickLock = true;
        config->state = STATE_STORE;
        config->list_cabinet[0]->showMsg(MSG_SCAN_LIST, false);
        waitForCodeScan = true;
        waitForGoodsListCode = true;
        win_access->setAccessModel(true);
        clearMenuState();
        ui->store->setChecked(true);
    }
    else
    {
        cabLock();
        initAccessState();
    }
}

void CabinetWidget::on_cut_clicked()
{
    config->state = STATE_FETCH;
    config->list_cabinet[0]->showMsg(MSG_EMPTY,false);
    waitForCodeScan = true;
    win_cab_list_view->show();
}

void CabinetWidget::on_check_clicked(bool checked)
{
    if(checked)
    {
        config->state = STATE_CHECK;
        waitForCodeScan = true;
        config->list_cabinet[0]->showMsg(MSG_CHECK,false);
        clickLock = false;
        clearMenuState();
        ui->check->setChecked(true);
    }
    else
    {
        qDebug("<>>>>>>>>>>>>>>");
        config->clearSearch();//重置单元格状态
        cabLock();
    }
}

void CabinetWidget::pinyinSearch(int id)
{
//    int i = 0;
    qDebug()<<groupBtn.button(id)->text()<<groupBtn.button(id)->isChecked();

//    if(!groupBtn.button(id)->isChecked())
//    {
//        groupBtn.button(id)->setChecked(false);
//        config->clearSearch();
//        return;
//    }
//    else
        config->searchByPinyin(groupBtn.button(id)->text().at(0));

//    for(i=0; i<25; i++)
//    {
//        if(i != id)
//            groupBtn.button(i)->setChecked(false);
//    }


}

void CabinetWidget::updateNetState(bool connected)
{
    ui->netState->setChecked(connected);
}

void CabinetWidget::wait_timeout()
{
    waitForServer = false;
//    if(msgBox == NULL)
//        return;

//    if(!waitForCardReader)
//        return;
//    waitForCardReader = false;
//    msgShow("等待超时", "身份校验超时",false);
//    msgBox->close();
//    msgBox->deleteLater();
//    msgBox = NULL;

//    msgBox = new QMessageBox(QMessageBox::NoIcon, "等待超时", "身份校验超时",QMessageBox::Ok,NULL,
//           Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
//    msgBox->setModal(true);
//    msgBox->exec();
//    msgBox->deleteLater();
    //    msgBox = NULL;
}

void CabinetWidget::saveStore(Goods *goods, int num)
{qDebug("[saveStore]");
    CaseAddress addr = config->checkCabinetByName(goods->name);
//    config->list_cabinet[addr.cabinetSeqNUM]->consumableIn(addr,num);
    emit goodsAccess(addr, goods->packageBarcode, num, 2);
    scanInfo.clear();
    curStoreList->goodsIn(goods->packageBarcode, num);

    if(curStoreList->isFinished())
        config->list_cabinet[0]->showMsg(MSG_EMPTY, false);
    else
        config->list_cabinet[0]->showMsg(MSG_STORE, false);
//    initAccessState();
}

void CabinetWidget::saveFetch(QString name, int num)
{
    if(num<=0)
        return;

    CaseAddress addr = config->checkCabinetByName(name);
//    config->list_cabinet[addr.cabinetSeqNUM]->consumableOut(addr,num);
    clickLock = false;
    emit requireOpenCase(addr.cabinetSeqNUM, addr.caseIndex);
    emit goodsAccess(addr, config->list_cabinet[addr.cabinetSeqNUM]->list_case[addr.caseIndex]->list_goods.at(addr.goodsIndex)->id, num, 1);
//    initAccessState();
}

void CabinetWidget::warningMsgBox(QString title, QString msg)
{
    QMessageBox* box = new QMessageBox(QMessageBox::NoIcon, title, msg,QMessageBox::Ok,NULL,
           Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
    box->setModal(true);
    box->exec();
    box->deleteLater();
}

void CabinetWidget::msgClear()
{
    if(msgBox != NULL)
    {
        msgBox->close();
        msgBox->deleteLater();
        msgBox = NULL;
    }
}

void CabinetWidget::msgShow(QString title, QString msg, bool setmodal)
{
    msgClear();

    if(setmodal)
    {
        msgBox = new QMessageBox(QMessageBox::NoIcon, title, msg,QMessageBox::Ok,NULL,
                                 Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
        msgBox->setModal(true);
        msgBox->exec();
        msgClear();
    }
    else
    {
        msgBox = new QMessageBox(QMessageBox::NoIcon, title, msg,QMessageBox::Ok,NULL,
                                 Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
        msgBox->setModal(false);
        msgBox->show();
    }
}
/*
|补货|退货|服务|退出|
*/
void CabinetWidget::setPowerState(int power)
{
    clickLock = false;
    if(win_store_list->isHidden())
        config->state = STATE_FETCH;
    win_access->setAccessModel(false);
    waitForCodeScan = false;

    ui->store->hide();
    ui->refund->hide();
    ui->service->hide();
    ui->cut->hide();
    ui->check->hide();
    ui->search->show();

    if(ui->netState->isChecked())
    {
        switch(power)
        {
        case 0://超级管理员:|补货|退货|服务|退出|
            ui->store->show();
            ui->refund->show();
            ui->service->show();
            ui->cut->show();
            ui->check->show();
            break;

        case 1://仓库员工:|补货|退货|退出|
            ui->store->show();
            ui->refund->show();
            ui->cut->show();
            ui->check->show();
            break;

        case 2://医院管理:|补货|退货|服务|退出|
            ui->store->show();
            ui->refund->show();
            ui->cut->show();
            //        ui->service->show();
            break;

        case 3://医院员工:|退货|服务|退出|
            ui->refund->show();
            ui->cut->show();
            //        ui->service->show();
            break;
        default:
            break;
        }
    }
    else
    {
        switch(power)
        {
        case 0://超级管理员:|服务|
            ui->service->show();
            ui->cut->show();
            break;

        case 1://仓库员工:
            ui->cut->show();
            break;

        case 2://医院管理:
            ui->cut->show();
            //        ui->service->show();
            break;

        case 3://医院员工:
            ui->cut->show();
            //        ui->service->show();
            break;
        default:
            break;
        }
    }
}

void CabinetWidget::recvUserInfo(QByteArray qba)
{
    if(!waitForCardReader)
    {
        qDebug()<<"[CabinetWidget]"<<"recvUserInfo not need.";
        msgClear();
        return;
    }
//    waitForCardReader = false;
//    optUser = QString(qba);
    if(!needWaitForServer())
    {
        waitForServer = false;
        msgShow("身份验证", "身份验证中...",false);
        emit requireUserCheck(QString(qba));
    }
}

void CabinetWidget::recvListInfo(GoodsList *l)
{
    qDebug("recv");
    if(l->list_goods.count() == 0)
    {
        config->list_cabinet[0]->showMsg(MSG_LIST_ERROR, false);
        return;
    }

    if(curStoreList != NULL)
        delete curStoreList;

    curStoreList = l;

    win_store_list->setLoginState(loginState);
    win_store_list->storeStart(l);
    win_store_list->show();
    return;

    win_access->setStoreList(l);
    config->list_cabinet[0]->showMsg(MSG_STORE, false);
    waitForCodeScan = true;
    waitForGoodsListCode = false;
}

void CabinetWidget::recvBindRst(bool rst)
{
    if(rst)
    {
        CaseAddress addr;
        addr.cabinetSeqNUM = selectCab;
        addr.caseIndex = selectCase;
        win_store_list->show();
        win_store_list->bindRst(addr);
//        win_access->clickOpen(curGoods->packageBarcode);
//        emit requireOpenCase(selectCab, selectCase);
    }
    else
    {
        clickLock = false;
//        win_access->save();
//        win_access->hide();
        config->list_cabinet[0]->showMsg(MSG_STORE_SELECT, false);
    }
}

void CabinetWidget::recvGoodsCheckRst(QString msg)
{
    if(msg.isEmpty())
        win_check->checkRst("盘点成功");
    else
        win_check->checkRst(msg);
}

void CabinetWidget::recvGoodsNumInfo(QString goodsId, int num)
{
    CaseAddress addr = config->checkCabinetByBarCode(goodsId);
    waitForServer = false;
    qDebug()<<goodsId<<num<<config->state<<addr.cabinetSeqNUM;
    if(addr.cabinetSeqNUM == -1)
        return;
    else
    {
        config->list_cabinet[addr.cabinetSeqNUM]->updateGoodsNum(addr, num);
        if(config->state == STATE_LIST)
            win_cab_list_view->fetchSuccess();
        else if(config->state == STATE_STORE)
            win_store_list->storeRst("存入成功",true);
        else if(config->state == STATE_REFUN)
            win_refund->refundRst("退货成功");
        else
        {
            emit goodsNumChanged(num);
        }
    }
}

void CabinetWidget::accessFailedMsg(QString msg)
{
    waitForServer = false;
    if(config->state == STATE_LIST)
    {
        win_cab_list_view->fetchFailed(msg);
    }
    else if(config->state == STATE_STORE)
    {
//        win_access->storeFailed(msg);
        win_store_list->storeRst(msg, false);
    }
    else if(config->state == STATE_FETCH)
    {
        win_access->fetchFailed(msg);
    }
    else if(config->state == STATE_REFUN)
    {
        win_refund->refundRst(msg);
    }
    qDebug()<<msg;
}

void CabinetWidget::updateTime()
{
    showCurrentTime(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
}

void CabinetWidget::updateId()
{
    ui->cabId->setText(QString("设备终端NO:%1").arg(config->getCabinetId()));
}

void CabinetWidget::newGoodsList(QString, QString)
{

}

void CabinetWidget::readyGoodsList(QString listCode)
{
    emit requireGoodsListCheck(listCode);
    config->state = STATE_STORE;
}

void CabinetWidget::cabinetBind(Goods *goods)
{
    curGoods = goods;
    clickLock = false;
    win_store_list->hide();
    config->list_cabinet[0]->showMsg(MSG_STORE_SELECT, false);
}

void CabinetWidget::checkOneCase(QList<CabinetCheckItem *> l, CaseAddress addr)
{
    int i=0;

    for(i=0; i<l.count(); i++)
    {
        addr.goodsIndex = i;
        config->list_cabinet[addr.cabinetSeqNUM]->updateGoodsNum(addr, l[i]->itemNum());
    }
    emit checkCase(l, addr);
}

void CabinetWidget::recvUserCheckRst(UserInfo* info)
{
    waitForServer = false;
    msgClear();
    optUser = info;
    qDebug()<<"[recvUserCheckRst]"<<optUser->cardId;
    ui->userInfo->setText(QString("您好！%1").arg(optUser->name));
    setPowerState(info->power);
    loginState = true;
    win_store_list->setLoginState(loginState);

    config->cabVoice.voicePlay(VOICE_WELCOME_USE);
}

void CabinetWidget::on_search_clicked()
{
    config->state = STATE_FETCH;
    config->list_cabinet[0]->showMsg(MSG_EMPTY,false);
    ui->menuWidget->setCurrentIndex(1);
}

void CabinetWidget::on_search_back_clicked()
{
    ui->menuWidget->setCurrentIndex(0);
}
