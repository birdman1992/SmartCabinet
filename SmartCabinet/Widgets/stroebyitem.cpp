#include "stroebyitem.h"
#include "ui_stroebyitem.h"

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


}

void StroeByItem::updateStoreList()
{
    ui->listId->setText();
}
