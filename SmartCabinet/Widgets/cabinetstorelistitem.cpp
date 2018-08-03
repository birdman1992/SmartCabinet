#include "cabinetstorelistitem.h"
#include "ui_cabinetstorelistitem.h"
#include <QPainter>
#include <QDebug>

CabinetStoreListItem::CabinetStoreListItem(Goods *goods, CaseAddress addr, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetStoreListItem)
{
    ui->setupUi(this);
//    cabGoods = new Goods(goods);
    cabGoods = goods;
    name = goods->name+QString("(%1)").arg(goods->packageType);
    num = goods->takeCount;
    package_id = goods->packageBarcode;
    pos_seq = addr.cabinetSeqNum;
    pos_index = addr.caseIndex;

    ui->name->setText(name);
    ui->num->setText(QString::number(num));
    ui->add->hide();
    ui->minus->hide();

    updateOptState();
}

CabinetStoreListItem::~CabinetStoreListItem()
{
    delete ui;
}

void CabinetStoreListItem::bindRst(CaseAddress addr)
{
    pos_seq = addr.cabinetSeqNum;
    pos_index = addr.caseIndex;
    cabGoods->pos = QPoint(addr.cabinetSeqNum, addr.caseIndex);

    updateOptState();
}

QString CabinetStoreListItem::itemId()
{
    return package_id;
}

int CabinetStoreListItem::itemNum()
{
    return num;
}

void CabinetStoreListItem::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CabinetStoreListItem::updateOptState()
{
    if(pos_seq == -1)
    {
        ui->opt->setText("绑定柜格");
    }
    else
    {
        ui->opt->setText("存放");
    }
}

void CabinetStoreListItem::on_minus_clicked()
{
    num = (num-1)<0?0:(num-1);
    ui->num->setText(QString::number(num));
}

void CabinetStoreListItem::on_add_clicked()
{
    num += 1;
    ui->num->setText(QString::number(num));
}

void CabinetStoreListItem::on_opt_clicked()
{
    if(pos_seq == -1)
    {
        emit requireBind(cabGoods, this);
    }
    else
    {
        emit requireOpenCase(pos_seq, pos_index);
    }
}
