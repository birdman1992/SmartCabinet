#include "frmrfid.h"
#include "ui_frmrfid.h"
#include <QDebug>
#include <QByteArray>
#include "MessageDialog.h"
#include "manager/signalmanager.h"
//#define test_rfid


FrmRfid::FrmRfid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmRfid)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
//    btnTable.insert(mark_no, ui->tab_filter_unknow);
    btnTable.insert(mark_new, ui->tab_filter_new);
    btnTable.insert(mark_back, ui->tab_filter_back);
    btnTable.insert(mark_out, ui->tab_filter_out);
//    btnTable.insert(mark_con, ui->tab_filter_consume);
//    btnTable.insert(mark_in, ui->tab_filter_in);
//    btnTable.insert(mark_wait_back, ui->tab_filter_wait_back);
//    btnTable.insert(mark_all, ui->tab_filter_all);

    QRegExp ipRx("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    ui->input_addr->setValidator(new QRegExpValidator(ipRx));

    QRegExp portRx("^([0-9]|[1-9]\\d|[1-9]\\d{2}|[1-9]\\d{3}|[1-5]\\d{4}|6[0-4]\\d{3}|65[0-4]\\d{2}|655[0-2]\\d|6553[0-5])$");
    ui->input_port->setValidator(new QRegExpValidator(portRx));

    config = CabinetConfig::config();

    visibleFlag = QBitArray(mark_checked+1, false);
    visibleFlag[mark_all] = true;
    rfScene = QBitArray(mark_checked+1);

    downCount = 60;
    eModel = new EpcModel(this);
    eSumModel = eModel->getSumModel();
    isLogin = false;
    doorIsOpen = false;
    rfManager = new RfidManager(eModel);
    connect(rfManager, SIGNAL(updateTimer(int)), this, SLOT(updateScanTimer(int)));
    connect(rfManager, SIGNAL(optFinish()), this, SLOT(showEpcInfo()));

    SignalManager* sigMan = SignalManager::manager();
    connect(sigMan, SIGNAL(accessSuccess(QString)), this, SLOT(accessSuccess(QString)));
    connect(sigMan, SIGNAL(accessFailed(QString)), this, SLOT(accessFailed(QString)));
    connect(sigMan, SIGNAL(configRfidDevice()), this, SLOT(showConfigDevice()));
    connect(sigMan, SIGNAL(lockState(int,bool)), this, SLOT(lockStateChanged(int,bool)));

    connect(ui->frm_operation, SIGNAL(winClose()), this, SLOT(showEpcInfo()));
    connect(ui->frm_operation, SIGNAL(requireUpdate()), sigMan, SIGNAL(requireUpdateOperation()));
    connect(sigMan, SIGNAL(operationInfoUpdate()), ui->frm_operation, SLOT(loadOperations()));
    connect(ui->frm_operation, SIGNAL(curOperationStrChanged(QString)), this, SLOT(updateOperationStr(QString)));
    connect(ui->frm_operation, SIGNAL(CurOperationNoChanged(QString)), eModel, SLOT(curOptNoChanged(QString)));

//    connect(eModel, SIGNAL(epcAccess(QStringList,QStringList,QString)), sigMan, SIGNAL(epcAccess(QStringList,QStringList,QString)));
    initTabs();
    initAntList();

    if(!config->getCabinetType().at(BIT_LOW_HIGH))//低值柜没有手术单
    {
        ui->operation->hide();
//        ui->tab_check_out->hide();
//        ui->tab_day_list->hide();
//        ui->pushButton_3->hide();
    }

#ifdef test_rfid
    QTimer::singleShot(1000, this, SLOT(testSlot()));
#endif
}

void FrmRfid::setLoginState(bool login)
{
    isLogin = login;
}

FrmRfid::~FrmRfid()
{
    delete filterModel;
    delete ui;
}

