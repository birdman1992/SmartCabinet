#include "checktable.h"
#include "ui_checktable.h"
#include <QDate>
#include <QDebug>

CheckTable::CheckTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CheckTable)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
//    ui->list_table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->date_start->installEventFilter(this);
    ui->date_finish->installEventFilter(this);
//    this->setWindowOpacity(0.7);
}

CheckTable::~CheckTable()
{
    delete ui;
}

void CheckTable::updateCheckTable(CheckList *l)
{
    this->show();

    if(ui->list_table_id->rowCount() == 0)
    {
        showOnlyCheckTable(l);
    }
    else
    {
        setCurCheckTable(l);
    }
    ui->list_table->resizeColumnsToContents();
}

void CheckTable::recvCheckTables(QList<CheckTableInfo *> tabs)
{
    QStringList tabDate;
    if(!listCheckTables.isEmpty())
    {
        qDeleteAll(listCheckTables.begin(), listCheckTables.end());
        listCheckTables.clear();
    }
    listCheckTables = tabs;

    foreach(CheckTableInfo* tab, tabs)
    {
        tabDate<<tab->eTime;
    }
    setCheckTables(tabDate);
}


void CheckTable::on_date_start_dateChanged(const QDate &date)
{
    qDebug()<<date;
    ui->date_finish->setMinimumDate(date);
}

void CheckTable::on_date_finish_dateChanged(const QDate &date)
{
    ui->date_start->setMaximumDate(date);
}

bool CheckTable::eventFilter(QObject *w, QEvent *e)
{
    return QWidget::eventFilter(w,e);
}

void CheckTable::showEvent(QShowEvent *)
{
    ui->date_start->setDisplayFormat("yyyy/MM/dd");
    ui->date_finish->setDisplayFormat("yyyy/MM/dd");
    ui->date_start->setDate(QDate::currentDate());
    ui->date_finish->setDate(QDate::currentDate());
    ui->date_start->setMaximumDate(QDate::currentDate());
    ui->date_finish->setMinimumDate(QDate::currentDate());
    ui->date_finish->setMaximumDate(QDate::currentDate());
}

void CheckTable::setCheckTables(QStringList l)
{
    ui->list_table_id->clearContents();
    ui->list_table_id->setRowCount(l.count());
    ui->list_table_id->setColumnCount(1);
    if(l.isEmpty())
        return;
    for(int i=0; i<l.count(); i++)
    {
        ui->list_table_id->setItem(i, 0, new QTableWidgetItem(l.at(i)));
    }

}
//物品名 编号 规格 入柜 出柜 退货 库存
void CheckTable::setCurCheckTable(CheckList *l)
{
    ui->list_table->clearContents();
    ui->list_table->setColumnCount(7);
    ui->list_table->setRowCount(l->listInfo.count());
    if(l->listInfo.isEmpty())
        return;

    for(int i=0; i<l->listInfo.count(); i++)
    {
        GoodsCheckInfo* info = l->listInfo.at(i);
        qDebug()<<info->num_in;
        ui->list_table->setItem(i, 0, new QTableWidgetItem(info->name));
        ui->list_table->setItem(i, 1, new QTableWidgetItem(info->id));
        ui->list_table->setItem(i, 2, new QTableWidgetItem(info->goodsSize));
        ui->list_table->setItem(i, 3, new QTableWidgetItem(QString::number(info->num_in)));
        ui->list_table->setItem(i, 4, new QTableWidgetItem(QString::number(info->num_out)));
        ui->list_table->setItem(i, 5, new QTableWidgetItem(QString::number(info->num_back)));
        ui->list_table->setItem(i, 6, new QTableWidgetItem(QString::number(info->num_cur)));
    }
}

void CheckTable::showOnlyCheckTable(CheckList *l)
{
    ui->list_table_id->hide();
    ui->check->hide();
    ui->date_start->setDate(l->startTime.date());
    ui->date_finish->setDate(l->finishTime.date());
    setCurCheckTable(l);
}

void CheckTable::on_close_clicked()
{
    ui->check->show();
    ui->list_table_id->show();
    this->hide();
    ui->list_table->clearContents();
    ui->list_table->setRowCount(0);
    ui->list_table_id->clearContents();
    ui->list_table_id->setRowCount(0);
}

void CheckTable::on_check_clicked()
{
    emit askCheckTables(ui->date_start->date(), ui->date_finish->date());
}

void CheckTable::on_list_table_id_cellClicked(int row, int)
{
    if(listCheckTables.isEmpty())
        return;
    if(row > listCheckTables.count())
        return;

    ui->list_table->clearContents();
    ui->list_table->setRowCount(0);
    emit askCheckInfo(listCheckTables.at(row)->id);
}
