#include "cabinet.h"
#include "ui_cabinet.h"
#include <QColor>
#include <QDebug>
#include <QTableWidgetItem>

Cabinet::Cabinet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Cabinet)
{
    ui->setupUi(this);
}

Cabinet::~Cabinet()
{
    delete ui;
}

void Cabinet::CabinetInit(int seq, int pos, int num, bool mainCab)
{
    seqNum = seq;
    posNum = pos;
    caseNum = num;
    isMainCabinet = mainCab;

    if(!isMainCabinet)
    {
        ui->logo->hide();
        ui->tableWidget->setRowCount(caseNum);
        return;
    }

    ui->tableWidget->setRowCount(caseNum-2);
}

void Cabinet::setCabPos(int pos)
{
    posNum = pos;
}

void Cabinet::addCase(CabinetInfo *info)
{
    if(list_case.count()>=caseNum)
    {
        qDebug()<<"[addCase]"<<"case is full";
        return;
    }

    int index = list_case.count();
    list_case<<info;
    ui->tableWidget->setItem(index,0,new QTableWidgetItem(info->name));

    if(!info->name.isEmpty())
        ui->tableWidget->item(index,0)->setBackgroundColor(QColor(0, 170, 127));
}

int Cabinet::getIndexByName(QString findName)
{
    int i = 0;

    for(i=0; i<list_case.count(); i++)
    {
        if(findName == list_case.at(i)->name)
            return i;
    }

    return -1;
}

void Cabinet::consumableIn()
{

}

void Cabinet::consumableOut()
{

}

int Cabinet::cabinetPosNum()
{
    return posNum;
}

void Cabinet::on_tableWidget_cellClicked(int row, int)
{

}