void FrmRfid::updateScanTimer(int ms)
{
    ui->scan_timer->setText(QString::number(ms));
    if(this->isVisible() && (!doorIsOpen))//关门且在扫描状态
        accessDownCount(eModel->checkOptTime(downCount));
    //刷新信号强度
//    int index = ui->tab_details->verticalScrollBar()->value();
//    int countMax = ui->tab_details->verticalScrollBar()->pageStep()+1;
//    int updateRowCount = qMin(countMax, filterModel->rowCount());

//    qDebug()<<"update col"<<index<<updateRowCount;
//    emit dataChanged(filterModel->index(index,10), filterModel->index(updateRowCount, 10));
}

void FrmRfid::updateCount(EpcMark mark, int count)
{
//    if(mark == 0)
//        return;

//    qDebug()<<"updateCount:"<<mark<<count;
    if(btnTable.contains(mark))
    {
        btnTable[mark]->setText(btnTable[mark]->text().replace(QRegExp(":[0-9]*"), QString(":%1").arg(count)));
        btnTable[mark]->setVisible(count && (visibleFlag[mark]));//加入屏蔽掩膜

        if((!btnTable[mark]->isChecked()) && btnTable[mark]->isVisible())
        {
            btnTable[mark]->setChecked(true);
        }
//        btnTable[mark]->setVisible(count);//没有屏蔽掩膜
    }

//    qDebug()<<mark<<eSumModel->scene();
    updateCountInfo(mark);

    setDefaultSel();//设置默认选中按钮
}

void FrmRfid::setDefaultSel()
{
//    QMap<EpcMark, QToolButton*>::iterator itTab = btnTable.begin();
//    QToolButton* checkBtn = NULL;

//    for(itTab=btnTable.begin(); itTab!=btnTable.end();itTab++)
//    {
//        if(itTab.value()->isVisible())
//        {
//            checkBtn = itTab.value();
//            if(itTab.value()->isChecked())
//                return;
//        }
//    }
//    if(checkBtn)
//        checkBtn->setChecked(true);
    ui->tab_filter_out->setChecked(false);
}

void FrmRfid::updateSelReader(QString devIp)
{
    Q_UNUSED(devIp);
}

void FrmRfid::initAntList()
{
    listAntEnable.clear();
    listAntEnable<<ui->ant1
                <<ui->ant2
               <<ui->ant3
              <<ui->ant4
             <<ui->ant5
            <<ui->ant6
           <<ui->ant7
          <<ui->ant8;

    foreach (QCheckBox* b, listAntEnable)
    {
        connect(b, SIGNAL(toggled(bool)), this, SLOT(ant_state_changed(bool)));
    }
}

void FrmRfid::setScene(EpcMark mark)
{
    sceneMark = mark;
    rfScene = QBitArray(mark_checked+1);
    rfScene.setBit(mark, true);
    eSumModel->setScene(mark);
}

QBitArray FrmRfid::curAntState()
{
    QBitArray ret = QBitArray(8);

    for(int i=0; i<ret.count(); i++)
    {
        ret.setBit(i, listAntEnable.at(i)->isChecked());
    }
    qDebug()<<"curAntState:"<<ret;
    return ret;
}

void FrmRfid::accessDownCount(int count)
{
    if(downCount == 0)
    {
        ui->OK->setText(QString("确定"));
        return;
    }

    ui->OK->setText(QString("确定(%1)").arg(count));
//    qDebug()<<"[accessDownCount]"<<count<<ui->tab_filter_new->text();
    if(count == 0 && ((ui->tab_filter_new->text() == QString("存入:0")) || (!ui->tab_filter_new->isVisible())))//没有存入数量或者存入按钮不可见
        on_OK_clicked();
}

void FrmRfid::updateCountInfo(EpcMark scene)
{
    if(scene == eSumModel->scene())//
    {
        ui->lab_cur_scene->setText(QString("%1:").arg(eModel->markTab().at(scene)));
        ui->lab_count->setText(QString("%1").arg(filterModel->rowCount()));
        ui->lab_pac->setText(QString("%1").arg(eSumModel->rowCount()));
    }
}

