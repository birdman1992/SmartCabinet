#include "cabinetwidget.h"
#include "ui_cabinetwidget.h"
#include <QDebug>
#include <QDateTime>
#include <QWidget>
#include "defines.h"
#include "Device/controldevice.h"
#include "funcs/systool.h"
#include "defines.h"
#include "iconfont/iconhelper.h"
#include "MessageDialog.h"
#include "Widgets/msgtips.h"

//提示信息

CabinetWidget::CabinetWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetWidget)
{
    ui->setupUi(this);
    storeNum = 0;
    tsCalFlag = 0;
    curCard = QString();
    clickLock = true;
    waitForCodeScan = false;
    waitForGoodsListCode = false;
    waitForCardReader = true;
    waitForSecondaryCard = false;
    waitForCheckFinish = false;
    waitForInit = true;
    loginState = false;
    rebindGoods = QString();
    curStoreList = NULL;
    msgBox = NULL;
    selectCase = -1;
    selectCab = -1;
    win_access = new CabinetAccess();
    win_cab_list_view = new CabinetListView();
    win_check = new CabinetCheck();
    win_store_list = new CabinetStoreList();
    win_refund = new CabinetRefund();
    win_net_set = new NetworkSet();
    win_check_warnning = new CheckWarning();
    sqlManager = SqlManager::manager();
    sqlManager->selectAllGoods();
    lastOptTime = QTime::currentTime();
    screenProState = false;

    SignalManager* sigMan = SignalManager::manager();
    connect(this, SIGNAL(loginStateChanged(bool)), sigMan, SIGNAL(updateLoginState(bool)));

    connect(win_check_warnning, SIGNAL(pushCheck()), this, SLOT(checkPush()));
#ifdef TCP_API
    goodsManager = GoodsManager::manager();
#endif
    timeUpdater = new QTimer(this);
    connect(timeUpdater, SIGNAL(timeout()), this, SLOT(updateTime()));
    timeUpdater->start(1000);

    initVolum();
    initSearchBtns();
//    connect(win_access, SIGNAL(saveStore(Goods*,int)), this, SLOT(saveStore(Goods*,int)));
//    connect(win_access, SIGNAL(saveFetch(QString,int)), this, SLOT(saveFetch(QString,int)));
    connect(this, SIGNAL(goodsNumChanged(int)), win_access, SLOT(recvOptGoodsNum(int)));

    connect(win_cab_list_view, SIGNAL(requireAccessList(QStringList,int)), this, SIGNAL(requireAccessList(QStringList,int)));
    connect(win_cab_list_view, SIGNAL(requireOpenCase(int,int)), this, SIGNAL(requireOpenCase(int,int)));
    connect(win_cab_list_view, SIGNAL(searchGoods(QString)), this, SLOT(searchByPinyin(QString)));

    connect(win_check, SIGNAL(checkCase(QList<CabinetCheckItem*>,QPoint)), this, SLOT(checkOneCase(QList<CabinetCheckItem*>,QPoint)));

    connect(win_store_list, SIGNAL(requireBind(Goods*)), this, SLOT(cabinetBind(Goods*)));
    connect(win_store_list, SIGNAL(requireOpenCase(int,int)), this, SIGNAL(requireOpenCase(int,int)));
    connect(win_store_list, SIGNAL(requireGoodsListCheck(QString)), this, SIGNAL(requireGoodsListCheck(QString)));
    connect(win_store_list, SIGNAL(storeList(QString ,QList<CabinetStoreListItem*>)), this, SIGNAL(storeList(QString ,QList<CabinetStoreListItem*>)));
    connect(win_store_list, SIGNAL(newStoreBarCode(QString)), this, SIGNAL(newStoreBarCode(QString)));
    connect(win_store_list, SIGNAL(requireScanState(bool)), this, SLOT(updateScanState(bool)));
    connect(win_store_list, SIGNAL(reportTraceId(QString)), this, SIGNAL(reportTraceId(QString)));

    connect(win_refund, SIGNAL(refundCase(QStringList,int)), this, SIGNAL(requireAccessList(QStringList,int)));
//    optUser = QString();

    showMap.insert(ui->store, false);
    showMap.insert(ui->refund, false);
    showMap.insert(ui->service, false);
    showMap.insert(ui->rebind, false);
    showMap.insert(ui->cut, false);
    showMap.insert(ui->check, false);
    showMap.insert(ui->search, false);
    showMap.insert(ui->reply, false);
    showMap.insert(ui->quit, false);
    showMap.insert(ui->back, false);
    showMap.insert(ui->btn_check_table, false);
    showMap.insert(ui->consume_date, false);
    updateShowMap();

    ui->menuWidget->setCurrentIndex(0);
//    IconHelper::Instance()->SetIcon(ui->back, QChar(0xf18e)+QString("  还货"), 42);
//#ifndef SIMULATE_ON
//    ui->msk1->hide();
//    ui->msk2->hide();
//#endif
}

CabinetWidget::~CabinetWidget()
{
    delete win_access;
    delete win_refund;
    delete win_cab_list_view;
    delete win_check;
    delete win_store_list;
    delete win_check_warnning;
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
    emit updateLoginState(false);
    tsCalFlag = 0;
    optUser = NULL;
    ui->userInfo->setText("     请刷卡使用");
    storeNum = 0;
    loginState = false;
    win_store_list->setLoginState(loginState);
    emit loginStateChanged(loginState);
    clickLock = true;
    waitForCodeScan = false;
    waitForGoodsListCode = false;
    waitForCardReader = true;
    curStoreList = NULL;
    msgBox = NULL;
    config->showMsg(MSG_EMPTY,false);
    config->clearOptId();
    showMap[ui->store] = false;
    showMap[ui->service] = false;
    showMap[ui->rebind] = false;
    showMap[ui->cut] = false;
    showMap[ui->refund] = false;
    showMap[ui->back] = false;
    showMap[ui->check] = false;
    showMap[ui->search] = false;
    showMap[ui->quit] = false;
    showMap[ui->consume_date] = false;
    showMap[ui->reply] = false;
    showMap[ui->btn_check_table] = false;
    updateShowMap();
    win_access->hide();
    curStoreList = NULL;
    config->state = STATE_NO;
    clearCaseState();
    config->wakeUp(0);
//    emit checkLockState();
    if(config->getCabinetType().at(BIT_CAB_AIO))
        emit stack_switch(INDEX_AIO);
}

