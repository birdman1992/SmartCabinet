#include "cabinetcheckitem.h"
#include "ui_cabinetcheckitem.h"
#include <QPainter>

CabinetCheckItem::CabinetCheckItem(GoodsInfo* info, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetCheckItem)
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

CabinetCheckItem::~CabinetCheckItem()
{
    delete ui;
}

int CabinetCheckItem::itemNum()
{
    return num;
}

QString CabinetCheckItem::itemId()
{
    return package_id;
}

bool CabinetCheckItem::itemAdd()
{
    //取消了盘点数量限制
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

void CabinetCheckItem::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

//void CabinetCheckItem::on_minus_10_clicked()
//{
//    num = (num-10)<0?0:(num-10);
//    ui->num->setText(QString::number(num));
//}

void CabinetCheckItem::on_minus_clicked()
{
    return;//不允许按钮控制
    num = (num-1)<0?0:(num-1);
    ui->num->setText(QString::number(num));
}

void CabinetCheckItem::on_add_clicked()
{
    return;//不允许按钮控制
    itemAdd();
//    num += 1;
//    ui->num->setText(QString::number(num));
}

//void CabinetCheckItem::on_add_10_clicked()
//{
//    num += 10;
//    ui->num->setText(QString::number(num));
//}