void FrmRfid::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);
    qDebug("close event");
    clearCurOperation();
}

//void FrmRfid::updateOperationState()
//{
//    QString strOperation = ui->frm_operation->curOperation();
//    if(strOperation.isEmpty())
//    {
//        ui->operation->setChecked(true);
//        ui->operation->setText("请选择手术单");
//    }
//    else
//    {
//        ui->operation->setChecked(false);
//        ui->operation->setText(strOperation);
//    }
//}

void FrmRfid::updateCurUser(QString optId)
{
    rfManager->setCurOptId(optId);
}

void FrmRfid::scanProgress(int curCount, int totalCount)
{
    Q_UNUSED(totalCount);
//    ui->tab_filter_all->setText(QString("总览:%1").arg(totalCount));
    ui->count->setText(QString::number(curCount));
}

//unuse 2020-11-06
void FrmRfid::updateLockCount(int lockCount)
{
    return;
//    ui->lock_count->display(lockCount);
    if(lockCount>0 && !ui->tab_filter_out->isChecked())
    {
        ui->tab_filter_out->setChecked(true);
    }
}

void FrmRfid::updateUnknowCount(int unknowCount)
{
    ui->unknow_count->setText(QString::number(unknowCount));
}

void FrmRfid::showConfigDevice()
{
//    qDebug("configDevice");
    ui->stackedWidget->setCurrentIndex(0);
    showMaximized();
}

void FrmRfid::showEpcInfo()
{
    ui->stackedWidget->setCurrentIndex(1);
    showMaximized();
}

void FrmRfid::rfidCheck()
{
    showEpcInfo();
    rfManager->initEpc();
    rfManager->startScan();
    rfManager->doorCloseScan();
}

void FrmRfid::lockStateChanged(int id, bool isOpen)
{
    qDebug()<<"lockState:"<<id<<isOpen;
    doorIsOpen = isOpen;
    if(!isLogin)
    {
        if(doorIsOpen)
            MessageDialog::instance().showMessage("非法开启门禁！", 20);
        else
            MessageDialog::instance().showFinish();
        return;
    }

    if(isOpen)
    {
        qDebug()<<"[visible]"<<this->isVisible();
        if(this->isVisible())//未结算的情况下再次开门
        {
            MessageDialog::instance().showMessage("检测到开门，扫描暂停", 60);
            rfManager->setScanLock(true);
        }
    }
    else
    {
        setDownCount(60);
        MessageDialog::instance().showFinish();
        rfidCheck();
    }
}

void FrmRfid::showOperation()
{
    ui->stackedWidget->setCurrentIndex(2);
    showMaximized();
}

void FrmRfid::updateAntInCount(int count)
{
    ui->in_count->setText(QString::number(count));
}

void FrmRfid::updateOperationStr(QString optStr)
{
    if(optStr.isEmpty())
    {
        ui->operation->setText("请选择手术单");
    }
    else
    {
        ui->operation->setText(optStr);
    }

    ui->operation->setChecked(optStr.isEmpty());
}

void FrmRfid::updateAntState(RfidReader* dev)
{
    QBitArray state = dev->property("antState").toBitArray();

    for(int i=0; i<listAntEnable.count(); i++)
    {
        qDebug()<<state.at(i);
        listAntEnable[i]->setChecked(state.at(i));
    }
}

void FrmRfid::accessSuccess(QString msg)
{
    ui->msg->setText(msg);
    QTimer::singleShot(2000, this, SLOT(hide()));
}

void FrmRfid::accessFailed(QString msg)
{
    ui->msg->setText(msg);
}

void FrmRfid::on_scan_clicked()
{
    rfManager->startScan();
}

void FrmRfid::on_stop_clicked()
{
    rfManager->doorCloseScan();
}

