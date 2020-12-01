#include "aiomachine.h"
#include "ui_aiomachine.h"
#include <QDebug>
#include <QTimer>
#include <iconfont/iconhelper.h>
#include "funcs/systool.h"
#include "defines.h"
#define MAX_TABLE_ROW 100

AIOMachine::AIOMachine(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AIOMachine)
{
    ui->setupUi(this);
    initNumLabel();
    initColMap();
    initIcons();
    ui->setting->hide();
    winActive = true;
    QWidget::installEventFilter(this);
    initStateMap();
    curState = 0;
    tempHub = new TempDevHub(this);
    tempHub->setObjectName("tempHub");
    connect(tempHub, SIGNAL(deviceListUpdate()), this, SLOT(updateTempDev()));
    connect(tempHub, SIGNAL(tempDevClicked(TempCase*)), this, SLOT(tempDevClicked(TempCase*)));

    win_rfid = new FrmRfid();
    connect(this, SIGNAL(requireRfidCheck()), win_rfid, SLOT(rfidCheck()));
    connect(win_rfid, SIGNAL(requireSysLock()), this, SLOT(on_aio_quit_clicked()));

//    win_access = new CabinetAccess();
//    connect(win_access, SIGNAL(saveStore(Goods*,int)), this, SLOT(saveStore(Goods*,int)));
//    connect(win_access, SIGNAL(saveFetch(QString,int)), this, SLOT(saveFetch(QString,int)));
//    connect(this, SIGNAL(goodsNumChanged(int)), win_access, SLOT(recvOptGoodsNum(int)));

    win_access = new CabinetAccess;
    connect(win_access, SIGNAL(saveStore(Goods*,int)), this, SLOT(saveStore(Goods*,int)));
    connect(win_access, SIGNAL(saveFetch(QString,int)), this, SLOT(saveFetch(QString,int)));
    connect(this, SIGNAL(goodsNumChanged(int)), win_access, SLOT(recvOptGoodsNum(int)));

//    win_fingerPrint = new FingerPrint(this);
//    connect(win_fingerPrint, SIGNAL(requireOpenLock(int,int)), this, SIGNAL(requireOpenLock(int,int)));

//    ui->page_overview->setWindowOpacity(1);
    ui->page_overview->setAttribute(Qt::WA_TranslucentBackground);
//    ui->page_table->setWindowOpacity(1);
    ui->page_table->setAttribute(Qt::WA_TranslucentBackground);
    ui->page_temp->setAttribute(Qt::WA_TranslucentBackground);

    ui->aio_check_in->hide();
    ui->aio_return->hide();
    ui->aio_check_create->hide();
//    ui->aio_check->hide();
    ui->frame_dev_panel->hide();
    ui->aio_store->hide();
    ui->aio_fetch->hide();
    ui->aio_back->hide();
//    ui->info_table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    loginState = false;
    optUser = NULL;
    sysTime = NULL;
    config = CabinetConfig::config();
    setAioInfo(config->getDepartName(), config->getCabinetId());
    config->msgLab = ui->msg;
    if(config->getCabinetType().at(BIT_CAB_AIO))
    {
        sysTime = new QTimer(this);
        connect(sysTime, SIGNAL(timeout()), this, SLOT(updateTime()));
        sysTime->start(1000);
    }
    ui->stackedWidget->setCurrentIndex(0);
    sysLock();
    initAioMode();
    ui->aio_external->hide();
//    timerCheckDev = startTimer(10000);
}

AIOMachine::~AIOMachine()
{
    win_rfid->deleteLater();
//    win_access->deleteLater();
    delete ui;
}

void AIOMachine::showEvent(QShowEvent *)
{
    setAioInfo(config->getDepartName(), config->getCabinetId());
    qDebug("show req");
    emit reqUpdateOverview();
    if((config->getCabinetType().at(BIT_CAB_AIO)) && (sysTime == NULL))
    {
        sysTime = new QTimer(this);
        connect(sysTime, SIGNAL(timeout()), this, SLOT(updateTime()));
        sysTime->start(1000);
    }
}

void AIOMachine::magicCmd(QString cmd)
{
    if(cmd == QString(MAGIC_CAL))
        emit tsCalReq();
    if(cmd == QString(MAGIC_SHOT))
        SysTool::singleShot();
}