void CabinetWidget::cabInit()
{
    ui->store->setChecked(false);
    ui->service->setChecked(false);
    ui->rebind->setChecked(false);
    ui->cut->setChecked(false);
    ui->refund->setChecked(false);
    ui->check->setChecked(false);
    ui->search->setChecked(false);
    ui->back->setChecked(false);
}

void CabinetWidget::cabInfoBind(int seq, int index, QString info)
{
    QPoint addr = SqlManager::searchByPackageId(info);
    SqlManager::bindGoodsId(seq, index, info);
    updateCase(addr);//更新旧位置
    updateCase(seq, index);//刷新新位置
//    qDebug()<<"bind"<<info.id<<info.abbName;
//    info.goodsType = config->getGoodsType(info.packageId);
//    qDebug()<<info.goodsType;
//    config->list_cabinet[seq]->setCaseName(info, index);
//    emit requireCaseBind(seq, index, info.packageId);
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

void CabinetWidget::initVolum()
{
    volume = new QSlider();
    volume->installEventFilter(this);
    QFile sliderStyle(":/stylesheet/styleSheet/SliderBar.qss");
    sliderStyle.open(QIODevice::ReadOnly);
    QString style = sliderStyle.readAll();
    volume->setStyleSheet(style);
    volume->setWindowFlags(Qt::FramelessWindowHint);
    volume->resize(48, 240);
    volume->setMaximum(100);
    volume->setValue(config->getSysVolem());
    config->setSysVolem(config->getSysVolem());

    connect(volume, SIGNAL(sliderPressed()), this, SLOT(vol_pressed()));
    connect(volume, SIGNAL(sliderReleased()), this, SLOT(vol_released()));
    connect(volume, SIGNAL(valueChanged(int)), this, SLOT(vol_changed(int)));
}

void CabinetWidget::showCurrentTime(QString curTime)
{
    ui->time->setText(curTime);
}

void CabinetWidget::rebindRecover()
{
    qDebug("rebindRecover");
//    qDebug()<<rebind_new_addr.goodsIndex;
//    if((rebindGoods == NULL) || (rebind_new_addr.cabinetSeqNum == -1) || (rebind_old_addr.cabinetSeqNum == -1))
//        return;
//    qDebug("rebindRecover2");
//    config->list_cabinet[rebind_new_addr.cabinetSeqNum]->list_case[rebind_new_addr.caseIndex]->list_goods.removeAt(rebind_new_addr.goodsIndex);
//    config->removeConfig(rebind_new_addr);
//    config->list_cabinet[rebind_old_addr.cabinetSeqNum]->setCaseName(*rebindGoods, rebind_old_addr.caseIndex);

//    rebindOver();
    rebindGoods = QString();
    rebind_new_addr.clear();
    rebind_old_addr.clear();
    config->showMsg(MSG_REBIND_SCAN,0);
}

void CabinetWidget::rebindOver()
{
    qDebug("rebindOver");
//    config->removeConfig(rebind_old_addr);
//    config->setConfig(rebind_new_addr, rebindGoods);

//    rebindGoods = NULL;
//    rebind_new_addr.clear();
//    rebind_old_addr.clear();
    rebindGoods = QString();
    config->showMsg(MSG_REBIND_SCAN,0);
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
    ui->rebind->setChecked(false);
    ui->back->setChecked(false);
    config->showMsg(MSG_EMPTY,false);
}

void CabinetWidget::volumTest()
{
    config->cabVoice.voicePlay("vol.wav");
}

void CabinetWidget::checkStart()
{
#ifndef TCP_API
    config->state = STATE_CHECK;
    waitForCodeScan = true;
    waitForGoodsListCode = false;
    clearMenuState();
    config->showMsg(MSG_CHECK,false);
    clickLock = false;
    ui->check->setChecked(true);
    config->wakeUp(TIMEOUT_CHECK);

    showMap[ui->store] = false;
    showMap[ui->service] = false;
    showMap[ui->rebind] = false;
    showMap[ui->cut] = false;
    showMap[ui->refund] = false;
//    ui->check] = false;
    showMap[ui->search] = false;
    showMap[ui->reply] = false;
    showMap[ui->quit] = false;
    updateShowMap();

    if(config->getCabinetType().at(BIT_CAB_AIO))
    {
        switchCabinetState(STATE_CHECK);
    }
#else
    config->state = STATE_CHECK;
    waitForCodeScan = true;
    waitForGoodsListCode = false;
    config->showMsg(MSG_CHECK,false);
    clickLock = false;
    clearMenuState();
    ui->check->setChecked(true);
    config->wakeUp(TIMEOUT_CHECK);

//    ui->store->hide();
//    ui->service->hide();
//    ui->cut->hide();
//    ui->refund->hide();
////    ui->check->hide();
//    ui->search->hide();
//    ui->reply->hide();
//    ui->quit->hide();
#endif
}

void CabinetWidget::checkCreat()
{
    config->showMsg(MSG_CHECK_CREAT,false);
    emit requireGoodsCheck();
}

void CabinetWidget::calCheck(QString card)
{
    if(card != curCard)
        return;

    tsCalFlag++;
    qDebug()<<"[tsCalFlag]"<<tsCalFlag;
    if(tsCalFlag>5)
    {
        tsCalFlag = 0;
        emit tsCalReq();
    }
}

void CabinetWidget::magicCmd(QString cmd)
{
    if(cmd == QString(MAGIC_CAL))
        emit tsCalReq();
    if(cmd == QString(MAGIC_SHOT))
        SysTool::singleShot();
}

bool posSort(Cabinet *A, Cabinet *B)
{
    return A->getCabPos() < B->getCabPos();
}

QByteArray CabinetWidget::scanDataTrans(QByteArray code)
{
//    int index = code.indexOf("-");
//    QByteArray ret = code;
//    if(index == -1)
//        return ret;

//    code = code.right(code.size()-index-1);

//    index = code.lastIndexOf("-");
//    if(index == -1)
//        return ret;

//    return code.left(index);
    QString strFull = QString(code);
    QStringList strList = strFull.split("-", QString::SkipEmptyParts);
    if(strList.count() < 4)
        return QByteArray();

    strList.removeLast();
    strList = strList.mid(strList.count()-2, 2);
    QByteArray ret = strList.join("-").toLocal8Bit();
    return ret;
}

//初始化药柜界面
void CabinetWidget::panel_init(QList<Cabinet *> cabinets)
{
    if(!waitForInit)
        return;
    qDebug("[panel_init]");
    waitForInit = false;
    int index = cabinets.count()-1;
    int i=0;
    qDebug()<<"index"<<index;
    qSort(cabinets.begin(),cabinets.end(),posSort);
    list_cabinet = cabinets;

    ui->cabinet_layout->addStretch();
    for(i=0; i<cabinets.count(); i++)
    {
        ui->cabinet_layout->addWidget(cabinets.at(i));
        connect(cabinets.at(i), SIGNAL(caseSelect(int,int)), this, SLOT(caseClicked(int,int)));
//        connect(cabinets.at(i), SIGNAL(caseEntered(QPoint)), this, SLOT(caseEntered(QPoint)));
//        connect(cabinets.at(i), SIGNAL(caseLeaved()), this, SLOT(caseLeaved()));

//        if(cabinets.at(i)->getScreenPos() > 0)//点击logo触发动作，暂无用，屏蔽
//        {
//            connect(cabinets.at(i), SIGNAL(logoClicked()), this, SLOT(logoClicked()));
//        }
    }
    ui->cabinet_layout->addStretch();
    win_cab_list_view->setCabView(ui->frame_6);
}

bool CabinetWidget::caseHasPos(int seq, int index)
{
    if(seq<0 || seq>=list_cabinet.count())
        return false;

    if(index<0 || index>=list_cabinet[seq]->rowCount())
        return false;

    return list_cabinet[seq]->haveEmptyPos(index);
}

void CabinetWidget::checkCabinetCase(int seq, int index)
{
    if(seq<0 || seq>=list_cabinet.count())
        return;

    if(index<0 || index>=list_cabinet[seq]->rowCount())
        return;

    list_cabinet[seq]->checkCase(index);
    list_state_case<<QPoint(seq, index);
}

void CabinetWidget::searchCabinetCase(int seq, int index)
{
    if(seq<0 || seq>=list_cabinet.count())
        return;

    if(index<0 || index>=list_cabinet[seq]->rowCount())
        return;

    list_cabinet[seq]->searchCase(index);
    list_state_case<<QPoint(seq, index);
}

void CabinetWidget::caseEntered(QPoint pos)
{
    QStringList caseStrList = SqlManager::getCaseDetailText(pos.x(), pos.y());
    qDebug()<<"caseEntered"<<caseStrList;
    MsgTips::instance().showText(caseStrList.join("\n"));
}

void CabinetWidget::caseLeaved()
{
    MsgTips::instance().hide();
    qDebug()<<"caseLeaved";
}

void CabinetWidget::caseClicked(int caseIndex, int cabSeqNum)
{
    qDebug()<<caseIndex<<cabSeqNum;
    qDebug()<<clickLock;
//    qDebug()<<config->getCabinetId();
//    emit requireOpenCase(cabSeqNum, caseIndex);
    if(config->isScreen(cabSeqNum, caseIndex))
        return;
    if(clickLock && (config->state != STATE_REBIND))//锁定状态下点击无效
    {
        if((caseIndex==selectCase) && (cabSeqNum == selectCab))
            return;
        else
            config->list_cabinet.at(cabSeqNum)->clearSelectState(caseIndex);
        return;
    }
    if(config->isSpec(cabSeqNum, caseIndex))
    {
        if(optUser == NULL)
            return;
        if((optUser->power > 1) && (optUser->power!=3))//0 1 3
            return;
        if(config->state == STATE_STORE)//禁止绑定至护士长柜格
            return;
    }
    if(config->state != STATE_CHECK)
        config->wakeUp(TIMEOUT_BASE);
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
//            config->showMsg(MSG_STORE_SELECT_REPEAT, false);
//            clickLock = false;
//            return;
//        }
        if(curGoods == NULL)
            return;

        if(!config->getCabinetType().at(BIT_CAB_AIO))//非一体机模式
        {
            if(!caseHasPos(cabSeqNum, caseIndex))//柜格没有空间了
            {
                caseUnlock();
                config->showMsg(MSG_FULL, 1);
                return;
            }
        }

        bindCab = selectCab;
        bindCase = selectCase;
        cabInfoBind(selectCab, selectCase, curGoods->packageId);
        emit requireCaseBind(selectCab, selectCase, curGoods->packageId);
        config->showMsg(MSG_EMPTY,0);
    }
    else if((config->state == STATE_FETCH) || (config->state == STATE_BACK))
    {
//        if(!clickRepeat)//如果该柜格没有存放药品
//        {qDebug()<<"empty";
//            selectCab = -1;
//            selectCase = -1;
//            clickLock = false;
//            return;
//        }
        //打开对应柜门
//        if(!ui->netState->isChecked())//离线状态只能批量取货
//        {
//            config->showMsg(MSG_OFFLINE,0);
//            return;
//        }

        qDebug()<<"[CabinetWidget]"<<"[open]"<<cabSeqNum<<caseIndex;
        emit requireOpenCase(cabSeqNum, caseIndex);

        waitForCodeScan = true;
        clickLock = false;
        scanInfo = QString();
//        CabinetInfo* info = config->list_cabinet[cabSeqNum]->list_case[caseIndex];
        win_access->setNetworkState(ui->netState->isChecked());
        win_access->setAccessModel(false);
        win_access->clickOpen();
//        config->showMsg(MSG_FETCH_SCAN, false);
    }
    else if(config->state == STATE_REFUN)
    {
        casePos = QPoint(cabSeqNum, caseIndex);
        win_refund->refundStart(casePos);
        clickLock = false;
        emit requireOpenCase(cabSeqNum, caseIndex);

//        waitForCodeScan = true;
//        clickLock = true;
//        scanInfo = QString();
//        CabinetInfo* info = config->list_cabinet[cabSeqNum]->list_case[caseIndex];
//        win_access->setAccessModel(false);
//        win_access->clickOpen(info);
    }
    else if(config->state == STATE_CHECK)
    {
        emit requireOpenCase(cabSeqNum, caseIndex);
        casePos = QPoint(cabSeqNum, caseIndex);
        win_check->checkStart(casePos);
//        setCheckState(QPoint(cabSeqNum, caseIndex));
        checkCabinetCase(cabSeqNum, caseIndex);
        qDebug()<<"[check]"<<caseIndex;
        clickLock = false;
    }
    else if(config->state == STATE_REBIND)
    {
        emit requireOpenCase(cabSeqNum, caseIndex);

//        if(!(config->list_cabinet[cabSeqNum]->haveEmptyPos(caseIndex)))
//        {
//            caseUnlock();
//            config->showMsg(MSG_FULL, 1);
//            return;
//        }
        if(rebindGoods.isEmpty())
        {
            waitForCodeScan = true;
            return;
        }
//        if(rebind_new_addr.cabinetSeqNum>=0)
//            return;
//        rebind_new_addr.cabinetSeqNum = selectCab;
//        rebind_new_addr.caseIndex = selectCase;
//        rebind_new_addr.goodsIndex = config->list_cabinet[selectCab]->list_case[selectCase]->list_goods.count();

//        config->removeConfig(rebind_old_addr);
        bindCab = selectCab;
        bindCase = selectCase;
        qDebug()<<"click bind"<<bindCab<<bindCase;
        emit requireCaseRebind(selectCab, selectCase, rebindGoods);
        cabInfoBind(selectCab, selectCase, rebindGoods);
    }
    else if(config->state == STATE_SPEC)
    {
        QPoint pSpec = QPoint(cabSeqNum, caseIndex);
        config->setSpecialCase(pSpec);
        emit winSwitch(INDEX_CAB_SERVICE);
        config->state = STATE_FETCH;
    }
}