//[物品|条码|RFID|规格|操作人|时间]
//enum TableMark
//{
//    tab_no = 0,
//    tab_in = 1,//放入表
//    tab_out = 2,//取出表
//    tab_back = 4,//还回表
//    tab_con = 8,//消耗表
//};
void FrmRfid::initTabs()
{
    filterModel = new QSortFilterProxyModel;
    filterModel->setSourceModel(eModel);
    filterModel->setDynamicSortFilter(true);
    connect(eModel, SIGNAL(updateCount(EpcMark,int)), this, SLOT(updateCount(EpcMark,int)));
    connect(eModel, SIGNAL(scanProgress(int,int)), this, SLOT(scanProgress(int,int)));
    connect(eModel, SIGNAL(updateLockCount(int)), this, SLOT(updateLockCount(int)));
    connect(eModel, SIGNAL(updateUnknowCount(int)), this, SLOT(updateUnknowCount(int)));
    connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex)), filterModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)));
    ui->tab_details->setModel(filterModel);
    ui->tab_details->setColumnWidth(0, 113);
    ui->tab_details->setColumnWidth(1, 109);
    ui->tab_details->setColumnWidth(2, 137);
    ui->tab_details->setColumnWidth(3, 210);
    ui->tab_details->setColumnWidth(4, 77);
    ui->tab_details->setColumnWidth(5, 228);
    ui->tab_details->setColumnWidth(6, 245);
    ui->tab_details->setColumnWidth(7, 90);
    ui->tab_details->setColumnWidth(8, 190);
    ui->tab_details->setColumnWidth(9, 77);
//    ui->tab_details->setColumnWidth(10, 100);
    ui->tab_details->setColumnWidth(10, 60);
    ui->tab_details->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->tab_details->setAlternatingRowColors(true);
    ui->tab_details->setStyleSheet("color: rgb(0, 0, 0);    /*前景色：文字颜色*/"
                                "background:white;"
                                "gridline-color:rgb(161,161,161);"
                                "alternate-background-color:rgb(244, 244, 244);"
                                "selection-color:white;    /*鼠标选中时前景色：文字颜色*/"
                                "selection-background-color:rgb(23, 166, 255);   /*鼠标选中时背景色*/");
    QFile qssScrollbar(":/stylesheet/styleSheet/ScrollBar.qss");
    qssScrollbar.open(QIODevice::ReadOnly);
    QString style = QString(qssScrollbar.readAll());
    ui->tab_frame->setStyleSheet(style);
    qssScrollbar.close();

    //初始化rfid设备展示
    ui->rfidDevView->setModel(rfManager->rfidReaderModel());
    ui->rfidDevView->setColumnWidth(0, 150);
    ui->rfidDevView->setColumnWidth(1, 80);
    ui->rfidDevView->setColumnWidth(2, 80);
    ui->rfidDevView->setColumnWidth(3,80);
    ui->rfidDevView->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->rfidDevView->setAlternatingRowColors(true);
    ui->rfidDevView->setStyleSheet("color: rgb(0, 0, 0);    /*前景色：文字颜色*/"
                                "background:white;"
                                "gridline-color:rgb(161,161,161);"
                                "alternate-background-color:rgb(244, 244, 244);"
                                "selection-color:white;    /*鼠标选中时前景色：文字颜色*/"
                                "selection-background-color:rgb(23, 166, 255);   /*鼠标选中时背景色*/");

    //初始化汇总信息展示
    sumFilterModel = new QSortFilterProxyModel;
    sumFilterModel->setSourceModel(eSumModel);
    sumFilterModel->setDynamicSortFilter(true);
    sumFilterModel->setFilterKeyColumn(3);
    sumFilterModel->setFilterRegExp("^[1-9]*[1-9][0-9]*$");

    ui->tab_summary->setModel(sumFilterModel);