//unuse
void AIOMachine::recvScanData(QByteArray qba)
{
    if(loginState == false)
        return;

    if((qba.indexOf("CK") == 0) && (qba.indexOf("-") == -1))//存货单
    {
        return;
    }

//    qDebug()<<"rfid"<<config->getCabinetType().at(BIT_RFID);
    if(config->getCabinetType().at(BIT_RFID))
        win_rfid->scanData(qba);
//    qDebug()<<config->state;

    switch(config->state)
    {
    case STATE_FETCH:

        break;
    case STATE_REFUN:

        break;
    case STATE_STORE:

        break;
    default:
        break;
    }
}

void AIOMachine::setPowState(int power)
{
    ui->aio_day_report->hide();//日清单
    ui->aio_check->hide();//盘点单
    ui->aio_check_create->hide();//盘点
    ui->aio_return->hide();//退货
    ui->aio_check_in->hide();//消耗登记
    ui->aio_store->hide();//存货
    ui->aio_fetch->hide();//取货
    ui->aio_back->hide();//还货
    ui->setting->hide();
    curState = 0;

    switch(power)
    {
    case 0://超级管理员:|补货|退货|服务|退出|
        ui->aio_day_report->show();
        ui->setting->show();
        ui->aio_store->show();//存货
        ui->aio_fetch->show();//取货
        ui->aio_back->show();//还货
        setState(STATE_FETCH);
//        ui->aio_check->show();
//        ui->aio_return->show();
//        ui->aio_check_create->show();
        break;

    case 1://护士长:|退货|退出|
        ui->aio_day_report->show();
        ui->aio_fetch->show();//取货
        ui->aio_back->show();//还货
        setState(STATE_FETCH);
//        ui->aio_check->show();
//        ui->aio_return->show();
//        ui->aio_check_create->show();
        break;

    case 2://护士:|退出|
        ui->aio_day_report->show();
        ui->aio_fetch->show();//取货
        ui->aio_back->show();//还货
        setState(STATE_FETCH);
//        ui->aio_check->show();
        break;

    case 3://管理员:|补货|退货|退出|
        ui->aio_day_report->show();
        ui->aio_store->show();//存货
        setState(STATE_STORE);
//        ui->aio_check->show();
//        ui->aio_return->show();
//        ui->aio_check_create->show();
        break;

    case 4://医院员工:|退出|
        ui->aio_day_report->show();
        ui->aio_fetch->show();//取货
        ui->aio_back->show();//还货
        setState(STATE_FETCH);
//        ui->aio_check->show();
//        ui->aio_return->show();
//        ui->aio_check_create->show();
        break;

    default:
        break;
    }
}

void AIOMachine::recvUserCheckRst(UserInfo *user)
{
    if(user == NULL)
        return;

    ui->stacked_hello->setCurrentWidget(ui->page_hello);
    emit updateLoginState(true);
    optUser = user;
    win_rfid->updateCurUser(optUser->name);
    win_rfid->setPow(optUser->power);
    loginState = true;
    if(config->getDepartName().isEmpty())
    {
        qDebug()<<user->departName;
        config->setDepartName(user->departName);
        setAioInfo(config->getDepartName(), config->getCabinetId());
    }

    ui->aio_quit->setText(QString("您好！%1").arg(user->name));
//    config->state = STATE_FETCH;
    setPowState(optUser->power);
//    updateState();
    sysUnlock();
    win_rfid->setLoginState(true);
    qDebug("user req");
    emit reqUpdateOverview();
//    win_rfid->showFullScreen();
}

void AIOMachine::recvUserInfo(QByteArray qba)
{
    if(loginState == true)
    {
        if(optUser->cardId == QString(qba))
            return;
    }
//    ui->frame_quit->show();
//    ui->aio_quit->show();
    qDebug()<<"[AIOMachine] recvUserInfo:"<<qba;
    ui->lab_tip->setText("正在识别");
    emit requireUserCheck(QString(qba));
    QTimer::singleShot(5000, this, SLOT(loginTimeout()));
}

void AIOMachine::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

