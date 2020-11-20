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
    goods->waitNum = goods->codes.count() - goods->scanCache.count();
    name = goods->name+QString("[%1](%2)").arg(goods->size).arg(goods->packageType);
    num = goods->waitNum;
    goods->rejectList = goods->codes;

    package_id = goods->packageId;
    pos_seq = addr.cabinetSeqNum;
    pos_index = addr.caseIndex;

    ui->name->setText(name);
    ui->num->setText(QString::number(num));
    ui->add->hide();
    ui->minus->hide();

    updateOptState();
}

CabinetStoreListItem::CabinetStoreListItem(Goods *goods, QPoint addr, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetStoreListItem)
{
    ui->setupUi(this);
//    cabGoods = new Goods(goods);
    cabGoods = goods;
    name = goods->name+QString("[%1](%2)").arg(goods->size).arg(goods->packageType);
    goods->waitNum = goods->codes.count() - goods->scanCache.count();
    num = goods->waitNum;
    goods->rejectList = goods->codes;
    package_id = goods->packageId;
    pos_seq = addr.x();
    pos_index = addr.y();

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
    qDebug()<<"bindRst"<<addr.cabinetSeqNum<<addr.caseIndex;
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
    return cabGoods->takeCount;
}

int CabinetStoreListItem::waitNum()
{
    return num;
}

QPoint CabinetStoreListItem::itemPos()
{
    return QPoint(pos_seq, pos_index);
}

bool CabinetStoreListItem::samePos(QPoint pos)
{
    if((pos_seq == pos.x()) && (pos_index == pos.y()))
        return true;
    else
        return false;
}

void CabinetStoreListItem::storeOnePac()
{
    if(num > 0)
        num--;
    if(cabGoods->waitNum > 0)
        cabGoods->waitNum--;
    ui->num->setText(QString::number(num));
    if(num == 0)
    {
        setStoreState(true);
        ui->opt->setChecked(true);
    }
}

Goods *CabinetStoreListItem::itemGoods()
{
    return cabGoods;
}

void CabinetStoreListItem::infoUpdate()
{
    num = cabGoods->waitNum;
    qDebug()<<"infoUpdate"<<num;
    ui->num->setText(QString::number(num));
    updateOptState();
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
    if(num == 0)
        setStoreState(true);
}

void CabinetStoreListItem::setStoreState(bool storeOver)
{
    if(storeOver)
    {
        ui->num->setStyleSheet("background-color: rgb(78, 154, 6);border:1px solid gray;");
    }
    else
    {
        ui->num->setStyleSheet("background-color: rgb(207, 207, 207);border:1px solid gray;");
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