//    ui->tab_summary->setColumnWidth(0, 150);
//    ui->tab_summary->setColumnWidth(1, 80);
//    ui->tab_summary->setColumnWidth(2, 80);
//    ui->tab_summary->setColumnWidth(3,80);
    ui->tab_summary->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tab_summary->setAlternatingRowColors(true);
    ui->tab_summary->setStyleSheet("color: rgb(0, 0, 0);    /*前景色：文字颜色*/"
                                   "background:white;"
                                   "gridline-color:rgb(161,161,161);"
                                   "alternate-background-color:rgb(244, 244, 244);"
                                   "selection-color:white;    /*鼠标选中时前景色：文字颜色*/"
                                   "selection-background-color:rgb(23, 166, 255);   /*鼠标选中时背景色*/");

}

void FrmRfid::setPow(int pow)
{
    rfManager->setCurOptPow(pow);
    QStringList powList;
    powList<<"权限：管理员"
          <<"权限：护士长"
         <<"权限：护士"
        <<"权限：仓管"
       <<"权限：员工";

    QStringList sceneList;
    sceneList<<"综合"
            <<"取货"
           <<"取货"
          <<"存货"
         <<"取货";
    ui->lab_scene->setText(sceneList.at(pow));

    switch(pow)
    {
    case 0:
        visibleFlag = QBitArray(mark_checked+1, false);
        visibleFlag[mark_in] = true;
        visibleFlag[mark_out] = true;
//        visibleFlag[mark_back] = true;
        visibleFlag[mark_new] = true;
        visibleFlag[mark_wait_back] = true;
        visibleFlag[mark_all] = true;
        visibleFlag[mark_checked] = true;
        break;
    case 1:
        visibleFlag = QBitArray(mark_checked+1, false);
//        visibleFlag[mark_in] = true;
        visibleFlag[mark_out] = true;
        btnTable[mark_out]->setChecked(true);
//        visibleFlag[mark_back] = true;
//        visibleFlag[mark_new] = true;
//        visibleFlag[mark_wait_back] = true;
//        visibleFlag[mark_all] = true;
//        visibleFlag[mark_checked] = true;
        break;
    case 2:
        visibleFlag = QBitArray(mark_checked+1, false);
//        visibleFlag[mark_in] = true;
        visibleFlag[mark_out] = true;
        btnTable[mark_out]->setChecked(true);
//        visibleFlag[mark_back] = true;
//        visibleFlag[mark_new] = true;
//        visibleFlag[mark_wait_back] = true;
//        visibleFlag[mark_all] = true;
//        visibleFlag[mark_checked] = true;
        break;
    case 3:
        visibleFlag = QBitArray(mark_checked+1, false);
//        visibleFlag[mark_in] = true;
//        visibleFlag[mark_out] = true;
//        visibleFlag[mark_back] = true;
        visibleFlag[mark_new] = true;
        btnTable[mark_new]->setChecked(true);
//        visibleFlag[mark_wait_back] = true;
//        visibleFlag[mark_all] = true;
//        visibleFlag[mark_checked] = true;
        QTimer::singleShot(2000, this, SLOT(rfidCheck()));
        break;
    case 4:
        visibleFlag = QBitArray(mark_checked+1, false);
//        visibleFlag[mark_in] = true;
        visibleFlag[mark_out] = true;
        btnTable[mark_out]->setChecked(true);
//        visibleFlag[mark_back] = true;
//        visibleFlag[mark_new] = true;
//        visibleFlag[mark_wait_back] = true;
//        visibleFlag[mark_all] = true;
//        visibleFlag[mark_checked] = true;
        break;
    default:
        return;
//        break;
    }

    ui->lab_pow->setText(powList.at(pow));

//    if(visibleFlag[mark_all])
//        ui->tab_filter_all->show();
}

void FrmRfid::setDownCount(int count)
{
    downCount = count;
}