bool AIOMachine::eventFilter(QObject *obj, QEvent *e)
{
    if(obj == this)
    {
        if(e->type() == QEvent::WindowActivate)
        {
            if(config->state == STATE_CHECK)//如果在盘点状态就结束盘点
            {
                emit aio_check(false);
            }
//            if((!winActive) && (loginState))//窗口被激活，排除初始显示被激活的情况,排除未登录情况
//            {
//                updateState();
//            }
            winActive = true;
        }
        else if(e->type() == QEvent::WindowDeactivate)
        {
            winActive = false;
        }
    }
    if(e->type() == QEvent::MouseButtonRelease)
    {
        if(loginState)
        {
            cEvent eventNum = (cEvent)l_num_label.indexOf((QLabel*)obj);
            if(eventNum>=0)
            {
                qDebug()<<"[click event]"<<eventNum;
                emit click_event(eventNum);
                recvClickEvent(eventNum);
            }
        }
    }
    return QWidget::eventFilter(obj, e);
}

void AIOMachine::initNumLabel()
{
    if(!l_num_label.isEmpty())
    {
        qDeleteAll(l_num_label.begin(), l_num_label.end());
        l_num_label.clear();
    }

    l_num_label<<ui->num_expired;
    l_num_label<<ui->num_goods;
    l_num_label<<ui->num_today_in;
    l_num_label<<ui->num_today_out;
    l_num_label<<ui->num_warning_rep;
    l_num_label<<ui->lab_temp;
    l_num_label<<ui->lab_hum;

    ui->num_expired->installEventFilter(this);
    ui->num_goods->installEventFilter(this);
    ui->num_today_in->installEventFilter(this);
    ui->num_today_out->installEventFilter(this);
    ui->num_warning_rep->installEventFilter(this);
    ui->lab_temp->installEventFilter(this);
    ui->lab_hum->installEventFilter(this);
}

void AIOMachine::initColMap()
{
    if(!mapColName.isEmpty())
        mapColName.clear();

    mapColName.insert("物品编码", goodsId);
    mapColName.insert("物品名称", goodsName);
    mapColName.insert("包类型", packageType);
    mapColName.insert("生产商", proName);
    mapColName.insert("供应商", supplyName);
    mapColName.insert("规格", goodsSize);
    mapColName.insert("单位", unit);
    mapColName.insert("预警数量", threshold);
    mapColName.insert("最大数量", maxThreshold);
    mapColName.insert("包数", packageCount);
    mapColName.insert("耗材数量", goodsCount);
    mapColName.insert("效期天数", lifeDay);
    mapColName.insert("有效期至", lifeTime);
    mapColName.insert("生产日期", productTime);
    mapColName.insert("单价", price);
    mapColName.insert("总价", sumCount);
    mapColName.insert("入库数", aioInNum);
    mapColName.insert("出库数", aioOutNum);
    mapColName.insert("操作人", optName);
    mapColName.insert("操作时间", optTime);
    mapColName.insert("批次", batchNumber);
    mapColName.insert("条码", traceId);
    listColName<<"物品编码"
              <<"物品名称"
             <<"包类型"
            <<"生产商"
           <<"供应商"
          <<"规格"
         <<"单位"
        <<"预警数量"
       <<"最大数量"
      <<"包数"
     <<"耗材数量"
    <<"效期天数"
    <<"有效期至"
    <<"生产日期"
    <<"单价"
    <<"总价"
    <<"入库数"
    <<"出库数"
    <<"操作人"
    <<"操作时间"
    <<"批次"
    <<"条码";
    //    qDebug()<<"[listColName]"<<listColName;
}

//1取货2存货3退货16还货
void AIOMachine::initStateMap()
{
    curStateText.clear();
    curStateText.insert("取", 2);
    curStateText.insert("存", 1);
    curStateText.insert("退", 3);
    curStateText.insert("还", 10);
}

void AIOMachine::initIcons()
{
//    ui->setting->setIconSize(ui->setting->size());
    //    ui->setting->setIcon(IconHelper::Instance()->GetIcon(ui->setting->rect().size(),0xf013,30,"#AEB035"));
}

void AIOMachine::initAioMode()
{
    QString mode = config->getAioMode();
    if(mode == "temp_view")
    {
        ui->mode_temp->setChecked(true);
    }
}

