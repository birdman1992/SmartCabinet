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
    btnTable.insert(mark_no, ui->tab_filter_unknow);
    btnTable.insert(mark_new, ui->tab_filter_new);
    btnTable.insert(mark_back, ui->tab_filter_back);
    btnTable.insert(mark_out, ui->tab_filter_out);
    btnTable.insert(mark_con, ui->tab_filter_consume);
    btnTable.insert(mark_in, ui->tab_filter_in);
    btnTable.insert(mark_wait_back, ui->tab_filter_wait_back);
    btnTable.insert(mark_all, ui->tab_filter_all);

    initTabs();
    isLogin = false;
    rfManager = new RfidManager(eModel);
//    connect(rfManager, SIGNAL(updateEpcInfo(EpcInfo*)), this, SLOT(updateEpcInfo(EpcInfo*)));
    connect(rfManager, SIGNAL(epcStateChanged(TableMark)), this, SLOT(showTabs(TableMark)));
//    connect(rfManager, SIGNAL(updateCount(int)), this, SLOT(updateCount(int)));
    connect(rfManager, SIGNAL(updateTimer(int)), this, SLOT(updateScanTimer(int)));
    connect(rfManager, SIGNAL(optFinish()), this, SLOT(showMaximized()));

    SignalManager* sigMan = SignalManager::manager();
    connect(sigMan, SIGNAL(accessSuccess(QString)), this, SLOT(accessSuccess(QString)));
    connect(sigMan, SIGNAL(accessFailed(QString)), this, SLOT(accessFailed(QString)));

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
    qDeleteAll(tabs.begin(), tabs.end());
    delete ui;
}

void FrmRfid::updateScanTimer(int ms)
{
    ui->scan_timer->display(ms);
}

void FrmRfid::updateCount(EpcMark mark, int count)
{
//    if(mark == 0)
//        return;

    qDebug()<<mark<<count;
    if(mark < btnTable.count())
    {
        btnTable[mark]->setText(btnTable[mark]->text().replace(QRegExp(":[0-9]*"), QString(":%1").arg(count)));
        btnTable[mark]->setVisible(count);
    }
}

void FrmRfid::updateCurUser(QString optId)
{
    rfManager->setCurOptId(optId);
}

void FrmRfid::scanProgress(int curCount, int totalCount)
{
    ui->tab_filter_all->setText(QString("总览:%1").arg(totalCount));
    ui->count->display(curCount);
}

void FrmRfid::updateLockCount(int lockCount)
{
    ui->lock_count->display(lockCount);
}

void FrmRfid::testSlot()
{
    ui->stackedWidget->setCurrentIndex(0);
    this->showFullScreen();
}

void FrmRfid::updateAntInCount(int count)
{
    ui->in_count->setText(QString::number(count));
}

void FrmRfid::updateEpcInfo(EpcInfo *info)
{
    for(int i=0; i<ui->id_table->rowCount(); i++)
    {
        if(info->epcId == ui->id_table->itemAt(0, i)->text())
        {
            updateTableRow(i, info);
            return;
        }
    }
    int rowCount = ui->id_table->rowCount();
    ui->id_table->setRowCount(rowCount+1);
    ui->id_table->setItem(rowCount, 0, new QTableWidgetItem(info->epcId));
    ui->id_table->setItem(rowCount, 1, new QTableWidgetItem(info->goodsCode));
    ui->id_table->setItem(rowCount, 2, new QTableWidgetItem(QString::number(info->lastStamp)));
    ui->id_table->setItem(rowCount, 3, new QTableWidgetItem(QString::number(info->state)));
    ui->id_table->resizeColumnsToContents();
    ui->num->setText(QString::number(ui->id_table->rowCount()));
    //    disconnect(rfManager, SIGNAL(updateEpcInfo(EpcInfo*)), this, SLOT(updateEpcInfo(EpcInfo*)));
}