void FrmRfid::clearCurOperation()
{
    qDebug("clear");
    ui->frm_operation->setCurOperationNo(QString());
    ui->frm_operation->setCurOperationStr(QString());
}

/**
 * @brief FrmRfid::scanData 扫码操作转为等效的RFID扫描信息
 * @param scanCode
 */
void FrmRfid::scanData(QByteArray scanCode)
{
    QString epcCode = SqlManager::getEpcCode(QString(scanCode));
    qDebug()<<"[scanEpcCode]"<<epcCode;

    if(epcCode.isEmpty())
        return;

    rfManager->updateEpc(epcCode, RF_AUTO);
}

void FrmRfid::clearCountText()
{
    scanProgress(0, 0);
    foreach (QToolButton* btn, btnTable)
    {
//        btn->setText(btn->text().replace(QRegExp(":[0-9]*"), QString(":%1").arg(0)));
        btn->hide();
        qDebug()<<"clear:"<<btn->objectName();
    }
    ui->lab_count->setText("0");
    ui->lab_pac->setText("0");
//    if(visibleFlag[mark_all])
//        ui->tab_filter_all->show();
}

void FrmRfid::showEvent(QShowEvent *)
{
    clearCountText();
    ui->msg->clear();
//    updateOperationState();
}

void FrmRfid::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.fillRect(rect(), QColor(50,50,50,0));
//    qDebug()<<"COL SIZE"<<ui->tab_details->columnWidth(0)
//              <<ui->tab_details->columnWidth(1)
//                <<ui->tab_details->columnWidth(2)
//                  <<ui->tab_details->columnWidth(3)
//                    <<ui->tab_details->columnWidth(4)
//                      <<ui->tab_details->columnWidth(5)
//                        <<ui->tab_details->columnWidth(6)
//                          <<ui->tab_details->columnWidth(7)
//                            <<ui->tab_details->columnWidth(8)
//                              <<ui->tab_details->columnWidth(9)
//                                <<ui->tab_details->columnWidth(10);
}

void FrmRfid::on_OK_clicked()
{
    rfManager->clsFinish();
    accessSuccess("操作成功");
    clearCountText();
    scanProgress(0, 0);
    emit requireSysLock();
}

void FrmRfid::on_fresh_clicked()
{
    rfManager->doorCloseScan();
    rfManager->timerClear();
    clearCountText();
    eModel->clearEpcMark();
}

void FrmRfid::on_close_clicked()
{
#ifdef test_rfid
    ui->stackedWidget->setCurrentIndex(0);
#else
    this->close();
    rfManager->clsGiveUp();
#endif
}

void FrmRfid::on_tab_filter_all_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(9);
        filterModel->setFilterRegExp(".*");