void AIOMachine::setState(CabState state)
{
    curState = state;
    config->state = state;
    switch (state) {
    case STATE_STORE:
        win_rfid->setScene(mark_new);
        break;

    case STATE_FETCH:
        win_rfid->setScene(mark_out);
        break;

    case STATE_BACK:
        win_rfid->setScene(mark_back);
        break;

//    case STATE_AUTO:
//        win_rfid->setScene(mark_all);
//        break;

    default:
        win_rfid->setScene(mark_out);
        break;
    }

    qDebug()<<"[updateState]"<<state<<config->state;
}

void AIOMachine::setAioInfo(QString departName, QString departId)
{
    ui->aio_info->setText(QString("%1 ID:%2").arg(departName).arg(departId));
}

void AIOMachine::setNumLabel(AIOOverview *overview)
{
    if(loginState)
    {
        ui->num_expired->setText(QString("%1\n近效期物品").arg(overview->lifeTimeGoodsCount));
        ui->num_goods->setText(QString("%1\n耗材品种（个）").arg(overview->chesetGoodsCount));
        ui->num_today_in->setText(QString("￥%1\n今日入库(元)").arg(overview->inSumCount));
        ui->num_today_out->setText(QString("￥%1\n今日出库(元)").arg(overview->outSumCount));
        ui->num_warning_rep->setText(QString("%1\n库存预警").arg(overview->warnGoodsCount));
    }
    else
    {
        ui->num_expired->setText(QString("%1\n近效期物品").arg(overview->lifeTimeGoodsCount));
        ui->num_goods->setText(QString("%1\n耗材品种（个）").arg(overview->chesetGoodsCount));
        ui->num_today_in->setText(QString("￥---\n今日入库(元)"));
        ui->num_today_out->setText(QString("￥---\n今日出库(元)"));
        ui->num_warning_rep->setText(QString("%1\n库存预警").arg(overview->warnGoodsCount));
    }

}

void AIOMachine::showTable(QString title, QStringList colNames, QList<Goods*>listInfo)
{
//    qDebug()<<"[showTable]"<<colNames;
    if(!cur_list.isEmpty())
    {
        qDeleteAll(cur_list.begin(), cur_list.end());
        cur_list.clear();
    }
    cur_list = listInfo;

    ui->stackedWidget->setCurrentIndex(1);
    ui->tab_title->setText(title);
    curPage = 0;
    int rowCount = listInfo.count()>MAX_TABLE_ROW?MAX_TABLE_ROW:listInfo.count();
    int colCount = colNames.count();
    ui->info_table->clear();
    ui->info_table->setRowCount(rowCount);
    ui->info_table->setColumnCount(colCount);
    ui->info_table->setHorizontalHeaderLabels(colNames);
    ui->info_table->setAlternatingRowColors(true);
    int colIndex = 0;
    int rowIndex = 0;

    QList<Goods*> showList = listPage(curPage);
    foreach (Goods* info, showList)
    {
        colIndex = 0;
        foreach (QString col, colNames)
        {
            ui->info_table->setItem(rowIndex, colIndex, new QTableWidgetItem(getGoodsInfoText(info, col)));
            colIndex++;
        }
        rowIndex++;
        if(rowIndex >= rowCount)
            break;
    }
    ui->info_table->resizeColumnsToContents();
}

void AIOMachine::showNumExpired(QList<Goods *> lInfo)
{
    QString title = "近效期物品";
    QStringList colNames;
    colNames<<listColName.at(goodsId);
    colNames<<listColName.at(goodsName);
    colNames<<listColName.at(batchNumber);
    colNames<<listColName.at(goodsSize);
    colNames<<listColName.at(unit);
//    colNames<<listColName.at(productTime);
    colNames<<listColName.at(supplyName);
    colNames<<listColName.at(packageType);
    colNames<<listColName.at(lifeDay);
    colNames<<listColName.at(productTime);
    colNames<<listColName.at(lifeTime);
    showTable(title, colNames, lInfo);
}

