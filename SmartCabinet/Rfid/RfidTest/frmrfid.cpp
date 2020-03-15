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
    initTabs();
    isLogin = false;
    rfManager = new RfidManager();
    rfManager->initTableViews(tabs[0], tabs[1], tabs[2], tabs[3], tabs[4]);
    connect(rfManager, SIGNAL(updateEpcInfo(EpcInfo*)), this, SLOT(updateEpcInfo(EpcInfo*)));
    connect(rfManager, SIGNAL(epcStateChanged(TableMark)), this, SLOT(showTabs(TableMark)));
    connect(rfManager, SIGNAL(updateCount(int)), this, SLOT(updateCount(int)));
    connect(rfManager, SIGNAL(updateTimer(int)), this, SLOT(updateScanTimer(int)));

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
    qDeleteAll(tabs.begin(), tabs.end());
    delete ui;
}

void FrmRfid::updateScanTimer(int ms)
{
    ui->scan_timer->display(ms);
}

void FrmRfid::updateCount(int count)
{
    ui->count->display(count);
}

void FrmRfid::updateCurUser(QString optId)
{
    rfManager->setCurOptId(optId);
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
    rfManager->stopScan();
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
    win_tabs = new QTabWidget(this);
    tabs<<new QTableView();
    tabs<<new QTableView();
    tabs<<new QTableView();
    tabs<<new QTableView();
    tabs<<new QTableView();
    list_win_name<<"存入"<<"取出"<<"还回"<<"消耗"<<"盘点";
    ui->layout_tabs->addWidget(win_tabs);
    QFile qssScrollbar(":/stylesheet/styleSheet/ScrollBar.qss");
    qssScrollbar.open(QIODevice::ReadOnly);
    QString style = QString(qssScrollbar.readAll());
    win_tabs->setStyleSheet(style);
    qssScrollbar.close();
    //    win_tabs->hide();
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
}

void FrmRfid::on_fresh_clicked()
{
    rfManager->stopScan();
}

void FrmRfid::on_pushButton_clicked()
{
#ifdef test_rfid
    ui->stackedWidget->setCurrentIndex(0);
#else
    this->hide();
#endif
}