bool CabinetWidget::isListCode(QByteArray qba)
{
    if((qba.indexOf("CK") == 0) && (qba.indexOf("-") == -1))//存货单
    {
        return true;
    }
    return false;
}

void CabinetWidget::setSearchState(QList<QPoint> l)
{
//    qDebug()<<"before"<<list_state_case;
    foreach (QPoint pos, list_state_case)
    {
        if(pos.x()<list_cabinet.count())
            list_cabinet[pos.x()]->initCase(pos.y());
    }

    list_state_case = l;
//    qDebug()<<"after"<<list_state_case;
    foreach (QPoint pos, list_state_case)
    {
        if(pos.x()<list_cabinet.count())
            list_cabinet[pos.x()]->searchCase(pos.y());
    }
}

void CabinetWidget::clearCaseState()
{
    foreach (QPoint pos, list_state_case)
    {
        if((pos.x()>=0) && (pos.x()<list_cabinet.count()))
            list_cabinet[pos.x()]->initCase(pos.y());
    }
    list_state_case.clear();
}

void CabinetWidget::recvScanData(QByteArray qba)
{qDebug()<<"recvScanData"<<qba<<qba.toHex()<<config->state;
    magicCmd(QString(qba));

    if(config->getCabinetType().at(BIT_CAB_AIO))//一体机模式，无需通过按钮进入存取货模式
    {
        return;
    }

    if(!waitForCodeScan)
    {
        qDebug()<<"[CabinetWidget]"<<"scan data not need";
        return;
    }

    qDebug()<<waitForGoodsListCode<<"need scan all:"<<config->getStoreMode();
    if(waitForGoodsListCode && (!config->getStoreMode()))//不用扫描全部物品的模式,扫描全部物品的模式下由存货窗口接管此信号的发射
    {
        qDebug()<<"requireGoodsListCheck";
        emit requireGoodsListCheck(QString(qba));
        return;
    }
    config->wakeUp(TIMEOUT_FETCH);

    bool newStore;
    Q_UNUSED(newStore);
//    QByteArray code = scanDataTrans(qba);//截取去掉唯一码,xxx-xxxxxxx-xx-xxxx  ->  xxxxxxx-xx

//    if(scanInfo != QString(code))
//    {
//        newStore = true;
//    }
    fullScanInfo = QString(qba);
    scanInfo = SqlManager::getPackageId(fullScanInfo);

#ifdef TCP_API
    scanGoodsId = goodsManager->getGoodsByCode(fullScanInfo);
#else
    scanGoodsId = scanInfo;
#endif
    qDebug()<<scanInfo<<fullScanInfo<<config->state;

    if(config->state == STATE_STORE)
    {
        if(scanInfo.isEmpty())
            scanInfo = fullScanInfo;
        win_store_list->recvScanCode(fullScanInfo);
//        win_store_list->recvStoreTraceRst(true, "存入成功", fullScanInfo);//test
//        emit reportTraceId(fullScanInfo);
    }
    else if(config->state == STATE_FETCH)
    {/*qDebug("fetch");*/
        QPoint addr = SqlManager::searchByCode(fullScanInfo);
        Goods* info = SqlManager::searchGoodsByCode(fullScanInfo);

        if(info == NULL)
        {
            qDebug()<<"[fetch]"<<"scan data not find";
            config->showMsg(MSG_GOODS_NOT_FIND, 1);
            win_access->showTips(MSG_GOODS_NOT_FIND, 1);
            return;
        }

        if(SqlManager::getGoodsCount(info->packageId)>0)//物品未取完
        {
            SqlManager::scanFetch(fullScanInfo, SqlManager::no_rep, SqlManager::mask_all);
            updateCase(addr);
            win_access->scanOpen(scanGoodsId, fullScanInfo);
            emit goodsAccess(addr,fullScanInfo, 1, 1);
        }
        else
        {
            SqlManager::scanFetch(fullScanInfo, SqlManager::no_rep, SqlManager::mask_all);
            updateCase(addr);
            win_access->scanOpen(scanGoodsId, fullScanInfo);
            config->showMsg(MSG_GOODS_USE_UP, 1);
            win_access->showTips(MSG_GOODS_USE_UP, 1);
        }
    }
    else if(config->state == STATE_REFUN)
    {
        win_refund->refundScan(scanGoodsId,fullScanInfo);
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
        win_check->checkScan(scanGoodsId,fullScanInfo);
    }
    else if(config->state == STATE_REBIND)
    {
        if(scanGoodsId.isEmpty())
        {
            config->showMsg(MSG_REBIND_UNKNOW, 0);
            return;
        }

        config->showMsg(MSG_REBIND_SELECT, 0);
        rebindGoods = scanGoodsId;
        waitForCodeScan = false;
    }
    else if(config->state == STATE_BACK)
    {
        QPoint addr = SqlManager::searchByCode(fullScanInfo);
        if((addr.x() >= 0) && (addr.y() >= 0))
        {
            SqlManager::scanFetch(fullScanInfo, SqlManager::all_rep, SqlManager::mask_all);
            updateCase(addr);
        }

        win_access->scanOpen(scanGoodsId, fullScanInfo);
        emit goodsAccess(addr,fullScanInfo, 1, 16);
    }

}

