#include "cabinet.h"
#include "ui_cabinet.h"

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
        ui->logo->hide();

    ui->tableWidget->setRowCount(caseNum);
}

void Cabinet::addCase(CabinetInfo *info)
{
//    ui->tableWidget->
    list_case<<info;
}
