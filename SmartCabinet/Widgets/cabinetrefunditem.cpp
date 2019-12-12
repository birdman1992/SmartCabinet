#include "cabinetrefunditem.h"
#include "ui_cabinetrefunditem.h"
#include <QPainter>

CabinetRefundItem::CabinetRefundItem(Goods* info, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetRefundItem)
{
    ui->setupUi(this);
    name = info->nameWithType();
    package_id = info->packageId;
    maxNum = info->num;
    num = 0;

    ui->name->setText(name);
    ui->num->setText(QString::number(num));
    ui->maxNum->setText(QString::number(maxNum));
}

CabinetRefundItem::~CabinetRefundItem()
{
    delete ui;
}

int CabinetRefundItem::itemNum()
{
    return num;
}

QString CabinetRefundItem::itemId()
{
    return package_id;
}

bool CabinetRefundItem::itemAdd()
{
//    if(num >= maxNum)
//    {
//        num = maxNum;
//        ui->num->setText(QString::number(num));
//        return false;
//    }
//    else
//    {
        num++;
        ui->num->setText(QString::number(num));
        return true;
//    }
}

void CabinetRefundItem::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