void CabinetWidget::updateScanState(bool state)
{
    waitForCodeScan = state;
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
    waitForInit = true;
    panel_init(config->list_cabinet);
}

void CabinetWidget::updateDelay(int delay)
{
//    qDebug()<<"update delay"<<delay;
    updateNetState(delay);
    ui->delay->setText(QString("%1ms").arg(delay));
    config->netState = ((delay<1000) && (delay>0));
    ui->delay->setChecked(config->netState);
}

void CabinetWidget::showEvent(QShowEvent *)
{
    ui->cabId->setText(QString("设备终端NO:%1").arg(config->getCabinetId()));

    if(config->state == STATE_STORE)
    {
        waitForCodeScan = true;
        config->showMsg(MSG_STORE, false);
    }
    else if(config->state == STATE_FETCH)
    {
        clickLock = false;
        selectCab = -1;
        selectCase = -1;
        config->showMsg(MSG_FETCH, false);
    }
}

void CabinetWidget::updateShowMap()
{
    QMap<QWidget*, bool>::iterator it;
    for(it=showMap.begin(); it!=showMap.end(); it++)
    {
        it.key()->setVisible(it.value());
    }
}

void CabinetWidget::hideAllShowMap()
{
    QMap<QWidget*, bool>::iterator it;
    for(it=showMap.begin(); it!=showMap.end(); it++)
    {
        it.key()->setVisible(false);
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
//    ui->devReader->setVisible(true);
//    ui->devScan->setVisible(true);
    ui->devReader->setVisible(config->getCardReaderState());
    ui->devScan->setVisible(config->getCodeScanState());

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

void CabinetWidget::updateCase(int col, int row)
{
    if(col<0 || col>=list_cabinet.count())
        return;

    list_cabinet[col]->updateCase(row);
}

void CabinetWidget::updateCase(QPoint pos)
{
    updateCase(pos.x(), pos.y());
}

void CabinetWidget::updateOptStamp()
{
    lastOptTime = QTime::currentTime();
//    if(screenProState)
//    {
//        screenProState = false;
//        emit screenPro(screenProState);
//    }
}

void CabinetWidget::switchCabinetState(CabState state)
{
    config->state = state;
    qDebug()<<"[switchCabinetState]"<<state;
    switch(state)
    {
    case STATE_FETCH:
        clickLock = false;
        waitForGoodsListCode = false;
        caseClicked(0,0);
        break;
    case STATE_STORE:
        on_store_clicked(true);
        break;
    case STATE_REFUN:
        clickLock = false;
        caseClicked(0,0);
        break;
    case STATE_CHECK:
        clickLock = false;
        caseClicked(0,0);
        break;
    case STATE_BACK:
        clickLock = false;
        caseClicked(0,0);
        break;
    case STATE_LIST:
        break;
    case STATE_REBIND:
        caseClicked(0,0);
        break;
    case STATE_SPEC:
        break;
    case CMD_CHECK_SHOW:
        emit requireCheckShow();
        break;
    case CMD_DAY_REPORT_SHOW:
        emit requireDayReportShow();
        break;
    default:
        break;
    }
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
        config->wakeUp(TIMEOUT_BASE);
    }
    else
    {
        cabLock();
    }
}

void CabinetWidget::on_rebind_clicked(bool checked)
{
    if(checked)
    {
        clearMenuState();
        ui->rebind->setChecked(true);
        config->state = STATE_REBIND;
        config->showMsg(MSG_REBIND_SCAN,0);
        config->wakeUp(TIMEOUT_BASE);
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
        qDebug()<<"[REFUND]";
        clickLock = false;
        config->state = STATE_REFUN;
        clearMenuState();
        config->showMsg(MSG_REFUND,false);
        ui->refund->setChecked(true);
        waitForCodeScan = true;
        waitForGoodsListCode = false;
        config->wakeUp(TIMEOUT_FETCH);
    }
    else
    {
        cabLock();
    }
}

void CabinetWidget::on_back_clicked(bool checked)
{
    if(checked)
    {
        qDebug()<<"[BACK]";
        clickLock = false;
        config->state = STATE_BACK;
        clearMenuState();
        config->showMsg(MSG_BACK,false);
        ui->back->setChecked(true);
        waitForCodeScan = true;
        waitForGoodsListCode = false;
        config->wakeUp(TIMEOUT_FETCH);
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
        clearMenuState();
        config->showMsg(MSG_SCAN_LIST, false);
        waitForCodeScan = true;
        waitForGoodsListCode = true;
        win_access->setAccessModel(true);
        ui->store->setChecked(true);
        config->wakeUp(TIMEOUT_BASE);
        if(config->getStoreMode())
            win_store_list->show();
    }
    else
    {
        cabLock();
        initAccessState();
    }
}

void CabinetWidget::on_store_toggled(bool checked)
{
    if(!checked)
    {
        waitForGoodsListCode = false;
    }
    else
    {
        waitForGoodsListCode = true;
    }
}

void CabinetWidget::on_cut_clicked()
{
    clearMenuState();
    config->state = STATE_FETCH;
    config->showMsg(MSG_EMPTY,false);
    waitForCodeScan = true;
    waitForGoodsListCode = false;
    win_cab_list_view->setNetState(ui->netState->isChecked());
    win_cab_list_view->show();
    config->wakeUp(TIMEOUT_BASE);
}

void CabinetWidget::on_check_clicked(bool checked)
{
#ifndef TCP_API
    if(checked)
    {
        clearMenuState();
        config->showMsg(MSG_CHECK_CREAT,false);
        emit requireGoodsCheck();
    }
    else
    {
        ui->check->setChecked(true);
        int uckNum = config->getUncheckCaseNum();
        QString msg;
        if(uckNum)
            msg = QString("还有%1个柜格未盘点").arg(uckNum);
        else
            msg = QString("柜格已全部盘点，可以提交");

        if(!config->getCabinetType().at(BIT_CAB_AIO))
            win_check_warnning->warnningMsg(msg, true);
        else
            emit goodsCheckFinish();
    }
#else
    if(checked)
    {
        if(msgBox != NULL)
            delete msgBox;

        waitForSecondaryCard = true;
        waitForCheckFinish = false;
        waitForCardReader = true;
        msgBox = new QMessageBox(QMessageBox::Information, "正在创建盘点", "请护士长刷卡", QMessageBox::Ok, this);
        msgBox->setModal(false);
        msgBox->show();
    }
    else
    {
        waitForSecondaryCard = true;
        waitForCheckFinish = true;
        waitForCardReader = true;
        msgBox = new QMessageBox(QMessageBox::Information, "正在创建盘点", "请护士长刷卡", QMessageBox::Ok, this);
        msgBox->setModal(false);
        msgBox->show();
    }
#endif
}

//void CabinetWidget::on_check_toggled(bool checked)
//{
//    if(!checked)
//    {
//        if(checkFinishLock)
//        {
//            checkFinishLock = false;
//            return;
//        }
//        emit goodsCheckFinish();
//    }
//}

void CabinetWidget::updateNetState(bool connected)
{
    netCheckState = connected;
    ui->netState->setChecked(netCheckState);
}

void CabinetWidget::saveStore(Goods *goods, int num)
{qDebug("[saveStore]");
    Q_UNUSED(goods);
    Q_UNUSED(num);
//    CaseAddress addr = config->checkCabinetByName(goods->name);

//    emit goodsAccess(addr, goods->packageBarcode, num, 2);
//    scanInfo.clear();
//    curStoreList->goodsIn(goods->packageBarcode, num);

//    if(curStoreList->isFinished())
//        config->showMsg(MSG_EMPTY, false);
//    else
//        config->showMsg(MSG_STORE, false);

}

void CabinetWidget::saveFetch(QString name, int num)
{
    Q_UNUSED(name);
    Q_UNUSED(num);
//    if(num<=0)
//        return;

//    CaseAddress addr = config->checkCabinetByName(name);
//    clickLock = false;
//    emit requireOpenCase(addr.cabinetSeqNum, addr.caseIndex);
    //    emit goodsAccess(addr, config->list_cabinet[addr.cabinetSeqNum]->list_case[addr.caseIndex]->list_goods.at(addr.goodsIndex)->id, num, 1);
}

void CabinetWidget::searchByPinyin(QString str)
{
    if(str.isEmpty())
    {
        clearCaseState();
        return;
    }
    QList<QPoint> l_search = SqlManager::goodsSearch(str);
    setSearchState(l_search);
}


void CabinetWidget::msgClear()
{
    MessageDialog::instance().showFinish();
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

    showMap[ui->back] = false;
    showMap[ui->store] = false;
    showMap[ui->refund] = false;
    showMap[ui->service] = false;
    showMap[ui->cut] = false;
    showMap[ui->check] = false;
    showMap[ui->reply] = false;
    showMap[ui->search] = true;
    showMap[ui->rebind] = false;
    showMap[ui->quit] = false;
    showMap[ui->btn_check_table] = true;
    showMap[ui->consume_date] = true;

    if(ui->netState->isChecked())
    {
        switch(power)
        {
        case 0://超级管理员:|补货|退货|服务|退出|
            showMap[ui->store] = true;
            showMap[ui->refund] = true;
            showMap[ui->service] = true;
            showMap[ui->rebind] = true;
            showMap[ui->cut] = true;
            showMap[ui->check] = true;
            showMap[ui->quit] = true;
            showMap[ui->reply] = true;
            showMap[ui->back] = config->getFuncWord() & funcBack;
            showMap[ui->refund] = config->getFuncWord() & funcRefun;
            showMap[ui->check] = config->getFuncWord() & funcCheck;
            showMap[ui->reply] = config->getFuncWord() & funcApply;
            break;

//        case 1://仓库员工:|补货|退货|退出|
//            showMap[ui->store] = true;
//            showMap[ui->refund] = true;
//            showMap[ui->cut] = true;
//            showMap[ui->check] = true;
//            showMap[ui->reply] = true;
//            break;

//        case 2://医院管理:|补货|退货|服务|退出|
//            showMap[ui->store] = true;
//            showMap[ui->refund] = true;
//            showMap[ui->cut] = true;
//            showMap[ui->reply] = true;
//            //        showMap[ui->service] = true;
//            break;

        case 1://护士长:|退货|退出|
            showMap[ui->reply] = true;
//            showMap[ui->refund] = true;
            showMap[ui->cut] = true;
            showMap[ui->quit] = true;
            showMap[ui->back] = config->getFuncWord() & funcBack;
            showMap[ui->refund] = config->getFuncWord() & funcRefun;
//            showMap[ui->check] = config->getFuncWord() & funcCheck;
            showMap[ui->reply] = config->getFuncWord() & funcApply;
            //        showMap[ui->service] = true;
            break;

        case 2://护士:|退出|
            showMap[ui->reply] = true;
            showMap[ui->cut] = true;
            showMap[ui->quit] = true;
            showMap[ui->back] = config->getFuncWord() & funcBack;
            showMap[ui->reply] = config->getFuncWord() & funcApply;
            //        ui->service] = true;
            break;

        case 3://管理员:|补货|退货|退出|
            showMap[ui->store] = true;
            showMap[ui->rebind] = true;
//            showMap[ui->service] = true;
            showMap[ui->cut] = true;
            showMap[ui->check] = true;
            showMap[ui->reply] = true;
            showMap[ui->quit] = true;
            showMap[ui->back] = config->getFuncWord() & funcBack;
            showMap[ui->refund] = config->getFuncWord() & funcRefun;
            showMap[ui->check] = config->getFuncWord() & funcCheck;
            showMap[ui->reply] = config->getFuncWord() & funcApply;
            break;

        case 4://医院员工:|退出|
//            showMap[ui->cut] = true;
            showMap[ui->quit] = true;
            showMap[ui->back] = config->getFuncWord() & funcBack;
            showMap[ui->reply] = config->getFuncWord() & funcApply;
            //        showMap[ui->service] = true;
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
            showMap[ui->quit] = true;
            showMap[ui->service] = true;
            showMap[ui->cut] = true;
            break;

        case 1://仓库员工:
            showMap[ui->quit] = true;
            showMap[ui->cut] = true;
            break;

        case 2://医院管理:
            showMap[ui->quit] = true;
            showMap[ui->cut] = true;
            //        showMap[ui->service] = true;
            break;

        case 3://医院员工:
            showMap[ui->cut] = true;
            showMap[ui->quit] = true;
            //        showMap[ui->service] = true;
            break;
        case 4://医院员工:
            showMap[ui->cut] = true;
            showMap[ui->quit] = true;
            //        showMap[ui->service] = true;
            break;
        default:
            break;
        }
    }
//    showMap[ui->back] = config->getFuncWord() & funcBack;
//    showMap[ui->refund] = config->getFuncWord() & funcRefun;
//    showMap[ui->check] = config->getFuncWord() & funcCheck;
//    showMap[ui->reply] = config->getFuncWord() & funcApply;
    updateShowMap();

//    qDebug()<<(config->getFuncWord() & funcBack)<<(config->getFuncWord() & funcRefun)<<(config->getFuncWord() & funcCheck);
}

void CabinetWidget::recvUserInfo(QByteArray qba)
{
//    calCheck(QString(qba));

    if(this->isHidden() && (config->getCabinetType().at(BIT_CAB_AIO)))
    {
        qDebug()<<"[CabinetWidget]"<<"recvUserInfo"<<qba<<"ignore..";
        return;
    }

//    if(!waitForCardReader)
//    {
//        qDebug()<<"[CabinetWidget]"<<"recvUserInfo not need.";
//        msgClear();
//        return;
//    }

//    waitForCardReader = false;
//    optUser = QString(qba);
    if(waitForSecondaryCard)//NEW_API
    {
        waitForSecondaryCard = false;
        config->setSecondUser(QString(qba));
        if(msgBox != NULL)
        {
            msgBox->close();
            delete msgBox;
            msgBox = NULL;
        }
        if(waitForCheckFinish)
        {
            emit goodsCheckFinish();
        }
        else
        {
            checkCreat();
        }

        return;
    }

    MessageDialog::instance().showMessage("身份验证中...", 10);
//    msgShow("身份验证", "身份验证中...",false);
    emit requireUserCheck(QString(qba));

//        setPowerState(0);
}

void CabinetWidget::recvListInfo(GoodsList *l)
{
    qDebug("[recvListInfo]");
    if((l->list_goods.count() == 0) || (!l->legalList))
    {
        config->showMsg(MSG_LIST_ERROR, 1);
        win_store_list->listError(MSG_LIST_ERROR);
        return;
    }

    curStoreList = l;
    win_store_list->setLoginState(loginState);
    win_store_list->storeStart(l);
    if(!config->getStoreMode())
        win_store_list->show();
    return;

    win_access->setStoreList(l);
    config->showMsg(MSG_STORE, false);
    waitForCodeScan = true;
    waitForGoodsListCode = false;
}

void CabinetWidget::recvBindRst(bool rst)
{
    qDebug("recvBindRst");
    if(config->state == STATE_REBIND)
    {
        if(rst)
            rebindOver();
        else
            rebindRecover();
    }
    else
    {
        if(rst)
        {
            CaseAddress addr;
            addr.cabinetSeqNum = bindCab;
            addr.caseIndex = bindCase;
//            cabInfoBind(bindCab, bindCase, bindInfo);

            win_store_list->show();
            setMenuHide(false);
            win_store_list->bindRst(addr);
            win_store_list->bindMsg("绑定成功");
    //        win_access->clickOpen(curGoods->packageBarcode);
    //        emit requireOpenCase(selectCab, selectCase);
        }
        else
        {
            clickLock = false;
            win_store_list->show();
            setMenuHide(false);
            win_store_list->bindMsg("绑定失败");
    //        win_access->save();
    //        win_access->hide();
            config->showMsg(MSG_STORE_SELECT, false);
        }
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
    QPoint pos = SqlManager::searchByPackageId(goodsId);
    if(pos.x() == -1)
    {
        qDebug()<<"[recvGoodsNumInfo]:search goodsId failed";
        return;
    }
    else
    {
        updateCase(pos);
        if(config->state == STATE_LIST)
            win_cab_list_view->fetchSuccess();
        else if(config->state == STATE_STORE)
            win_store_list->storeRst("存入成功",true);
        else if(config->state == STATE_REFUN)
            win_refund->refundRst("退货成功");
        else
        {
            qDebug("goodsNumChanged");
            if(!win_access->isHidden())
                emit goodsNumChanged(num);
        }
    }
}

void CabinetWidget::accessFailedMsg(QString msg)
{
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
    else if(config->state == STATE_BACK)
    {
        win_access->fetchFailed(msg);
    }
    qDebug()<<msg;
}

void CabinetWidget::accessSuccessMsg(QString msg)
{
    qDebug()<<"[accessSuccessMsg]"<<msg;
    if(config->state == STATE_FETCH)
    {
        win_access->showTips(msg, false);
    }
}

void CabinetWidget::updateFetchPrice(float single, float total)
{
    win_access->setPrice(single, total);
}

void CabinetWidget::updateTime()
{
    showCurrentTime(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    float cupTemp = SysTool::getCpuTemp();
//    qDebug()<<"[cpu temp]:"<<cupTemp<<"℃";
    if(cupTemp > 41.0)
    {
        emit cpuFanOn(true);
    }
    else if(cupTemp < 39.0)
    {
        emit cpuFanOn(false);
    }

    if((QDateTime::currentDateTime().time().hour() == 4) && (QTime::currentTime().minute() == 0) && (QTime::currentTime().second() == 0))
    {
        qDebug("[update time]");
        emit reqCheckVersion(false);
    }
//    if((lastOptTime.secsTo(QTime::currentTime()) > 180) && (!screenProState))
//    {
//        screenProState = true;
//        emit screenPro(screenProState);
//    }
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

void CabinetWidget::sysLock()
{qDebug("sysLock");
    ui->store->setChecked(false);
    ui->service->setChecked(false);
    ui->cut->setChecked(false);
    ui->refund->setChecked(false);
    ui->check->setChecked(false);
    ui->search->setChecked(false);
    this->show();
//    qDebug("1");
    cabLock();
}

void CabinetWidget::recvCabSyncResult(bool rst)
{
    if(rst)
        ui->syncMsg->setText("智能柜数据同步成功");
    else
        ui->syncMsg->setText("智能柜数据同步失败");

    QTimer::singleShot(3000, this, SLOT(syncMsgTimeout()));
}

void CabinetWidget::recvCheckRst(bool success, QString msg)
{
    if(success)
    {
        checkStart();
    }
    else
    {
        config->showMsg(msg,false);
        ui->check->setChecked(false);
    }
}

void CabinetWidget::recvCheckCreatRst(bool success, QString msg)
{
    if(success)
    {
        checkStart();
    }
    else
    {
        config->showMsg(msg,false);
        ui->check->setChecked(false);
    }
}
//MASTER
void CabinetWidget::recvCheckFinish(bool success)
{
    if(success)
    {
        ui->check->setChecked(false);
        cabLock();
        win_check_warnning->close();

    }
}
//NEW_API
void CabinetWidget::recvCheckFinishRst(bool success, QString msg)
{
    if(success)
    {
        waitForCheckFinish = false;
        ui->check->setChecked(false);
//        clearCaseState();//重置单元格状态
        cabLock();
        on_netState_clicked();
    }
    else
    {
        config->showMsg(msg,false);
        ui->check->setChecked(true);
    }
}

void CabinetWidget::recvGoodsTraceRst(bool success, QString msg, QString goodsCode)
{
    win_store_list->recvStoreTraceRst(success, msg, goodsCode);
}

void CabinetWidget::setMenuHide(bool ishide)
{
    if(ishide)
    {
        hideAllShowMap();
    }
    else
    {
        updateShowMap();
    }
}

void CabinetWidget::cabinetBind(Goods *goods)
{
    curGoods = goods;
    clickLock = false;
    setMenuHide(true);

    config->showMsg(MSG_STORE_SELECT, false);
    if(config->getCabinetType().at(BIT_CAB_AIO))
        caseClicked(0,0);
    else
        win_store_list->hide();
}

void CabinetWidget::checkOneCase(QList<CabinetCheckItem *> l, QPoint addr)
{
    updateCase(addr);
    emit checkCase(l, addr);
}

void CabinetWidget::checkPush()
{
    emit goodsCheckFinish();
}

void CabinetWidget::recvUserCheckRst(UserInfo* info)
{
    if(info == NULL)
        return;

    cabInit();
    waitForCodeScan = false;
    waitForGoodsListCode = false;
    rebindGoods = QString();
    msgClear();
//    emit updateLoginState(0xff, true);
    clearCaseState();
    emit updateLoginState(true);
    optUser = info;
    curCard = optUser->cardId;
//    tsCalFlag = 0;
    config->setOptId(info->cardId);
    qDebug()<<"[recvUserCheckRst]"<<optUser->cardId;
    config->showMsg(MSG_EMPTY, false);
    config->state = STATE_FETCH;
    ui->userInfo->setText(QString("     您好！%1").arg(optUser->name));
    setPowerState(info->power);
    loginState = true;

    if(win_store_list->isVisible())
        config->state = STATE_STORE;
    win_store_list->setLoginState(loginState);
    emit loginStateChanged(loginState);

    config->cabVoice.voicePlay(VOICE_WELCOME_USE);
}

void CabinetWidget::on_search_clicked()
{
    clearMenuState();
    config->wakeUp(TIMEOUT_BASE);
    config->state = STATE_FETCH;
    ui->menuWidget->setCurrentIndex(1);
}

void CabinetWidget::on_search_back_clicked()
{
    config->wakeUp(TIMEOUT_BASE);
    ui->menuWidget->setCurrentIndex(0);
}

void CabinetWidget::on_searchClear_clicked()
{
    ui->searchStr->clear();
    clearCaseState();
//    config->clearSearch();
}

void CabinetWidget::pinyinSearch(int id)
{
    QString str = ui->searchStr->text()+groupBtn.button(id)->text();
    ui->searchStr->setText(str);
    searchByPinyin(str);
//    config->searchByPinyin(str);
}

void CabinetWidget::on_netState_clicked()
{
    ui->netState->setChecked(netCheckState);
    ui->syncMsg->setText("智能柜数据同步中..");
    emit requireCabSync();
    QTimer::singleShot(3000, this, SLOT(syncMsgTimeout()));
}

void CabinetWidget::on_volCtrl_clicked()
{
    if(volume->isHidden())
    {
        QPoint showPos = ui->volCtrl->geometry().topLeft();
        showPos = ui->volCtrl->mapToGlobal(QPoint(0,0));
        showPos.setY(showPos.y()+ui->volCtrl->height()+10);
        volume->move(showPos);
        volume->show();
        volume->setFocus();
    }
    else
    {
        volume->hide();
//        volume->show();
    }

}

void CabinetWidget::vol_changed(int vol)
{
    if(!volPressed)
    {
        config->setSysVolem(vol);
        volumTest();
        qDebug()<<"vol_changed";
    }
}

void CabinetWidget::vol_released()
{
    config->setSysVolem(volume->value());
    volumTest();
    qDebug()<<"vol_released";
    volPressed = false;
}

void CabinetWidget::vol_pressed()
{
    volPressed = true;
}

void CabinetWidget::syncMsgTimeout()
{
    ui->syncMsg->clear();
}

bool CabinetWidget::eventFilter(QObject *w, QEvent *e)
{
    if(w == volume)
    {
        if(e->type() == QEvent::FocusOut)
        {
            volume->hide();
            return true;
        }
    }

    return QWidget::eventFilter(w, e);
}

void CabinetWidget::resizeEvent(QResizeEvent *)
{
    QRect rect_menu = ui->frm_menu->geometry();
    rect_menu.moveTo(0,0);
    ui->menuWidget->setGeometry(rect_menu);
    ui->lab_copyright->setGeometry(rect_menu);
}

void CabinetWidget::on_quit_clicked()
{
    cabLock();
}


void CabinetWidget::on_btn_check_table_clicked()
{
    emit requireCheckShow();
}

void CabinetWidget::on_reply_clicked()
{
    waitForCodeScan = false;
    waitForGoodsListCode = false;
    waitForCardReader = true;
    clearMenuState();
    config->showMsg(MSG_EMPTY,false);
    config->state = STATE_NO;
    emit requireApplyShow();
}

void CabinetWidget::on_consume_date_clicked()
{
    emit requireDayReportShow();
}


void CabinetWidget::on_check_toggled(bool checked)
{
    if(checked)
        ui->check->setText("结束\n盘点");
    else
        ui->check->setText("盘点");
}

