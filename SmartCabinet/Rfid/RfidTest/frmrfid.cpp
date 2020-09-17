#include "frmrfid.h"
#include "ui_frmrfid.h"
#include <QDebug>
#include <QByteArray>
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
//    btnTable.insert(mark_new, ui->tab_filter_new);
//    btnTable.insert(mark_back, ui->tab_filter_back);
    btnTable.insert(mark_out, ui->tab_filter_out);
//    btnTable.insert(mark_con, ui->tab_filter_consume);
//    btnTable.insert(mark_in, ui->tab_filter_in);
//    btnTable.insert(mark_wait_back, ui->tab_filter_wait_back);
//    btnTable.insert(mark_all, ui->tab_filter_all);

    QRegExp ipRx("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    ui->input_addr->setValidator(new QRegExpValidator(ipRx));

    QRegExp portRx("^([0-9]|[1-9]\\d|[1-9]\\d{2}|[1-9]\\d{3}|[1-5]\\d{4}|6[0-4]\\d{3}|65[0-4]\\d{2}|655[0-2]\\d|6553[0-5])$");
    ui->input_port->setValidator(new QRegExpValidator(portRx));

    eModel = new EpcModel(this);
    isLogin = false;
    rfManager = new RfidManager(eModel);
    connect(rfManager, SIGNAL(updateTimer(int)), this, SLOT(updateScanTimer(int)));
    connect(rfManager, SIGNAL(optFinish()), this, SLOT(showEpcInfo()));

    SignalManager* sigMan = SignalManager::manager();
    connect(sigMan, SIGNAL(accessSuccess(QString)), this, SLOT(accessSuccess(QString)));
    connect(sigMan, SIGNAL(accessFailed(QString)), this, SLOT(accessFailed(QString)));
    connect(sigMan, SIGNAL(configRfidDevice()), this, SLOT(showConfigDevice()));

    connect(ui->frm_operation, SIGNAL(winClose()), this, SLOT(showEpcInfo()));
    connect(ui->frm_operation, SIGNAL(requireUpdate()), sigMan, SIGNAL(requireUpdateOperation()));
    connect(sigMan, SIGNAL(operationInfoUpdate()), ui->frm_operation, SLOT(loadOperations()));

    initTabs();
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
    ui->scan_timer->display(ms);
    //刷新信号强度
//    int index = ui->tab_view->verticalScrollBar()->value();
//    int countMax = ui->tab_view->verticalScrollBar()->pageStep()+1;
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
//        btnTable[mark]->setVisible(count);//没有屏蔽掩膜
    }

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
    ui->tab_filter_out->setChecked(true);
}

void FrmRfid::updateSelReader(QString devIp)
{

}

void FrmRfid::updateOperationState()
{
    QString strOperation = ui->frm_operation->curOperation();
    if(strOperation.isEmpty())
    {
        ui->operation->setChecked(true);
        ui->operation->setText("请选择手术单");
    }
    else
    {
        ui->operation->setChecked(false);
        ui->operation->setText(strOperation);
    }
}

void FrmRfid::updateCurUser(QString optId)
{
    rfManager->setCurOptId(optId);
}

void FrmRfid::scanProgress(int curCount, int totalCount)
{
//    ui->tab_filter_all->setText(QString("总览:%1").arg(totalCount));
    ui->count->display(curCount);
}

void FrmRfid::updateLockCount(int lockCount)
{
    ui->lock_count->display(lockCount);
    if(lockCount>0 && !ui->tab_filter_out->isChecked())
    {
        ui->tab_filter_out->setChecked(true);
    }
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

void FrmRfid::showOperation()
{
    ui->stackedWidget->setCurrentIndex(2);
    showMaximized();
}

void FrmRfid::updateAntInCount(int count)
{
    ui->in_count->setText(QString::number(count));
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
    connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex)), filterModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)));
    ui->tab_view->setModel(filterModel);
    ui->tab_view->setColumnWidth(0, 113);
    ui->tab_view->setColumnWidth(1, 109);
    ui->tab_view->setColumnWidth(2, 137);
    ui->tab_view->setColumnWidth(3, 210);
    ui->tab_view->setColumnWidth(4, 77);
    ui->tab_view->setColumnWidth(5, 228);
    ui->tab_view->setColumnWidth(6, 245);
    ui->tab_view->setColumnWidth(7, 90);
    ui->tab_view->setColumnWidth(8, 190);
    ui->tab_view->setColumnWidth(9, 77);
//    ui->tab_view->setColumnWidth(10, 100);
    ui->tab_view->setColumnWidth(10, 60);
    ui->tab_view->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->tab_view->setAlternatingRowColors(true);
    ui->tab_view->setStyleSheet("color: rgb(0, 0, 0);    /*前景色：文字颜色*/"
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


//    foreach (QToolButton* btn, btnTable)
//    {
//        btn->hide();
//    }
}

void FrmRfid::setPow(int pow)
{
    switch(pow)
    {
    case 0:
//        visibleFlag = QBitArray(mark_checked+1, true);
        visibleFlag = QBitArray(mark_checked+1, false);
        visibleFlag[mark_in] = true;
        visibleFlag[mark_out] = true;
//        visibleFlag[mark_back] = true;
        visibleFlag[mark_new] = true;
        visibleFlag[mark_wait_back] = true;
        visibleFlag[mark_all] = true;
        visibleFlag[mark_checked] = true;
        break;
    default:
        break;
    }

//    if(visibleFlag[mark_all])
//        ui->tab_filter_all->show();
}

