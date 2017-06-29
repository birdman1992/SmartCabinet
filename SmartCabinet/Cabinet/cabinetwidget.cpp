#include "cabinetwidget.h"
#include "ui_cabinetwidget.h"
#include <QDebug>
#include "defines.h"
#include "Device/controldevice.h"

//提示信息
#define MSG_EMPTY ""
#define MSG_SCAN_LIST "请扫描送货单条码"
#define MSG_LIST_ERROR "无效的送货单"
#define MSG_STORE "请扫描待存放物品条形码"
#define MSG_STORE_SELECT "请选择存放位置"
#define MSG_STORE_SELECT_REPEAT "选择的位置被占用 请重新选择"
#define MSG_FETCH "请选择要取出的物品 柜门打开后请扫描条形码取出"
#define MSG_FETCH_SCAN "请扫描条形码取出物品 取用完毕请点击此处并关闭柜门"
#define MSG_FETCH_EMPTY "没有库存了 请关好柜门 点击此处退出"

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
    curStoreList = NULL;
    msgBox = NULL;
    selectCase = -1;
    selectCab = -1;
    win_access = new CabinetAccess();
    initSearchBtns();
    connect(win_access, SIGNAL(saveStore(Goods*,int)), this, SLOT(saveStore(Goods*,int)));
    connect(win_access, SIGNAL(saveFetch(QString,int)), this, SLOT(saveFetch(QString,int)));
    connect(this, SIGNAL(goodsNumChanged(int)), win_access, SLOT(recvOptGoodsNum(int)));
//    optUser = QString();
    ui->store->hide();
    ui->refund->hide();
    ui->service->hide();
    ui->quit->hide();

#ifndef SIMULATE_ON
    ui->msk1->hide();
    ui->msk2->hide();
#endif
}

CabinetWidget::~CabinetWidget()
{
    delete win_access;
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
    optUser = UserInfo();
    ui->userInfo->setText("请刷卡使用");
    storeNum = 0;
    clickLock = true;
    waitForCodeScan = false;
    waitForGoodsListCode = false;
    waitForCardReader = true;
    curStoreList = NULL;
    msgBox = NULL;
    config->list_cabinet[0]->showMsg(MSG_EMPTY,false);
    ui->store->hide();
    ui->service->hide();
    ui->quit->hide();
    ui->refund->hide();
    curStoreList = NULL;
    config->state = STATE_FETCH;
}

