#include "cabinetaccess.h"
#include "ui_cabinetaccess.h"

CabinetAccess::CabinetAccess(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetAccess)
{
    ui->setupUi(this);
    isStore = false;//默认为取模式
    storeList = NULL;
}

CabinetAccess::~CabinetAccess()
{
    delete ui;
}

void CabinetAccess::setAccessModel(bool store)
{
    isStore = store;
}

void CabinetAccess::setStoreList(GoodsList *list)
{
    storeList = list;
}

void CabinetAccess::clickOpen()
{

}

void CabinetAccess::scanOpen(QString goodsId)
{
    Goods* storeGoods = storeList->getGoodsById(goodsId);
    storeGoods->curNum++;
    QString info = QString("已存入%1%2   共需存入存入%3%4").arg(storeGoods->curNum).arg(storeGoods->unit).arg(storeGoods->totalNum).arg(storeGoods->unit);
    ui->name->setText(storeGoods->name);
    ui->tip->setText("提示：请继续扫描或者点清数量一键存入");
    ui->info->setText("info");
    if(this->isHidden())
        this->show();
}

void CabinetAccess::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