void AIOMachine::showNumGoods(QList<Goods *> lInfo)
{
    QString title = "库存物品";
    QStringList colNames;
    colNames<<listColName.at(goodsId);
    colNames<<listColName.at(goodsName);
    colNames<<listColName.at(goodsSize);
    colNames<<listColName.at(proName);
    colNames<<listColName.at(supplyName);
    colNames<<listColName.at(packageType);
    colNames<<listColName.at(unit);
    colNames<<listColName.at(threshold);
    colNames<<listColName.at(maxThreshold);
    colNames<<listColName.at(packageCount);
    colNames<<listColName.at(goodsCount);
    showTable(title, colNames, lInfo);
}

void AIOMachine::showNumTodayIn(QList<Goods *> lInfo)
{
    QString title = "今日入库";
    QStringList colNames;
    colNames<<listColName.at(goodsId);
    colNames<<listColName.at(goodsName);
    colNames<<listColName.at(goodsSize);
    colNames<<listColName.at(unit);
    colNames<<listColName.at(proName);
    colNames<<listColName.at(supplyName);
    colNames<<listColName.at(packageType);
    colNames<<listColName.at(aioInNum);
    colNames<<listColName.at(price);
    colNames<<listColName.at(sumCount);
    showTable(title, colNames, lInfo);
}

void AIOMachine::showNumTodayOut(QList<Goods *> lInfo)
{
    QString title = "今日出库";
    QStringList colNames;
    colNames<<listColName.at(goodsId);
    colNames<<listColName.at(goodsName);
    colNames<<listColName.at(goodsSize);
    colNames<<listColName.at(unit);
    colNames<<listColName.at(batchNumber);
    colNames<<listColName.at(traceId);
    colNames<<listColName.at(proName);
    colNames<<listColName.at(supplyName);
    colNames<<listColName.at(goodsCount);
    colNames<<listColName.at(price);
    colNames<<listColName.at(sumCount);
    colNames<<listColName.at(optName);
    colNames<<listColName.at(optTime);
    showTable(title, colNames, lInfo);
}

void AIOMachine::showNumWarningRep(QList<Goods *> lInfo)
{
    QString title = "库存预警";
    QStringList colNames;
    colNames<<listColName.at(goodsId);
    colNames<<listColName.at(goodsName);
    colNames<<listColName.at(goodsSize);
    colNames<<listColName.at(unit);
    colNames<<listColName.at(proName);
    colNames<<listColName.at(packageType);
    colNames<<listColName.at(packageCount);
    colNames<<listColName.at(goodsCount);
    colNames<<listColName.at(threshold);
    showTable(title, colNames, lInfo);
}

QString AIOMachine::getGoodsInfoText(Goods *info, QString key)
{
    colMark mark = mapColName.value(key, unknow);
//    qDebug()<<mark<<key<<info->productTime;

    switch(mark)
    {
    case goodsId:return info->goodsId;//物品编码
    case goodsName:return info->name;//物品名称
    case packageType:return QString("%1").arg(info->packageType);//包类型
    case proName:return info->proName;//生产商
    case supplyName:return info->supName;//供应商
    case goodsSize:return info->size;//规格
    case unit:return info->unit;//单位
    case threshold:return QString("%1").arg(info->threshold);//预警数量
    case maxThreshold:return QString("%1").arg(info->maxThreshold);//最大数量
    case packageCount:return QString("%1").arg(info->packageCount);
    case goodsCount:return QString("%1").arg(info->goodsCount);
    case lifeDay:return QString("%1").arg(info->lifeDay);
    case lifeTime:return info->lifeTime;
    case productTime:return info->productTime;
    case price:return QString("%1").arg(info->price);
    case sumCount:return QString("%1").arg(info->sumCount);
    case aioInNum:return info->aioInNum;
    case aioOutNum:return info->aioOutNum;
    case optName:return info->optName;
    case optTime:return info->optTime;
    case batchNumber:return info->batch;
    case traceId:return info->traceId;

    default:
        break;
    }
    return QString();
}

QList<Goods *> AIOMachine::listPage(unsigned int pageNum)
{
    int firstIndex = pageNum * MAX_TABLE_ROW;
    int pageLen = (cur_list.count() - firstIndex);
    pageLen = pageLen>MAX_TABLE_ROW ? MAX_TABLE_ROW:pageLen;

    return cur_list.mid(firstIndex, pageLen);
}

