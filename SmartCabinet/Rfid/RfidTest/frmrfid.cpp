#include "frmrfid.h"
#include "ui_frmrfid.h"
#include <QDebug>
#include <QByteArray>

FrmRfid::FrmRfid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmRfid)
{
    ui->setupUi(this);
    rfManager = new RfidManager();
    connect(rfManager, SIGNAL(updateEpcInfo(EpcInfo*)), this, SLOT(updateEpcInfo(EpcInfo*)));
}

FrmRfid::~FrmRfid()
{
    delete ui;
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
    ui->num->setText(QString::number(rowCount));
//    disconnect(rfManager, SIGNAL(updateEpcInfo(EpcInfo*)), this, SLOT(updateEpcInfo(EpcInfo*)));
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
