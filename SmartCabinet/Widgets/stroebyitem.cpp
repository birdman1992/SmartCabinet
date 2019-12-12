#include "stroebyitem.h"
#include "ui_stroebyitem.h"
#include <QDebug>

StroeByItem::StroeByItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StroeByItem)
{
    ui->setupUi(this);
    curList = NULL;
    config = CabinetConfig::config();
}

StroeByItem::~StroeByItem()
{
    delete ui;
}

void StroeByItem::newStoreList(GoodsList *l)
{
    if(l == NULL)
        return;

    curList = l;
    updateStoreList();
}

bool sortByPos(Goods* goods1, Goods* goods2)
{
    return (goods1->pos.x()*100+goods1->pos.y()) < (goods2->pos.x()*100+goods2->pos.y());
}

void StroeByItem::updateStoreList()
{
    if(curList == NULL)
        return;

    ui->listId->setText(curList->barcode);

    //get goods pos
    foreach(Goods* goods,curList->list_goods)
    {
        QPoint addr = SqlManager::searchByPackageId(goods->packageId);
        goods->pos = QPoint(addr.x(), addr.y());
    }

    //sort by pos
    qSort(curList->list_goods.begin(), curList->list_goods.end(), sortByPos);

    //show in list
    clearList();
    ui->goodsList->setRowCount(curList->list_goods.count());
    ui->goodsList->setColumnCount(1);
    CabinetStoreListItem* item;

    for(int i=0; i<curList->list_goods.count(); i++)
    {
        Goods* goods = curList->list_goods.at(i);
        qDebug()<<"storeStart"<<goods->abbName<<goods->pos;
        CaseAddress addr;
        addr.setAddress(goods->pos);
        item = new CabinetStoreListItem(goods, addr);
        connect(item, SIGNAL(requireBind(Goods*,CabinetStoreListItem*)), this, SLOT(itemBind(Goods*,CabinetStoreListItem*)));
        connect(item, SIGNAL(requireOpenCase(int,int)), this, SIGNAL(requireOpenCase(int,int)));
        list_item<<item;
        ui->goodsList->setCellWidget(i, 0, item);
    }
}

void StroeByItem::clearList()
{
    if(!list_item.isEmpty())
    {
        qDeleteAll(list_item.begin(), list_item.end());
        list_item.clear();
    }

    ui->goodsList->clear();
    ui->goodsList->setRowCount(0);
}

void StroeByItem::itemBind(Goods* goods, CabinetStoreListItem* item)
{
    bindItem = item;
    emit requireBind(goods);
}
