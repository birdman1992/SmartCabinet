#include "dayreport.h"
#include "ui_dayreport.h"
#include <QDebug>

DayReport::DayReport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DayReport)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
//    ui->list_table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->label_2->hide();
    ui->date_start->installEventFilter(this);
    ui->date_finish->installEventFilter(this);
//    this->setWindowOpacity(0.7);
//    setCheckTables(createDateList(QDateTime(), QDateTime::currentDateTime()));
}

DayReport::~DayReport()
{
    delete ui;
}

void DayReport::recvReportInfo(QList<DayReportInfo *> l, QString msg)
{
    qDebug()<<msg;
    setCurReportTable(l);
    qDeleteAll(l.begin(), l.end());
}

//void DayReport::updateCheckTable(CheckList *l)
//{
//    this->show();

//    if(ui->list_table_id->rowCount() == 0)
//    {
//        showOnlyCheckTable(l);
//    }
//    else
//    {
//        setCurCheckTable(l);
//    }
//    ui->list_table->resizeColumnsToContents();
//}

//void DayReport::recvCheckTables(QList<CheckTableInfo *> tabs)
//{
//    QStringList tabDate;
//    if(!listCheckTables.isEmpty())
//    {
//        qDeleteAll(listCheckTables.begin(), listCheckTables.end());
//        listCheckTables.clear();
//    }
//    listCheckTables = tabs;

//    foreach(CheckTableInfo* tab, tabs)
//    {
//        tabDate<<tab->eTime;
//    }
//    setCheckTables(tabDate);
//}


void DayReport::on_date_start_dateChanged(const QDate &date)
{
    ui->date_finish->setMinimumDate(date);
    emit askListInfo(ui->date_start->date(), ui->date_start->date());
//    setCheckTables(createDateList(ui->date_start->dateTime(), ui->date_finish->dateTime()));
}

//unuse
void DayReport::on_date_finish_dateChanged(const QDate &date)
{
    ui->date_start->setMaximumDate(date);
//    setCheckTables(createDateList(ui->date_start->dateTime(), ui->date_finish->dateTime()));
}

bool DayReport::eventFilter(QObject *w, QEvent *e)
{
    return QWidget::eventFilter(w,e);
}

void DayReport::showEvent(QShowEvent *)
{
    QDate curStartDate = ui->date_start->date();
    ui->date_start->setDisplayFormat("yyyy/MM/dd");
    ui->date_finish->setDisplayFormat("yyyy/MM/dd");
    ui->date_start->setDate(QDate::currentDate());
    ui->date_finish->setDate(QDate::currentDate());
    ui->date_start->setMaximumDate(QDate::currentDate());
    ui->date_finish->setMinimumDate(QDate::currentDate());
    ui->date_finish->setMaximumDate(QDate::currentDate());
    ui->date_finish->hide();

    if(curStartDate == ui->date_start->date())
        emit askListInfo(ui->date_start->date(), ui->date_start->date());
}

//void DayReport::setCheckTables(QStringList l)
//{
//    listDates = l;
//    ui->list_table_id->clearContents();
//    ui->list_table_id->setRowCount(l.count());
//    ui->list_table_id->setColumnCount(1);
//    if(l.isEmpty())
//        return;
//    for(int i=0; i<l.count(); i++)
//    {
//        ui->list_table_id->setItem(i, 0, new QTableWidgetItem(l.at(i)));
//    }

//}
//物品编码 批号 物品名称 规格 数量 单位 消耗类型 操作人 时间 物品条码 生产商 供应商 单价 合计
void DayReport::setCurReportTable(QList<DayReportInfo *> l)
{
    ui->list_table->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
    ui->list_table->clearContents();
    ui->list_table->setColumnCount(14);
    ui->list_table->setRowCount(l.count());

    if(l.isEmpty())
        return;

    QStringList lState;
    lState<<""<<""<<"正常消耗"<<""<<"盘点消耗"<<""<<"手术单取出";

    for(int i=0; i<l.count(); i++)
    {
        DayReportInfo* info = l.at(i);
//        qDebug()<<info->state<<lState;
        ui->list_table->setItem(i, 0, new QTableWidgetItem(info->goodsId));
        ui->list_table->setItem(i, 1, new QTableWidgetItem(info->batchNumber));
        ui->list_table->setItem(i, 2, new QTableWidgetItem(info->goodsName));
        ui->list_table->setItem(i, 3, new QTableWidgetItem(info->size));
        ui->list_table->setItem(i, 4, new QTableWidgetItem(info->goodsCount));
        ui->list_table->setItem(i, 5, new QTableWidgetItem(info->unit));
        ui->list_table->setItem(i, 6, new QTableWidgetItem(lState.at(info->state)));
        ui->list_table->setItem(i, 7, new QTableWidgetItem(info->optName));
        ui->list_table->setItem(i, 8, new QTableWidgetItem(info->optTime));
        ui->list_table->setItem(i, 9, new QTableWidgetItem(info->traceId));
        ui->list_table->setItem(i, 10, new QTableWidgetItem(info->proName));
        ui->list_table->setItem(i, 11, new QTableWidgetItem(info->supplyName));
        ui->list_table->setItem(i, 12, new QTableWidgetItem(QString::number(info->price, 'f', 2)));
        ui->list_table->setItem(i, 13, new QTableWidgetItem(QString::number(info->sumCount, 'f', 2)));

    }

    ui->list_table->resizeColumnsToContents();
}

//void DayReport::showOnlyCheckTable(CheckList *l)
//{
//    ui->list_table_id->hide();
//    ui->check->hide();
//    ui->date_start->setDate(l->startTime.date());
//    ui->date_finish->setDate(l->finishTime.date());
//    setCurReportTable(l);
//}

QStringList DayReport::createDateList(QDateTime sDate, QDateTime eDate)
{
    QDateTime cDate = sDate;
    if(cDate.isNull())
    {
        cDate = eDate;
        QDate tmp = cDate.date();
        tmp.setDate(cDate.date().year(), cDate.date().month(), 1);
        cDate.setDate(tmp);
    }
    QStringList ret;
    while(cDate < eDate)
    {
        ret<<cDate.date().toString("yyyy-MM-dd");
        cDate = cDate.addDays(1);
        qDebug()<<cDate;
    }
    return ret;
}

void DayReport::on_close_clicked()
{
    ui->check->show();
//    ui->list_table_id->show();
    this->hide();
    ui->list_table->clearContents();
    ui->list_table->setRowCount(0);
//    ui->list_table_id->clearContents();
//    ui->list_table_id->setRowCount(0);
}

void DayReport::on_check_clicked()
{
    QDate cDate = ui->date_start->date();
    cDate = cDate.addDays(-1);
    ui->date_start->setDate(cDate);
}

void DayReport::on_check_next_clicked()
{
    QDate cDate = ui->date_start->date();
    cDate = cDate.addDays(1);
    ui->date_start->setDate(cDate);
}

//void DayReport::on_list_table_id_cellClicked(int row, int)
//{
//    if(listDates.isEmpty())
//        return;
//    if(row > listDates.count())
//        return;

//    ui->list_table->clearContents();
//    ui->list_table->setRowCount(0);
//    QDate checkDate = QDate::fromString(listDates.at(row), "yyyy-MM-dd");
//    emit askListInfo(checkDate, checkDate);
//}