void FrmRfid::showTabs(TableMark tabMark)
{
    if(tabMark == 0)
    {
        win_tabs->clear();
        this->hide();
        qDebug()<<"[showTabs]"<<"empty tabMark";
        rfManager->clsFinish();
        return;
    }
    this->showFullScreen();

    ui->stackedWidget->setCurrentWidget(ui->page_tab);
    win_tabs->clear();
    for(int i=0; i<tabs.count(); i++)
    {
        if(tabMark & (1<<i))
        {
            win_tabs->addTab(tabs[i], list_win_name[i]);
            tabs[i]->resizeColumnsToContents();
        }
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

void FrmRfid::updateTableRow(int rowIndex, EpcInfo *info)
{
    ui->id_table->itemAt(0, rowIndex)->setText(info->epcId);
    ui->id_table->itemAt(1, rowIndex)->setText(info->goodsCode);
    ui->id_table->itemAt(2, rowIndex)->setText(QString::number(info->lastStamp));
    ui->id_table->itemAt(3, rowIndex)->setText(QString::number(info->state));
    ui->id_table->resizeColumnsToContents();
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
    eModel = new EpcModel(this);
    filterModel = new QSortFilterProxyModel;
    filterModel->setSourceModel(eModel);
    filterModel->setDynamicSortFilter(true);
    connect(eModel, SIGNAL(updateCount(EpcMark,int)), this, SLOT(updateCount(EpcMark,int)));
    connect(eModel, SIGNAL(scanProgress(int,int)), this, SLOT(scanProgress(int,int)));
    connect(eModel, SIGNAL(updateLockCount(int)), this, SLOT(updateLockCount(int)));
    ui->tab_view->setModel(filterModel);
    ui->tab_view->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->tab_view->setAlternatingRowColors(true);
    ui->tab_view->setStyleSheet("color: rgb(0, 0, 0);    /*前景色：文字颜色*/"
                                "background:white;"
                                "alternate-background-color:rgb(244, 244, 244);"
                                "selection-color:white;    /*鼠标选中时前景色：文字颜色*/"
                                "selection-background-color:rgb(23, 166, 255);   /*鼠标选中时背景色*/");

    QFile qssScrollbar(":/stylesheet/styleSheet/ScrollBar.qss");
    qssScrollbar.open(QIODevice::ReadOnly);
    QString style = QString(qssScrollbar.readAll());
    ui->tab_frame->setStyleSheet(style);
    qssScrollbar.close();

    foreach (QToolButton* btn, btnTable)
    {
        btn->hide();
    }
    ui->tab_filter_all->show();
}

void FrmRfid::setPow(int pow)
{
    switch(pow)
    {
    case 0:
        visibleFlag = QBitArray(mark_checked+1, true);
        break;
    default:
        break;
    }
}

void FrmRfid::clearCountText()
{
    foreach (QToolButton* btn, btnTable)
    {
//        btn->setText(btn->text().replace(QRegExp(":[0-9]*"), QString(":%1").arg(0)));
        btn->hide();
        qDebug()<<"clear:"<<btn->objectName();
    }
    ui->tab_filter_all->show();
}

void FrmRfid::showEvent(QShowEvent *)
{
    ui->msg->clear();
}

void FrmRfid::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.fillRect(rect(), QColor(50,50,50,0));
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

void FrmRfid::on_pushButton_clicked()
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
        filterModel->setFilterKeyColumn(7);
        filterModel->setFilterRegExp(".*");
        ui->tab_view->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_out_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(7);
        filterModel->setFilterRegExp("取出");
        ui->tab_view->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_new_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(7);
        filterModel->setFilterRegExp("存入");
        ui->tab_view->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_back_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(7);
        filterModel->setFilterRegExp("还回");
        ui->tab_view->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_consume_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(7);
        filterModel->setFilterRegExp("登记");
        ui->tab_view->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_in_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(7);
        filterModel->setFilterRegExp("柜内");
        ui->tab_view->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_unknow_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(7);
        filterModel->setFilterRegExp("未知");
        ui->tab_view->resizeColumnsToContents();
    }
}

void FrmRfid::on_tab_filter_wait_back_toggled(bool checked)
{
    if(checked)
    {
        filterModel->setFilterKeyColumn(7);
        filterModel->setFilterRegExp("取出未还");
        ui->tab_view->resizeColumnsToContents();
    }
}