void CabinetWidget::cabInfoBind(int seq, int index, GoodsInfo info)
{
    qDebug()<<"bind"<<info.id;
    config->list_cabinet[seq]->setCaseName(info, index);
    emit requireCaseBind(seq, index, info.id);
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
    groupBtn.setExclusive(false);

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

QByteArray CabinetWidget::scanDataTrans(QByteArray code)
{
    int index = code.indexOf("-");
    if(index == -1)
        return code;

    return code.right(code.size()-index-1);
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
}

void CabinetWidget::caseClicked(int caseIndex, int cabSeqNum)
{
//    qDebug()<<config->getCabinetId();
//    emit requireOpenCase(cabSeqNum, caseIndex);
    if(clickLock)//锁定状态下点击无效
    {
        if((caseIndex==selectCase) && (cabSeqNum == selectCab))
            return;
        else
            config->list_cabinet.at(cabSeqNum)->clearSelectState(caseIndex);
        return;
    }

    bool clickRepeat = false;
    if((caseIndex==selectCase) && (cabSeqNum == selectCab))
        clickRepeat = true;//标记为重复点击
//    if(!config->list_cabinet[cabSeqNum]->list_case[caseIndex]->name.isEmpty())
//        clickRepeat = true;

    clickLock = true;
    selectCab = cabSeqNum;
    selectCase = caseIndex;
    qDebug()<<caseIndex<<cabSeqNum;

    if(config->state == STATE_STORE)
    {
//        if(clickRepeat)
//        {
//            config->list_cabinet[0]->showMsg(MSG_STORE_SELECT_REPEAT, false);
//            clickLock = false;
//            return;
//        }
        GoodsInfo info;
        info.name = curGoods->name;
        info.id = curGoods->goodsId;
        info.packageId = curGoods->packageBarcode;
        info.unit = curGoods->unit;
        info.num = 0;
        info.Py = config->getPyCh(info.name);
        qDebug()<<"[pinyin]"<<info.Py;
        cabInfoBind(selectCab, selectCase, info);
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
        emit requireOpenCase(cabSeqNum, caseIndex);

        waitForCodeScan = true;
        clickLock = true;
        scanInfo = QString();
        CabinetInfo* info = config->list_cabinet[cabSeqNum]->list_case[caseIndex];
        win_access->setAccessModel(false);
        win_access->clickOpen(info);
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
    QByteArray code = scanDataTrans(qba);//截取去掉唯一码
    qDebug()<<code;
    if(scanInfo != QString(code))
    {
        newStore = true;
    }
    scanInfo = QString(code);
    fullScanInfo = QString(qba);

    if(config->state == STATE_STORE)
    {
        curGoods = curStoreList->getGoodsById(scanInfo);
        if(curGoods == NULL)
        {
            qDebug()<<"[recvScanData]"<<"scan goods id not find";
            return;
        }
        else
            qDebug()<<"[recvScanData]"<<"scan goods id find";
        //根据物品名搜索柜格位置
        casePos = config->checkCabinetByName(curGoods->name);

        if(casePos.cabinetSeqNUM == -1)//没有搜索到药品对应的柜格
        {
            clickLock = false;
            win_access->save();
            win_access->hide();
            config->list_cabinet[0]->showMsg(MSG_STORE_SELECT, false);
        }
        else
        {
            //打开对应柜门
            qDebug()<<"[CabinetWidget]"<<"[open]"<<casePos.cabinetSeqNUM<<casePos.caseIndex;
            if(newStore)
                emit requireOpenCase(casePos.cabinetSeqNUM, casePos.caseIndex);


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
        CaseAddress addr = config->checkCabinetByBarCode(scanInfo);
        if(addr.cabinetSeqNUM == -1)
        {
            qDebug()<<"[refun]"<<"scan data not find";
            return;
        }
        if(config->list_cabinet[addr.cabinetSeqNUM]->list_case[addr.caseIndex]->list_goods[addr.goodsIndex]->num>0)//物品未取完
        {
            if(!needWaitForServer())
            {
                win_access->scanOpen(scanInfo);
                emit goodsAccess(addr, fullScanInfo,1, 3);
            }
        }
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

void CabinetWidget::on_store_clicked()
{
//    waitForCardReader = false;
//    config->state = STATE_STORE;
//    config->list_cabinet[0]->showMsg(MSG_SCAN_LIST, false);
//    waitForCodeScan = true;
//    waitForGoodsListCode = true;
//    win_access->setAccessModel(true);

//    if(msgBox != NULL)
//    {
//        msgBox->close();
//        msgBox->deleteLater();
//        msgBox = NULL;
//    }
//    msgBox = new QMessageBox(QMessageBox::NoIcon, "身份验证", "请刷卡验证身份",QMessageBox::Ok,NULL,
//           Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
//    msgBox->setModal(false);
//    msgBox->show();
//    msgShow("身份验证", "请刷卡验证身份",false);
//    QTimer::singleShot(10000,this, SLOT(wait_timeout()));
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

void CabinetWidget::on_service_toggled(bool checked)
{
    if(checked)
    {
        emit winSwitch(INDEX_CAB_SERVICE);
    }
    else
    {
        cabLock();
    }
}

void CabinetWidget::on_refund_toggled(bool checked)//退货模式
{
    if(checked)
    {
        config->state = STATE_REFUN;
    }
    else
    {
        cabLock();
    }
}

void CabinetWidget::on_store_toggled(bool checked)
{
    if(checked)
    {
        waitForCardReader = false;
        config->state = STATE_STORE;
        config->list_cabinet[0]->showMsg(MSG_SCAN_LIST, false);
        waitForCodeScan = true;
        waitForGoodsListCode = true;
        win_access->setAccessModel(true);
    }
    else
    {
        cabLock();
        initAccessState();
    }
}

void CabinetWidget::pinyinSearch(int id)
{
    int i = 0;
    qDebug()<<groupBtn.button(id)->text()<<groupBtn.button(id)->isChecked();

    if(!groupBtn.button(id)->isChecked())
    {
        groupBtn.button(id)->setChecked(false);
        config->clearSearch();
        return;
    }

    for(i=0; i<25; i++)
    {
        if(i != id)
            groupBtn.button(i)->setChecked(false);
    }
    config->searchByPinyin(groupBtn.button(id)->text().at(0));
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
    config->state = STATE_FETCH;
    win_access->setAccessModel(false);
    waitForCodeScan = false;

    ui->store->hide();
    ui->refund->hide();
    ui->service->hide();
    ui->quit->hide();

    switch(power)
    {
    case 0://超级管理员:|补货|退货|服务|退出|
        ui->store->show();
        ui->refund->show();
//        ui->service->show();
        break;

    case 1://仓库员工:|补货|退货|退出|
        ui->store->show();
        ui->refund->show();break;

    case 2://医院管理:|补货|退货|服务|退出|
        ui->store->show();
        ui->refund->show();
//        ui->service->show();
        break;

    case 3://医院员工:|退货|服务|退出|
        ui->refund->show();
//        ui->service->show();
        break;
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
        waitForServer = true;
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
    win_access->setStoreList(l);
    config->list_cabinet[0]->showMsg(MSG_STORE, false);
    waitForCodeScan = true;
    waitForGoodsListCode = false;
}

void CabinetWidget::recvBindRst(bool rst)
{
    if(rst)
    {
        win_access->clickOpen(curGoods->packageBarcode);
        emit requireOpenCase(selectCab, selectCase);
    }
    else
    {
        clickLock = false;
        win_access->save();
        win_access->hide();
        config->list_cabinet[0]->showMsg(MSG_STORE_SELECT, false);
    }
}

void CabinetWidget::recvGoodsNumInfo(QString goodsId, int num)
{
    CaseAddress addr = config->checkCabinetByGoodsId(goodsId);
    waitForServer = false;
    if(addr.cabinetSeqNUM == -1)
        return;
    else
    {
        config->list_cabinet[addr.cabinetSeqNUM]->updateGoodsNum(addr, num);
        emit goodsNumChanged(num);
    }
}

void CabinetWidget::recvUserCheckRst(UserInfo info)
{
    waitForServer = false;
    msgClear();
    optUser = info;
    qDebug()<<optUser.cardId;
    ui->userInfo->setText(QString("您好！%1").arg(optUser.name));
    setPowerState(info.power);
    config->cabVoice.voicePlay(VOICE_WELCOME_USE);
}