void AIOMachine::sysLock()
{
    ui->stacked_footer->setCurrentIndex(1);
    ui->lab_tip->setText("请将识别卡放入下方感应区");
    ui->aio_quit->setText("");
    ui->stacked_hello->setCurrentWidget(ui->page_scan_tip);
    win_rfid->close();
    ui->frame_aio->hide();
    ui->setting->hide();
    loginState = false;
    win_rfid->setLoginState(false);
    optUser = NULL;
    win_rfid->updateCurUser(QString());
    qDebug("lock req");
    ui->num_today_in->setText(QString("￥---\n今日入库(元)"));
    ui->num_today_out->setText(QString("￥---\n今日出库(元)"));

    emit reqUpdateOverview();
    emit updateLoginState(false);
    emit logout();

    //    ui->frame_quit->hide();
}

void AIOMachine::recvAioOverview(QString msg, AIOOverview *overview)
{
    if(overview == NULL)
    {
        qDebug()<<"[AioOverview]"<<msg;
        return;
    }
    setNumLabel(overview);
    delete overview;
}

void AIOMachine::recvAioData(QString msg, AIOMachine::cEvent e, QList<Goods *> lInfo)
{
    if(lInfo.isEmpty())//接口调用失败
    {
        qDebug()<<"[AIOMachine]recvAioData failed:"<<msg;
        return;
    }
    switch(e)
    {
    case AIOMachine::click_lab_hum: break;
    case AIOMachine::click_lab_temp : break;
//        showLabTemp(lInfo);break;
    case AIOMachine::click_num_expired:
        showNumExpired(lInfo); break;
    case AIOMachine::click_num_goods :
        showNumGoods(lInfo);break;
    case AIOMachine::click_num_today_in :
        showNumTodayIn(lInfo);break;
    case AIOMachine::click_num_today_out :
        showNumTodayOut(lInfo);break;
    case AIOMachine::click_num_warning_rep :
        showNumWarningRep(lInfo);break;
    default:
        break;
    }
}

//
void AIOMachine::recvClickEvent(AIOMachine::cEvent e)
{
    if(!loginState)
        return;

    switch(e)
    {
    case AIOMachine::click_lab_temp:
//        ui->stackedWidget->setCurrentIndex(2);
        break;
    default:
        break;
    }
}

void AIOMachine::updateTemp(QString temp)
{
    ui->lab_temp->setText(QString("%1\n当前温度").arg(temp));
}

void AIOMachine::updateHum(QString hum)
{
    ui->lab_hum->setText(QString("%1\n当前湿度").arg(hum));
}

void AIOMachine::winMsg(QString msg)
{
    ui->msg->setText(msg);
    QTimer::singleShot(4000, ui->msg, SLOT(clear()));
}

void AIOMachine::updateDelay(int delay)
{
//    qDebug()<<"update delay"<<delay;
    updateNetState(delay);
//    ui->delay->setText(QString("%1ms").arg(delay));
//    config->netState = ((delay<1000) && (delay>0));
//    ui->delay->setChecked(config->netState);
}

void AIOMachine::recvCabSyncResult(bool rst)
{
    if(rst)
        winMsg("智能柜数据同步成功");
    else
        winMsg("智能柜数据同步失败");
}

void AIOMachine::updateNetState(bool connected)
{
//    qDebug()<<"AIOMachine::updateNetState"<<connected;
    netCheckState = connected;
    ui->netState->setChecked(netCheckState);
}

void AIOMachine::sysUnlock()
{
    ui->frame_aio->show();
    ui->stacked_hello->setCurrentWidget(ui->page_hello);
    ui->stacked_footer->setCurrentIndex(0);
    //    ui->frame_quit->show();
}

void AIOMachine::loginTimeout()
{
    if(loginState == false)
    {
        ui->lab_tip->setText("请将识别卡放入下方感应区");
//        ui->aio_quit->clear();
    }
}

void AIOMachine::updateTime()
{
    ui->aio_time->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    if((QDateTime::currentDateTime().time().hour() == 4) && (QTime::currentTime().minute() == 0))
    {
        qDebug("[update time]");
        emit reqCheckVersion(false);
    }
}

