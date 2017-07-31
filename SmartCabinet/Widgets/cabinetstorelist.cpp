#include "cabinetstorelist.h"
#include "ui_cabinetstorelist.h"
#include <QPainter>

CabinetStoreList::CabinetStoreList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetStoreList)
{
    ui->setupUi(this);
    clearList();
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    bindItem = NULL;
    loginState = false;
}

CabinetStoreList::~CabinetStoreList()
{
    delete ui;
}

void CabinetStoreList::storeStart(GoodsList *l)
{
    list_store = l;
    CabinetStoreListItem* item;
    int i = 0;

    ui->ok->setEnabled(true);
    ui->storeTable->setRowCount(l->list_goods.count());
    ui->storeTable->setColumnCount(1);

    for(i=0; i<list_store->list_goods.count(); i++)
    {
        Goods* goods = l->list_goods.at(i);
        CaseAddress addr = config->checkCabinetByBarCode(goods->packageBarcode);
        item = new CabinetStoreListItem(goods, addr);
        connect(item, SIGNAL(requireBind(Goods*,CabinetStoreListItem*)), this, SLOT(itemBind(Goods*,CabinetStoreListItem*)));
        connect(item, SIGNAL(requireOpenCase(int,int)), this, SIGNAL(requireOpenCase(int,int)));
        list_item<<item;
        ui->storeTable->setCellWidget(i, 0, item);
    }
}

void CabinetStoreList::storeFinish()
{
    clearList();
}

void CabinetStoreList::bindRst(CaseAddress addr)
{
    if(bindItem == NULL)
        return;

    bindItem->bindRst(addr);
}

void CabinetStoreList::storeRst(QString msg, bool success)
{
    if(success)
    {
       ui->ok->setEnabled(false);
       storeFinish();
    }
    else
        ui->ok->setEnabled(true);
    ui->msg->setText(msg);
}

void CabinetStoreList::setLoginState(bool login)
{
    loginState = login;
    ui->ok->setVisible(login);
    if(login)
    {
        ui->msg->clear();
    }
    else
    {
        ui->msg->setText("有待存送货单，请刷卡登录操作");
    }
}

bool CabinetStoreList::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    return true;
}

void CabinetStoreList::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.fillRect(this->rect(), QColor(22, 52, 73, 200));
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CabinetStoreList::clearList()
{
    if(!list_item.isEmpty())
    {
        list_item.clear();
    }
    list_store = NULL;
    ui->storeTable->clear();
    ui->storeTable->setRowCount(0);
}

void CabinetStoreList::itemBind(Goods* goods, CabinetStoreListItem* item)
{
    bindItem = item;
    emit requireBind(goods);
}

void CabinetStoreList::on_ok_clicked()
{
    if(list_store == NULL)
        return;
    ui->msg->setText("正在提交");
    ui->ok->setEnabled(false);
    emit storeList(list_item);
}

void CabinetStoreList::on_back_clicked()
{
    storeFinish();
    this->close();
}
