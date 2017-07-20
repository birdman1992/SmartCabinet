#include "cabinetcheckitem.h"
#include "ui_cabinetcheckitem.h"
#include <QPainter>

CabinetCheckItem::CabinetCheckItem(GoodsInfo* info, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetCheckItem)
{
    ui->setupUi(this);
    name = info->name;
    package_id = info->packageId;
    num = info->num;

    ui->name->setText(name);
    ui->num->setText(QString::number(num));
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

void CabinetCheckItem::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CabinetCheckItem::on_minus_10_clicked()
{
    num = (num-10)<0?0:(num-10);
    ui->num->setText(QString::number(num));
}

void CabinetCheckItem::on_minus_clicked()
{
    num = (num-1)<0?0:(num-1);
    ui->num->setText(QString::number(num));
}

void CabinetCheckItem::on_add_clicked()
{
    num += 1;
    ui->num->setText(QString::number(num));
}

void CabinetCheckItem::on_add_10_clicked()
{
    num += 10;
    ui->num->setText(QString::number(num));
}