void AIOMachine::updateTempDev()
{
    QList<TempCase*> devs = tempHub->devList();
    //移除原来的控件
    foreach (TempCase* c, ui->layout_temp->findChildren<TempCase*>())
    {
        ui->layout_temp->removeWidget(c);
    }

    int i=0;
    foreach (TempCase* dev, devs)
    {
        ui->layout_temp->addWidget(dev, i/8, i%8, 1,1);
        i++;
    }
}

void AIOMachine::tempDevClicked(TempCase* dev)
{
    selectedDev = dev;
    ui->frame_dev_panel->show();
    ui->set_dev_name->setText(selectedDev->devName());
    ui->set_temp_max->setText(QString::number(selectedDev->maxTemp()));
    ui->set_temp_min->setText(QString::number(selectedDev->minTemp()));
    ui->set_report_time->setText(QString::number(selectedDev->reportTime()));
    ui->set_sound_off->setChecked(selectedDev->SoundOff());
//    ui->set_report_time->setText(selectedDev->tReport);
}

void AIOMachine::checkTempDev()
{
    foreach (TempCase* dev, tempHub->devList())
    {
        dev->checkOverTime();
    }
}

void AIOMachine::on_aio_quit_clicked()
{
    sysLock();

}

//void AIOMachine::timerEvent(QTimerEvent *event)
//{
//    int timerId = event->timerId();
//    if(timerId == timerCheckDev)
//    {
//        checkTempDev();
//    }
//}

void AIOMachine::on_tab_back_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

//void AIOMachine::on_aio_fetch_clicked()
//{
//    emit cabinetStateChange(STATE_FETCH);
//}

void AIOMachine::on_aio_return_clicked()
{
    emit cabinetStateChange(STATE_REFUN);
//    emit aio_return(true);
//    emit stack_switch(INDEX_CAB_SHOW);
//    emit aio_fetch(0, 0);//模拟一次点击
}

void AIOMachine::on_aio_check_clicked()
{
    emit cabinetStateChange(CMD_CHECK_SHOW);
}

void AIOMachine::on_aio_day_report_clicked()
{
    emit cabinetStateChange(CMD_DAY_REPORT_SHOW);
}

void AIOMachine::on_aio_check_create_clicked()
{
//    emit aio_check(true);
    win_rfid->setDownCount(0);
    emit requireRfidCheck();
}

void AIOMachine::on_tab_last_clicked()
{

}

void AIOMachine::on_tab_next_clicked()
{

}

void AIOMachine::on_setting_clicked()
{
    emit stack_switch(INDEX_CAB_SERVICE);
}

void AIOMachine::on_temp_btn_back_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void AIOMachine::on_mode_view_toggled(bool checked)
{
    if(checked)
    {
        config->setAioMode("goods_view");
        ui->stackedWidget->setCurrentIndex(0);
    }
    else
    {

    }
}

void AIOMachine::on_mode_temp_toggled(bool checked)
{
    if(checked)
    {
        config->setAioMode("temp_view");
        ui->stackedWidget->setCurrentIndex(2);
    }
    else
    {

    }
}

void AIOMachine::on_set_dev_params_clicked()
{
    int max = ui->set_temp_max->text().toInt();
    int min = ui->set_temp_min->text().toInt();
    int report = ui->set_report_time->text().toInt();
    bool soundOff = ui->set_sound_off->isChecked();
    selectedDev->setTempParams(max, min, max, report, soundOff);
    ui->frame_dev_panel->hide();
}

void AIOMachine::on_netState_clicked()
{
//    ui->netState->setChecked(netCheckState);
    updateNetState(netCheckState);
    winMsg("智能柜数据同步中..");
    emit requireCabSync();
}

void AIOMachine::on_aio_store_clicked()
{
    setState(STATE_STORE);
    win_rfid->setDownCount(0);
    win_rfid->rfidCheck();
}

void AIOMachine::on_aio_fetch_clicked()
{
    setState(STATE_FETCH);
    win_rfid->setDownCount(0);
    win_rfid->rfidCheck();
}

void AIOMachine::on_aio_back_clicked()
{
    setState(STATE_BACK);
    win_rfid->setDownCount(0);
    win_rfid->rfidCheck();
}

void AIOMachine::on_aio_external_clicked()
{
    setState(STATE_CONSUME);
    win_rfid->setDownCount(0);
    win_rfid->rfidCheck();
}
