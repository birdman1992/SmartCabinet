#include "cabinetaccess.h"
#include "ui_cabinetaccess.h"
#include <QDesktopWidget>

CabinetAccess::CabinetAccess(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetAccess)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    QDesktopWidget* desktop = QApplication::desktop(); // =qApp->desktop();也可以
    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);
    isStore = false;//默认为取模式

    keyBoard = new NumKeyboard();
    connect(keyBoard, SIGNAL(key(int)), this, SLOT(input(int)));
    connect(keyBoard, SIGNAL(backspace()), this,SLOT(backspace()));
    connect(keyBoard, SIGNAL(clearAll()), this, SLOT(clearAll()));

    storeList = NULL;
    curGoods = NULL;
}

CabinetAccess::~CabinetAccess()
{
    delete keyBoard;
    delete ui;
}

void CabinetAccess::setAccessModel(bool store)
{
    isStore = store;
}

void CabinetAccess::setStoreList(GoodsList *list)
{
    storeList = list;
    isStore = true;
}

void CabinetAccess::clickOpen(QString goodsId)
{
    if(isStore)
    {
        Goods* storeGoods = storeList->getGoodsById(goodsId);
        curGoods = storeGoods;
        storeGoods->curNum++;
        QString info = QString("已存入%1%2   共需存入存入%3%4").arg(storeGoods->curNum).arg(storeGoods->unit).arg(storeGoods->totalNum).arg(storeGoods->unit);
        ui->name->setText(storeGoods->name);
        ui->tip->setText("提示：请继续扫描或者点清数量一键存入");
        ui->info->setText(info);
        ui->onekey->show();
        if(this->isHidden())
            this->show();
    }
}

void CabinetAccess::clickOpen(CabinetInfo *info)
{
    if(!isStore)
    {
        ui->info->clear();
        keyBoard->show();
        curCab = info;
        ui->name->setText(curCab->name);
        ui->tip->setText(QString("提示：剩余%1%2,请扫描取出或者输入取出数量").arg(curCab->num).arg(curCab->unit));
        ui->onekey->hide();
        if(this->isHidden())
            this->show();
    }
}

void CabinetAccess::scanOpen(QString goodsId)
{
    if(curGoods != NULL)
    {
        if(curGoods->goodsId != goodsId)//扫描了另一样物品的条码
        {
            save();
        }
    }

    Goods* storeGoods = storeList->getGoodsById(goodsId);
    curGoods = storeGoods;
    if(storeGoods->curNum >= storeGoods->totalNum)
    {
        storeGoods->curNum = storeGoods->totalNum;
        ui->tip->setText("提示：已全部存入");
    }
    else
    {
        storeGoods->curNum++;
        ui->tip->setText("提示：请继续扫描或者点清数量一键存入");
    }
    QString info = QString("已存入%1%2   共需存入存入%3%4").arg(storeGoods->curNum).arg(storeGoods->unit).arg(storeGoods->totalNum).arg(storeGoods->unit);
    ui->name->setText(storeGoods->name);
    ui->info->setText(info);
    ui->onekey->show();
    if(this->isHidden())
        this->show();
}

void CabinetAccess::save()
{
    if(isStore)
    {
        if(curGoods == NULL)
            return;
        int storeNum = curGoods->curNum-curGoods->storeNum;
        curGoods->storeNum = curGoods->curNum;
        emit saveStore(curGoods, storeNum);
        curGoods = NULL;
    }
    else
    {
        if(curCab == NULL)
            return;

        emit saveFetch(curCab->name,ui->info->text().toInt());
    }
}

void CabinetAccess::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CabinetAccess::showEvent(QShowEvent *)
{
    keyBoard->move(this->geometry().x()+this->geometry().width()+10, this->geometry().y());
}

void CabinetAccess::on_onekey_clicked()
{
    curGoods->curNum = curGoods->totalNum;
    QString info = QString("已存入%1%2   共需存入存入%3%4").arg(curGoods->curNum).arg(curGoods->unit).arg(curGoods->totalNum).arg(curGoods->unit);
    ui->name->setText(curGoods->name);
    ui->tip->setText("提示：已全部存入");
    ui->info->setText(info);
}

void CabinetAccess::on_cancel_clicked()
{
    if(isStore)
        curGoods->curNum = 0;
    this->hide();
    keyBoard->hide();
}

void CabinetAccess::on_ok_clicked()
{
    save();
    this->hide();
    keyBoard->hide();
}

void CabinetAccess::input(int val)
{
    strInput = ui->info->text();
    strInput += QString::number(val);
    int numInput = strInput.toInt();

    if(curCab != NULL)
    {
        numInput = (numInput>curCab->num)?curCab->num:numInput;
    }

    ui->info->setText(QString::number(numInput));
}

void CabinetAccess::backspace()
{
    strInput = strInput = ui->info->text();
    strInput.chop(1);
    ui->info->setText(strInput);
}

void CabinetAccess::clearAll()
{
    ui->info->clear();
}