void FrmRfid::clearCountText()
{
    foreach (QToolButton* btn, btnTable)
    {
//        btn->setText(btn->text().replace(QRegExp(":[0-9]*"), QString(":%1").arg(0)));
        btn->hide();
        qDebug()<<"clear:"<<btn->objectName();
    }
//    if(visibleFlag[mark_all])
//        ui->tab_filter_all->show();
}

void FrmRfid::showEvent(QShowEvent *)
{
    ui->msg->clear();
    updateOperationState();
}

void FrmRfid::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.fillRect(rect(), QColor(50,50,50,0));
//    qDebug()<<"COL SIZE"<<ui->tab_view->columnWidth(0)
//              <<ui->tab_view->columnWidth(1)
//                <<ui->tab_view->columnWidth(2)
//                  <<ui->tab_view->columnWidth(3)
//                    <<ui->tab_view->columnWidth(4)
//                      <<ui->tab_view->columnWidth(5)
//                        <<ui->tab_view->columnWidth(6)
//                          <<ui->tab_view->columnWidth(7)
//                            <<ui->tab_view->columnWidth(8)
//                              <<ui->tab_view->columnWidth(9)
//                                <<ui->tab_view->columnWidth(10);
}

void FrmRfid::on_OK_clicked()
{
    rfManager->clsFinish();
    accessSuccess("操作成功");
    clearCountText();
    scanProgress(0, 0);
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
    this->hide();
    rfManager->clsGiveUp();
#endif
}

void FrmRfid::on_tab_filter_all_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(9);
        filterModel->setFilterRegExp(".*");
//        ui->tab_view->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_out_toggled(bool checked)
{
//    Q_UNUSED(checked);
    if(checked)
    {
        filterModel->setFilterKeyColumn(9);
        filterModel->setFilterRegExp("取出$");
//        ui->tab_view->resizeColumnsToContents();
    }
    else
    {
        filterModel->setFilterKeyColumn(9);
        filterModel->setFilterRegExp(".*");
//        ui->tab_view->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_new_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(9);
//        filterModel->setFilterRegExp("存入");
        filterModel->setFilterFixedString(eModel->markTab().at(mark_new));
//        ui->tab_view->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_back_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(9);
//        filterModel->setFilterRegExp("还回");
        filterModel->setFilterFixedString(eModel->markTab().at(mark_back));
//        ui->tab_view->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_consume_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(9);
//        filterModel->setFilterRegExp("登记");
        filterModel->setFilterFixedString(eModel->markTab().at(mark_con));
//        ui->tab_view->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_in_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(9);
//        filterModel->setFilterRegExp("柜内");
        filterModel->setFilterFixedString(eModel->markTab().at(mark_in));
//        ui->tab_view->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_unknow_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(9);
//        filterModel->setFilterRegExp("未知");
        filterModel->setFilterFixedString(eModel->markTab().at(mark_no));
//        ui->tab_view->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_wait_back_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(9);
//        filterModel->setFilterRegExp("取出未还");
        filterModel->setFilterFixedString(eModel->markTab().at(mark_wait_back));
//        ui->tab_view->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_view_clicked(const QModelIndex &index)
{
    if(index.column() != (ui->tab_view->model()->columnCount()-1))//不是最后一列的操作,排除
    {
        return;
    }

    if(rfManager->accessIsLock())
        eModel->operation(ui->tab_view->model()->index(index.row(), 1).data().toString(), mark_in);
    else
        eModel->operation(ui->tab_view->model()->index(index.row(), 1).data().toString(), mark_checked);

//    qDebug()<<ui->tab_view->model()->index(index.row(), 1).data().toString();
}

void FrmRfid::on_stop_scan_clicked()
{
    rfManager->clsGiveUp();
}

void FrmRfid::on_close_2_clicked()
{
    this->close();
}

void FrmRfid::on_add_device_clicked()
{
    RfidDevHub* devModel = (RfidDevHub*)ui->rfidDevView->model();
    QString dev_addr = ui->input_addr->text();
    quint16 dev_port = ui->input_port->text().toInt();
    QString dev_type = ui->dev_outside->isChecked()?QString("outside"):QString("inside");
    devModel->addDevice(dev_addr, dev_port, dev_type);
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
    qDebug()<<ui->dev_name->text()<<curSelRfidReader;
    if(ui->dev_name->text() == curSelRfidReader)
        return;

    RfidReader* dev = devModel->device(curSelRfidReader);
    if(dev == NULL)
        return;
    qDebug()<<dev->property("confIntens").toByteArray().toHex();
    qDebug()<<dev->property("antPowConfig").toByteArray().at(0);
    qDebug()<<dev->property("gradientThreshold").toInt();
    qDebug()<<dev->property("outsideDev").toBool();
    ui->conf_int->setValue((int)dev->property("confIntens").toByteArray().at(0));
    ui->ant_pow->setValue((int)dev->property("antPowConfig").toByteArray().at(0));
    ui->grad_thre->setValue(dev->property("gradientThreshold").toInt());
    ui->dev_type->setChecked(dev->property("outsideDev").toBool());
    ui->dev_name->setText(curSelRfidReader);
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
    Q_UNUSED(checked);
    showOperation();
}