//        ui->tab_details->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_out_toggled(bool checked)
{
//    Q_UNUSED(checked);
    if(checked)
    {
        filterModel->setFilterKeyColumn(9);
        filterModel->setFilterRegExp("取出$");
//        filterModel->setFilterFixedString(eModel->markTab().at(mark_out));
        ui->lab_cur_scene->setText("当前取出");

        eSumModel->setScene(mark_out);
        updateCountInfo(mark_out);
//        ui->tab_details->resizeColumnsToContents();
    }
    else
    {
//        filterModel->setFilterKeyColumn(9);
//        filterModel->setFilterRegExp(".*");
//        ui->tab_details->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_new_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(9);
        ui->lab_cur_scene->setText("当前存入");
//        filterModel->setFilterRegExp("存入");
        filterModel->setFilterFixedString(eModel->markTab().at(mark_new));

        eSumModel->setScene(mark_new);
        updateCountInfo(mark_new);
//        ui->tab_details->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_back_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(9);
//        filterModel->setFilterRegExp("还回");
        ui->lab_cur_scene->setText("当前还回");
        filterModel->setFilterFixedString(eModel->markTab().at(mark_back));

        eSumModel->setScene(mark_back);
        updateCountInfo(mark_back);
//        ui->tab_details->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_consume_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(9);
//        filterModel->setFilterRegExp("登记");
        filterModel->setFilterFixedString(eModel->markTab().at(mark_con));

        eSumModel->setScene(mark_con);
        updateCountInfo(mark_con);
//        ui->tab_details->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_in_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(9);
//        filterModel->setFilterRegExp("柜内");
        filterModel->setFilterFixedString(eModel->markTab().at(mark_in));

        eSumModel->setScene(mark_in);
        updateCountInfo(mark_in);
//        ui->tab_details->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_unknow_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(9);
//        filterModel->setFilterRegExp("未知");
        filterModel->setFilterFixedString(eModel->markTab().at(mark_no));

        eSumModel->setScene(mark_no);
        updateCountInfo(mark_no);
//        ui->tab_details->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_wait_back_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(9);
//        filterModel->setFilterRegExp("取出未还");
        filterModel->setFilterFixedString(eModel->markTab().at(mark_wait_back));

        eSumModel->setScene(mark_wait_back);
        updateCountInfo(mark_wait_back);
//        ui->tab_details->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_details_clicked(const QModelIndex &index)
{
    if(index.column() != (ui->tab_details->model()->columnCount()-1))//不是最后一列的操作,排除
    {
        return;
    }

    if(rfManager->accessIsLock())
        eModel->operation(ui->tab_details->model()->index(index.row(), 1).data().toString(), mark_in);
    else
        eModel->operation(ui->tab_details->model()->index(index.row(), 1).data().toString(), mark_checked);

//    qDebug()<<ui->tab_details->model()->index(index.row(), 1).data().toString();
}

void FrmRfid::on_stop_scan_clicked()
{
    rfManager->clsGiveUp();
}

void FrmRfid::on_close_2_clicked()
{
    close();
//    clearCurOperation();
}

void FrmRfid::on_add_device_clicked()
{
    RfidDevHub* devModel = (RfidDevHub*)ui->rfidDevView->model();
    QString dev_addr = ui->input_addr->text();
    quint16 dev_port = ui->input_port->text().toInt();
    DevAction dev_act = DevAction(1<<ui->dev_act->currentIndex());
    devModel->addDevice(dev_addr, dev_port, dev_act);
}

/**
 * @brief int2bit 求一个数等效于1左移多少位
 * @param num
 * @return
 */
int int2bit(int num)
{
    int ret=0;
    while(num)
    {
        qDebug()<<"num:"<<num;
        num = (num>>1);
        ret++;
    }
    qDebug()<<"int2bit"<<num<<"= 1<<"<<ret;
    return ret;
}

void FrmRfid::on_rfidDevView_clicked(const QModelIndex &index)
{
    RfidDevHub* devModel = rfManager->rfidReaderModel();
    QString selDevIp = devModel->index(index.row(),0).data().toString();
    if(index.column() == 3)//删除
    {
        devModel->delDevice(selDevIp);
        return;
    }

    curSelRfidReader = selDevIp;

    if(ui->dev_name->text() == curSelRfidReader)
        return;

    RfidReader* dev = devModel->device(curSelRfidReader);
    if(dev == NULL)
        return;

    qDebug()<<"curSelRfidReader:"<<curSelRfidReader;
    qDebug()<<"confIntens:"<<dev->property("confIntens").toByteArray().toHex();
    qDebug()<<"antPowConfig:"<<dev->property("antPowConfig").toByteArray().toHex();
    qDebug()<<"gradientThreshold:"<<dev->property("gradientThreshold").toInt();
    qDebug()<<"devAct:"<<dev->property("devAct").toInt();
    qDebug()<<"antState:"<<dev->property("antState");
//    qDebug()<<dev->property("outsideDev").toBool();
    ui->conf_int->setValue((int)dev->property("confIntens").toByteArray().at(0));
    ui->ant_pow->setValue((int)dev->property("antPowConfig").toByteArray().at(0));
    ui->grad_thre->setValue(dev->property("gradientThreshold").toInt());
    ui->dev_act->setCurrentIndex(int2bit(dev->property("devAct").toInt())-1);
    ui->dev_name->setText(curSelRfidReader);
    updateAntState(dev);
}

void FrmRfid::on_sig_add_clicked()
{
    ui->ant_pow->setValue(ui->ant_pow->value()+1);
}

void FrmRfid::on_sig_minus_clicked()
{
    ui->ant_pow->setValue(ui->ant_pow->value()-1);
}

void FrmRfid::on_ant_pow_valueChanged(int value)
{
    if(ui->dev_name->text() != curSelRfidReader)
        return;

    RfidDevHub* devModel = (RfidDevHub*)ui->rfidDevView->model();
    RfidReader* dev = devModel->device(curSelRfidReader);
    if(dev == NULL)
        return;

    dev->setProperty("antPowConfig", QByteArray(8, (char)value));
}

void FrmRfid::on_conf_minus_clicked()
{
    ui->conf_int->setValue(ui->conf_int->value()-1);
}

void FrmRfid::on_conf_add_clicked()
{
    ui->conf_int->setValue(ui->conf_int->value()+1);
}

void FrmRfid::on_conf_int_valueChanged(int value)
{
    if(ui->dev_name->text() != curSelRfidReader)
        return;
    RfidDevHub* devModel = (RfidDevHub*)ui->rfidDevView->model();
    RfidReader* dev = devModel->device(curSelRfidReader);
    if(dev == NULL)
        return;

    dev->setProperty("confIntens", QByteArray(8, (char)value));
}

void FrmRfid::on_grad_minus_clicked()
{
    ui->grad_thre->setValue(ui->grad_thre->value()-1);
}

void FrmRfid::on_grad_add_clicked()
{
    ui->grad_thre->setValue(ui->grad_thre->value()+1);
}

void FrmRfid::on_grad_thre_valueChanged(int value)
{
    if(ui->dev_name->text() != curSelRfidReader)
        return;
    RfidDevHub* devModel = (RfidDevHub*)ui->rfidDevView->model();
    RfidReader* dev = devModel->device(curSelRfidReader);
    if(dev == NULL)
        return;

    dev->setProperty("gradientThreshold", value);
}

void FrmRfid::ant_state_changed(bool)
{
    if(ui->dev_name->text() != curSelRfidReader)
        return;
    RfidDevHub* devModel = (RfidDevHub*)ui->rfidDevView->model();
    RfidReader* dev = devModel->device(curSelRfidReader);
    if(dev == NULL)
        return;

    dev->setProperty("antState", curAntState());
}

void FrmRfid::on_dev_type_toggled(bool checked)
{
    RfidDevHub* devModel = (RfidDevHub*)ui->rfidDevView->model();
    RfidReader* dev = devModel->device(curSelRfidReader);
    if(dev == NULL)
        return;

    dev->setProperty("outsideDev", checked);
}

void FrmRfid::on_operation_clicked(bool checked)
{
//    Q_UNUSED(checked);
    ui->operation->setChecked(!checked);
    showOperation();
}

//void FrmRfid::on_dev_act_currentIndexChanged(int index)
//{

//}

void FrmRfid::on_dev_act_activated(int index)
{
    RfidDevHub* devModel = (RfidDevHub*)ui->rfidDevView->model();
    RfidReader* dev = devModel->device(curSelRfidReader);
    if(dev == NULL)
        return;

    dev->setProperty("devAct", (1<<index));
}

void FrmRfid::on_show_summary_toggled(bool checked)
{
    if(checked)
    {
        ui->stacked_view->setCurrentWidget(ui->page_summary);
    }
}

void FrmRfid::on_show_details_toggled(bool checked)
{
    if(checked)
    {
        ui->stacked_view->setCurrentWidget(ui->page_details);
    }
}
