#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QtDebug>
#include <QMessageBox>
#include <QDir>
#include <QTextStream>
#include <QMetaType>
#include "defines.h"
//#include "test/setdebugnew.h"

#define LatticeNum 7  //定义药柜格子数

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    init_huangpo();
}

void MainWidget::cabinetClear()
{
    cabinetConf->clearConfig();
}

void MainWidget::globalTouch()
{
    qDebug()<<"[globalTouch]";
    cabinetConf->clearTimeoutFlag();
    win_cabinet->updateOptStamp();
}

void MainWidget::init_huangpo()
{
    qDebug("init_huangpo<<server");
    qRegisterMetaType<QList<CabinetStoreListItem*> >("QList<CabinetStoreListItem*>");
    qRegisterMetaType<QList<CabinetCheckItem*> >("QList<CabinetCheckItem*>");
    qRegisterMetaType<QList<DayReportInfo*> >("QList<DayReportInfo*>");
    qRegisterMetaType<QList<Goods*> >("QList<GoodsInfo*>");
    qRegisterMetaType<AIOMachine::cEvent>("AIOMachine::cEvent");
    qRegisterMetaType<EpcInfo*>("EpcInfo*");
    qRegisterMetaType<TableMark>("TableMark");
    qRegisterMetaType<TempDevHub*>("TempDevHub*");

    devWatcher = new QDeviceWatcher(this);

//    win_screenPro = new ScreenPro;

    win_aio = NULL;

    //智能柜配置
    cabinetConf = CabinetConfig::config();
    cabinetConf->configInit();

    routeRepair = new RouteRepair(this);

#ifdef TCP_API
    //tcp通信类
    cabServer = new tcpServer(this);
    cabServer->installGlobalConfig(cabinetConf);
#else
    //http通信类
    cabServer = new CabinetServer(this);
    cabServer->installGlobalConfig(cabinetConf);
#endif

//    //tcp通信类
//    tcpApi = new tcpServer(this);
//    tcpApi->installGlobalConfig(cabinetConf);
//    tcpApi->setServer(QHostAddress("120.78.144.255"), 8088);

    //仿真控制台
    ctrlUi = new ControlDevice;
    ctrlUi->installGlobalConfig(cabinetConf);
    connect(cabServer, SIGNAL(newGoodsCar(GoodsCar)), ctrlUi, SLOT(readyForNewCar(GoodsCar)));

    //扫码输入面板
    win_coder_keyboard = new coderKeyboard();
    connect(win_coder_keyboard, SIGNAL(coderData(QByteArray)), ctrlUi, SIGNAL(codeScanData(QByteArray)));

    //盘点表格窗口
    win_check_table = new CheckTable();
    connect(cabServer, SIGNAL(curCheckList(CheckList*)), win_check_table, SLOT(updateCheckTable(CheckList*)));
    connect(cabServer, SIGNAL(checkTables(QList<CheckTableInfo*>)), win_check_table, SLOT(recvCheckTables(QList<CheckTableInfo*>)));
    connect(win_check_table, SIGNAL(askCheckTables(QDate,QDate)), cabServer, SLOT(requireCheckTables(QDate,QDate)));
    connect(win_check_table, SIGNAL(askCheckInfo(QString)), cabServer, SLOT(requireCheckTableInfo(QString)));

    //请货窗口
    win_goods_apply = new GoodsApply();
    connect(win_goods_apply, SIGNAL(searchRequire(QString)), cabServer, SLOT(searchSpell(QString)));
    connect(win_goods_apply, SIGNAL(replyRequire(QList<GoodsCheckInfo*>)), cabServer, SLOT(replyRequire(QList<GoodsCheckInfo*>)));
    connect(cabServer, SIGNAL(curSearchList(CheckList*)), win_goods_apply, SLOT(recvSearchRst(CheckList*)));
    connect(cabServer, SIGNAL(goodsReplyRst(bool,QString)), win_goods_apply, SLOT(recvReplyRst(bool,QString)));

    win_day_report = new DayReport();
    connect(win_day_report, SIGNAL(askListInfo(QDate,QDate)), cabServer, SLOT(requireListInfo(QDate,QDate)));
    connect(cabServer, SIGNAL(dayReportRst(QList<DayReportInfo*>,QString)), win_day_report, SLOT(recvReportInfo(QList<DayReportInfo*>, QString)));

    //服务界面
    win_cab_service = new CabinetService();
    win_cab_service->installGlobalConfig(cabinetConf);
    connect(win_cab_service, SIGNAL(winSwitch(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(win_cab_service, SIGNAL(requireOpenLock(int,int)), ctrlUi, SLOT(openLock(int,int)));
    connect(win_cab_service, SIGNAL(requireClear()), this, SLOT(cabinetClear()));
    connect(win_cab_service, SIGNAL(requireUpdateServerAddress()), cabServer, SLOT(updateAddress()));
    connect(win_cab_service, SIGNAL(userCardActive(QByteArray)), ctrlUi, SIGNAL(cardReaderData(QByteArray)));
    connect(ctrlUi, SIGNAL(cardReaderData(QByteArray)), win_cab_service, SLOT(recvCurCardId(QByteArray)));

#ifdef TCP_API
    connect(win_cab_service, SIGNAL(requireInsertCol(int,QString)), cabServer, SLOT(cabColInsert(int,QString)));
#else
    connect(win_cab_service, SIGNAL(requireInsertCol(int,int)), cabServer, SLOT(cabColInsert(int,int)));
#endif
//    connect(win_cab_service, SIGNAL(requireInsertUndo()), cabServer, SLOT(cabInsertUndo()));
    connect(cabServer, SIGNAL(insertRst(bool)), win_cab_service, SLOT(recvInsertColResult(bool)));
//    connect(cabServer, SIGNAL(insertUndoRst(bool)), win_cab_service, SLOT(recvInsertUndoResult(bool)));

    //智能柜展示界面
    win_cabinet = new CabinetWidget(this);
    win_cabinet->installGlobalConfig(cabinetConf);
    connect(win_cabinet, SIGNAL(stack_switch(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
//    connect(win_cabinet, SIGNAL(screenPro(bool)), win_screenPro, SLOT(updateProState(bool)));
#ifdef TCP_API
    connect_new_api();
#else
    //TCP接口,对接肖萍
    cabTcp = new CabinetTcp(this);
    connect(cabTcp, SIGNAL(serverDelay(int)), win_cabinet, SLOT(updateDelay(int)));
    connect(cabTcp, SIGNAL(syncRequire()), cabServer, SLOT(cabInfoSync()));
    connect(cabTcp, SIGNAL(requireOpenCase(int,int)), ctrlUi, SLOT(openCase(int,int)));
    connect_master();
#endif

    ledCtrl = new LedCtrl(this);
    connect(win_cabinet, SIGNAL(cpuFanOn(bool)), ledCtrl, SLOT(fanSwitch(bool)));
//    connect(win_cabinet, SIGNAL(updateLoginState(int,bool)), ledCtrl, SLOT(ledSwitch(int,bool)));//登入登出控制led
    connect(win_cab_service, SIGNAL(doorState(int)), ctrlUi, SLOT(setLed(int)));

    tempDev = new TempDev(this);
    connect(ctrlUi, SIGNAL(tempData(QByteArray)), tempDev, SLOT(recvTempData(QByteArray)));
    connect(tempDev, SIGNAL(updateHum(float)), cabTcp, SLOT(updateHum(float)));
    connect(tempDev, SIGNAL(updateTemp(float)), cabTcp, SLOT(updateTemp(float)));

    if(cabinetConf->getCabinetType().at(BIT_CAB_AIO))
    {
        win_aio = new AIOMachine(this);
        connect(win_aio, SIGNAL(requireOpenLock(int,int)), ctrlUi, SLOT(openLock(int,int)));
        connect(win_aio, SIGNAL(requireUserCheck(QString)), cabServer, SLOT(userLogin(QString)));
        connect(win_aio, SIGNAL(stack_switch(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
        connect(win_aio, SIGNAL(aio_check(bool)), win_cabinet, SLOT(on_check_clicked(bool)));
//        connect(win_aio, SIGNAL(service_show()), win_cab_service, SLOT(show()));
        connect(cabServer, SIGNAL(loginRst(UserInfo*)), win_aio, SLOT(recvUserCheckRst(UserInfo*)));
        connect(cabServer, SIGNAL(sysLock()), win_aio, SLOT(sysLock()));
        connect(win_aio, SIGNAL(logout()), win_cabinet, SLOT(sysLock()));
        connect(win_aio, SIGNAL(reqUpdateOverview()), cabServer, SLOT(requireAioOverview()));
        connect(cabServer, SIGNAL(aioOverview(QString,AIOOverview*)), win_aio, SLOT(recvAioOverview(QString,AIOOverview*)));
        connect(win_aio, SIGNAL(click_event(int)), cabServer, SLOT(requireAioData(int)));
        connect(cabServer, SIGNAL(aioData(QString,AIOMachine::cEvent,QList<Goods*>)), win_aio, SLOT(recvAioData(QString,AIOMachine::cEvent,QList<Goods*>)));
        connect(win_aio, SIGNAL(tsCalReq()), win_cab_service, SLOT(tsCalibration()));
        connect(win_aio, SIGNAL(cabinetStateChange(CabState)), win_cabinet, SLOT(switchCabinetState(CabState)));
        connect(tempDev, SIGNAL(updateHumString(QString)), win_aio, SLOT(updateHum(QString)));
        connect(tempDev, SIGNAL(updateTempString(QString)), win_aio, SLOT(updateTemp(QString)));
        connect(win_aio->findChild<TempDevHub *>("tempHub"), SIGNAL(tempDevReport(QByteArray)), cabServer, SLOT(tempDevReport(QByteArray)));
//        qDebug()<<win_aio->findChild<TempDevHub *>("tempHub");
        //    connect(win_aio, SIGNAL(aio_fetch(int,int)), win_cabinet, SLOT(caseClicked(int,int)));
    //    connect(win_aio, SIGNAL(aio_return(bool)), win_cabinet, SLOT(on_refund_clicked(bool)));
        ui->page_2->layout()->addWidget(win_aio);
//        win_aio->sysLock();
    }

    //待机界面
    win_standby = new StandbyWidget(this);
    win_standby->installGlobalConfig(cabinetConf);
//    connect(ctrlUi, SIGNAL(cardReaderData(QByteArray)), win_standby, SLOT(recvUserInfo(QByteArray)));
    connect(win_standby, SIGNAL(winSwitch(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));

    //用户管理界面
    win_user_manage = new UserWidget(this);
    win_user_manage->installGlobalConfig(cabinetConf);
    connect(win_user_manage, SIGNAL(winSwitch(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
//    connect(ctrlUi, SIGNAL(cardReaderData(QByteArray)), win_user_manage, SLOT(recvUserInfo(QByteArray)));

    //智能柜组合设置界面
    win_cabinet_set = new CabinetSet(this);
    win_cabinet_set->installGlobalConfig(cabinetConf);
    connect(win_cabinet_set, SIGNAL(winSwitch(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(win_cabinet_set, SIGNAL(cabinetCreated()), win_cabinet, SLOT(cabinetInit()));
    connect(win_cabinet_set, SIGNAL(lockTest()), win_cab_service, SLOT(ctrl_boardcast()));
    connect(win_cabinet_set, SIGNAL(requireOpenCase(int,int)), ctrlUi, SLOT(openLock(int,int)));
    connect(win_cabinet_set, SIGNAL(updateServerAddr()),cabServer, SLOT(getServerAddr()));
    connect(win_cabinet_set, SIGNAL(cabinetClone(QString)), cabServer, SLOT(cabCloneReq(QString)));
    connect(win_cabinet_set, SIGNAL(requireCabRigster()), cabServer, SLOT(cabRegister()));
    connect(cabServer, SIGNAL(regResult(bool)), win_cabinet_set, SLOT(regResult(bool)));
    connect(cabServer, SIGNAL(cloneResult(bool,QString)), win_cabinet_set, SLOT(cloneResult(bool,QString)));
    connect(ctrlUi, SIGNAL(cardReaderData(QByteArray)), win_cabinet_set, SLOT(getCardId(QByteArray)));
    connect(ctrlUi, SIGNAL(codeScanData(QByteArray)), win_cabinet_set, SLOT(getCodeScanData(QByteArray)));

//    connect(win_cabinet_set, SIGNAL(setCabinet(QByteArray)), cabinetConf, SLOT(creatCabinetConfig(QByteArray)));


//    win_fingerPrint = new FingerPrint();
//    connect(win_fingerPrint, SIGNAL(requireOpenLock(int,int)), ctrlUi, SLOT(openLock(int,int)));
//    connect(win_fingerPrint, SIGNAL(doorState(int, bool)), ledCtrl, SLOT(ledSwitch(int, bool)));//开关门控制led

    ui->stackedWidget->addWidget(win_standby);
    ui->stackedWidget->addWidget(win_user_manage);
    ui->stackedWidget->addWidget(win_cabinet_set);
    ui->stackedWidget->addWidget(win_cabinet);
    ui->stackedWidget->addWidget(win_cab_service);


    if(cabinetConf->isFirstUse())
    {
        ui->stackedWidget->setCurrentIndex(INDEX_CAB_SET);
    }
    else
    {
        ui->stackedWidget->setCurrentIndex(INDEX_CAB_SHOW);
        win_cabinet->panel_init(cabinetConf->list_cabinet);
        cabinetConf->cabVoice.voicePlay(VOICE_WELCOME);
        if(cabinetConf->getCabinetType().at(BIT_CAB_AIO))
            ui->stackedWidget->setCurrentIndex(INDEX_AIO);
    }
#ifndef PC
    AuthorManager *m = new AuthorManager();
    if(!m->authorCheck())
        ui->stackedWidget->setCurrentIndex(0);
#endif
    qDebug()<<"[currentIndex]"<<ui->stackedWidget->currentIndex();
}

void MainWidget::connect_master()
{
    connect(ctrlUi, SIGNAL(codeScanData(QByteArray)), win_cabinet, SLOT(recvScanData(QByteArray)));
    connect(ctrlUi, SIGNAL(cardReaderData(QByteArray)), win_cabinet, SLOT(recvUserInfo(QByteArray)));
    connect(ctrlUi, SIGNAL(readyListData(QString)), win_cabinet,SLOT(readyGoodsList(QString)));
    connect(win_cabinet, SIGNAL(checkLockState()), ctrlUi, SLOT(getLockState()));
    connect(win_cabinet, SIGNAL(requireOpenCase(int,int)), ctrlUi, SLOT(openCase(int,int)));
    connect(win_cabinet, SIGNAL(winSwitch(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(win_cabinet, SIGNAL(requireUserCheck(QString)), cabServer, SLOT(userLogin(QString)));
    connect(win_cabinet, SIGNAL(requireGoodsListCheck(QString)), cabServer, SLOT(listCheck(QString)));
    connect(win_cabinet, SIGNAL(requireCaseBind(int,int,QString)), cabServer, SLOT(cabinetBind(int,int,QString)));
    connect(win_cabinet, SIGNAL(requireCaseRebind(int,int,QString)), cabServer, SLOT(cabinetBind(int,int,QString)));
    connect(win_cabinet, SIGNAL(goodsAccess(QPoint,QString,int,int)), cabServer, SLOT(goodsAccess(QPoint,QString,int,int)));
    connect(win_cabinet, SIGNAL(requireAccessList(QStringList,int)), cabServer, SLOT(listAccess(QStringList,int)));
    connect(win_cabinet, SIGNAL(checkCase(QList<CabinetCheckItem*>,CaseAddress)), cabServer, SLOT(goodsCheck(QList<CabinetCheckItem*>,CaseAddress)));
    connect(win_cabinet, SIGNAL(checkCase(QStringList,CaseAddress)), cabServer, SLOT(goodsCheck(QStringList,CaseAddress)));
    connect(win_cabinet, SIGNAL(storeList(QList<CabinetStoreListItem*>)), cabServer, SLOT(goodsListStore(QList<CabinetStoreListItem*>)));
    connect(win_cabinet, SIGNAL(newStoreBarCode(QString)), cabServer, SLOT(updateCurBarcode(QString)));
    connect(win_cabinet, SIGNAL(requireCabSync()), cabServer, SLOT(cabInfoSync()));
    connect(win_cabinet, SIGNAL(requireGoodsCheck()), cabServer, SLOT(goodsCheckReq()));
    connect(win_cabinet, SIGNAL(goodsCheckFinish()), cabServer, SLOT(goodsCheckFinish()));
    connect(win_cabinet, SIGNAL(tsCalReq()), win_cab_service, SLOT(tsCalibration()));
    connect(win_cabinet, SIGNAL(requireCheckShow()), win_check_table, SLOT(show()));
    connect(win_cabinet, SIGNAL(requireApplyShow()), win_goods_apply, SLOT(show()));
    connect(win_cabinet, SIGNAL(requireDayReportShow()), win_day_report, SLOT(show()));
    connect(win_cabinet, SIGNAL(reqCheckVersion(bool)), cabServer, SLOT(checkUpdate(bool)));
    connect(win_cabinet, SIGNAL(reportTraceId(QString)), cabServer, SLOT(goodsStoreTrace(QString)));
    connect(win_cab_service, SIGNAL(checkVersion(bool)), cabServer, SLOT(checkUpdate(bool)));
    connect(win_cab_service, SIGNAL(updateStart()), cabServer, SLOT(updateStart()));
    connect(cabServer, SIGNAL(updateCheckRst(bool,QString)), win_cab_service, SLOT(recvVersionInfo(bool,QString)));
    connect(cabServer, SIGNAL(checkCreatRst(bool, QString)), win_cabinet, SLOT(recvCheckRst(bool, QString)));
    connect(cabServer, SIGNAL(cabSyncResult(bool)), win_cabinet, SLOT(recvCabSyncResult(bool)));
    connect(cabServer, SIGNAL(loginRst(UserInfo*)), win_cabinet, SLOT(recvUserCheckRst(UserInfo*)));
    connect(cabServer, SIGNAL(listRst(GoodsList*)), win_cabinet, SLOT(recvListInfo(GoodsList*)));
    connect(cabServer, SIGNAL(bindRst(bool)), win_cabinet, SLOT(recvBindRst(bool)));
    connect(cabServer, SIGNAL(goodsNumChanged(QString,int)), win_cabinet, SLOT(recvGoodsNumInfo(QString,int)));
    connect(cabServer, SIGNAL(accessFailed(QString)), win_cabinet, SLOT(accessFailedMsg(QString)));
    connect(cabServer, SIGNAL(accessSuccess(QString)), win_cabinet, SLOT(accessSuccessMsg(QString)));
    connect(cabServer, SIGNAL(updateGoodsPrice(float,float)), win_cabinet, SLOT(updateFetchPrice(float,float)));
//    connect(cabServer, SIGNAL(timeUpdate()), win_cabinet, SLOT(updateTime()));
    connect(cabServer, SIGNAL(idUpdate()), win_cabinet, SLOT(updateId()));
    connect(cabServer, SIGNAL(goodsCheckRst(QString)), win_cabinet, SLOT(recvGoodsCheckRst(QString)));
    connect(cabServer, SIGNAL(newGoodsList(QString,QString)), win_cabinet, SLOT(newGoodsList(QString,QString)));
    connect(cabServer, SIGNAL(netState(bool)), win_cabinet, SLOT(updateNetState(bool)));
    connect(cabServer, SIGNAL(netState(bool)), routeRepair, SLOT(repairStart(bool)));
    connect(cabServer, SIGNAL(sysLock()), win_cabinet, SLOT(sysLock()));
    connect(cabServer, SIGNAL(checkFinish(bool)), win_cabinet, SLOT(recvCheckFinish(bool)));
    connect(cabServer, SIGNAL(goodsTraceRst(bool,QString,QString)), win_cabinet, SLOT(recvGoodsTraceRst(bool,QString,QString)));
//    connect(routeRepair, SIGNAL(repairOk()), cabServer, SLOT(waitForRepaitOK()));
    routeRepair->repairStart(false);
}

void MainWidget::connect_new_api()
{
//    connect(ctrlUi, SIGNAL(codeScanData(QByteArray)), win_cabinet, SLOT(recvScanData(QByteArray)));
//    connect(ctrlUi, SIGNAL(cardReaderData(QByteArray)), win_cabinet, SLOT(recvUserInfo(QByteArray)));
    connect(ctrlUi, SIGNAL(readyListData(QString)), win_cabinet,SLOT(readyGoodsList(QString)));
    connect(win_cabinet, SIGNAL(checkLockState()), ctrlUi, SLOT(getLockState()));
    connect(win_cabinet, SIGNAL(requireOpenCase(int,int)), ctrlUi, SLOT(openCase(int,int)));
    connect(win_cabinet, SIGNAL(winSwitch(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(win_cabinet, SIGNAL(requireUserCheck(QString)), cabServer, SLOT(userLogin(QString)));
    connect(win_cabinet, SIGNAL(requireGoodsListCheck(QString)), cabServer, SLOT(listCheck(QString)));
    connect(win_cabinet, SIGNAL(requireCaseBind(int,int,QString)), cabServer, SLOT(cabinetBind(int,int,QString)));
    connect(win_cabinet, SIGNAL(requireCaseRebind(int,int,QString)), cabServer, SLOT(cabinetRebind(int,int,QString)));
    connect(win_cabinet, SIGNAL(goodsAccess(QPoint,QString,int,int)), cabServer, SLOT(goodsAccess(QPoint,QString,int,int)));
    connect(win_cabinet, SIGNAL(requireAccessList(QStringList,int)), cabServer, SLOT(listAccess(QStringList,int)));
    connect(win_cabinet, SIGNAL(checkCase(QList<CabinetCheckItem*>,CaseAddress)), cabServer, SLOT(goodsCheck(QList<CabinetCheckItem*>,CaseAddress)));
    connect(win_cabinet, SIGNAL(checkCase(QStringList,CaseAddress)), cabServer, SLOT(goodsCheck(QStringList,CaseAddress)));
    connect(win_cabinet, SIGNAL(storeList(QList<CabinetStoreListItem*>)), cabServer, SLOT(goodsListStore(QList<CabinetStoreListItem*>)));
    connect(win_cabinet, SIGNAL(requireCabSync()), cabServer, SLOT(cabInfoSync()));
    connect(win_cabinet, SIGNAL(requireGoodsCheck()), cabServer, SLOT(goodsCheckReq()));
    connect(win_cabinet, SIGNAL(goodsCheckFinish()), cabServer, SLOT(goodsCheckFinish()));
    connect(win_cabinet, SIGNAL(tsCalReq()), win_cab_service, SLOT(tsCalibration()));
    connect(win_cabinet, SIGNAL(requireCheckShow()), win_check_table, SLOT(show()));
    connect(win_cabinet, SIGNAL(requireApplyShow()), win_goods_apply, SLOT(show()));
    connect(cabServer, SIGNAL(checkCreatRst(bool, QString)), win_cabinet, SLOT(recvCheckCreatRst(bool, QString)));
    connect(cabServer, SIGNAL(checkFinishRst(bool, QString)), win_cabinet, SLOT(recvCheckFinishRst(bool, QString)));
    connect(cabServer, SIGNAL(cabSyncResult(bool)), win_cabinet, SLOT(recvCabSyncResult(bool)));
    connect(cabServer, SIGNAL(loginRst(UserInfo*)), win_cabinet, SLOT(recvUserCheckRst(UserInfo*)));
    connect(cabServer, SIGNAL(listRst(GoodsList*)), win_cabinet, SLOT(recvListInfo(GoodsList*)));
    connect(cabServer, SIGNAL(bindRst(bool)), win_cabinet, SLOT(recvBindRst(bool)));
    connect(cabServer, SIGNAL(goodsNumChanged(QString,int)), win_cabinet, SLOT(recvGoodsNumInfo(QString,int)));
    connect(cabServer, SIGNAL(accessFailed(QString)), win_cabinet, SLOT(accessFailedMsg(QString)));
    connect(cabServer, SIGNAL(accessSuccess(QString)), win_cabinet, SLOT(accessSuccessMsg(QString)));
    connect(cabServer, SIGNAL(updateGoodsPrice(float,float)), win_cabinet, SLOT(updateFetchPrice(float,float)));
    connect(cabServer, SIGNAL(timeUpdate()), win_cabinet, SLOT(updateTime()));
    connect(cabServer, SIGNAL(idUpdate()), win_cabinet, SLOT(updateId()));
    connect(cabServer, SIGNAL(goodsCheckRst(QString)), win_cabinet, SLOT(recvGoodsCheckRst(QString)));
    connect(cabServer, SIGNAL(newGoodsList(QString,QString)), win_cabinet, SLOT(newGoodsList(QString,QString)));
    connect(cabServer, SIGNAL(netState(bool)), win_cabinet, SLOT(updateNetState(bool)));
    connect(cabServer, SIGNAL(sysLock()), win_cabinet, SLOT(sysLock()));
    connect(cabServer, SIGNAL(cabPanelChanged()), win_cabinet, SLOT(cabinetInit()));
}

void MainWidget::aio_connect_mode(bool con)
{
    if(con)
    {
        connect(ctrlUi, SIGNAL(codeScanData(QByteArray)), win_aio, SLOT(recvScanData(QByteArray)), Qt::UniqueConnection);
        connect(ctrlUi, SIGNAL(cardReaderData(QByteArray)), win_aio, SLOT(recvUserInfo(QByteArray)),Qt::UniqueConnection);
    }
    else
    {
        disconnect(ctrlUi, SIGNAL(codeScanData(QByteArray)), win_aio, SLOT(recvScanData(QByteArray)));
        disconnect(ctrlUi, SIGNAL(cardReaderData(QByteArray)), win_aio, SLOT(recvUserInfo(QByteArray)));
    }
}

void MainWidget::cab_connect_mode(bool con)
{
    if(con)
    {
        connect(ctrlUi, SIGNAL(codeScanData(QByteArray)), win_cabinet, SLOT(recvScanData(QByteArray)), Qt::UniqueConnection);
        connect(ctrlUi, SIGNAL(cardReaderData(QByteArray)), win_cabinet, SLOT(recvUserInfo(QByteArray)),Qt::UniqueConnection);
    }
    else
    {
        disconnect(ctrlUi, SIGNAL(codeScanData(QByteArray)), win_cabinet, SLOT(recvScanData(QByteArray)));
        disconnect(ctrlUi, SIGNAL(cardReaderData(QByteArray)), win_cabinet, SLOT(recvUserInfo(QByteArray)));
    }
}

MainWidget::~MainWidget()
{
//    delete win_fingerPrint;
    delete ui;
    //--写入配置信息
//    writeSettings();
}

void MainWidget::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

//void MainWidget::on_stackedWidget_currentChanged(int arg1)
//{
//    qDebug()<<"[Current main stack]"<<arg1;
//    if(arg1 == INDEX_AIO)
//    {
//        aio_connect_mode(true);
//        cab_connect_mode(false);
//    }
//    else if(arg1 == INDEX_CAB_SHOW)
//    {
//        aio_connect_mode(false);
//        cab_connect_mode(true);
//    }
//}

void MainWidget::on_stackedWidget_currentChanged(int arg1)
{
    if((arg1 == INDEX_AIO) && win_aio == NULL)
    {
        win_aio = new AIOMachine(this);
        connect(win_aio, SIGNAL(requireOpenLock(int,int)), ctrlUi, SLOT(openLock(int,int)));
        connect(win_aio, SIGNAL(requireUserCheck(QString)), cabServer, SLOT(userLogin(QString)));
        connect(win_aio, SIGNAL(stack_switch(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
        connect(win_aio, SIGNAL(aio_check(bool)), win_cabinet, SLOT(on_check_clicked(bool)));
        connect(win_aio, SIGNAL(service_show()), win_cab_service, SLOT(show()));
        connect(cabServer, SIGNAL(loginRst(UserInfo*)), win_aio, SLOT(recvUserCheckRst(UserInfo*)));
        connect(cabServer, SIGNAL(sysLock()), win_aio, SLOT(sysLock()));
        connect(win_aio, SIGNAL(logout()), win_cabinet, SLOT(sysLock()));
        connect(win_aio, SIGNAL(reqUpdateOverview()), cabServer, SLOT(requireAioOverview()));
        connect(cabServer, SIGNAL(aioOverview(QString,AIOOverview*)), win_aio, SLOT(recvAioOverview(QString,AIOOverview*)));
        connect(win_aio, SIGNAL(click_event(int)), cabServer, SLOT(requireAioData(int)));
        connect(cabServer, SIGNAL(aioData(QString,AIOMachine::cEvent,QList<Goods*>)), win_aio, SLOT(recvAioData(QString,AIOMachine::cEvent,QList<Goods*>)));
        connect(win_aio, SIGNAL(tsCalReq()), win_cab_service, SLOT(tsCalibration()));
        connect(win_aio, SIGNAL(cabinetStateChange(CabState)), win_cabinet, SLOT(switchCabinetState(CabState)));
        connect(tempDev, SIGNAL(updateHumString(QString)), win_aio, SLOT(updateHum(QString)));
        connect(tempDev, SIGNAL(updateTempString(QString)), win_aio, SLOT(updateTemp(QString)));
        //    connect(win_aio, SIGNAL(aio_fetch(int,int)), win_cabinet, SLOT(caseClicked(int,int)));
        //    connect(win_aio, SIGNAL(aio_return(bool)), win_cabinet, SLOT(on_refund_clicked(bool)));
        ui->page_2->layout()->addWidget(win_aio);
        //        win_aio->sysLock();
    }
}
