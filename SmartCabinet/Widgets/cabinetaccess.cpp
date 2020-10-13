#include "cabinetaccess.h"
#include "ui_cabinetaccess.h"
#include <QDesktopWidget>
#include <QDebug>
#include "defines.h"

CabinetAccess::CabinetAccess(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetAccess)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    QDesktopWidget* desktop = QApplication::desktop(); // =qApp->desktop();也可以
    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);
    isStore = false;//默认为取模式
    ui->frame_pri->hide();
    networkState = false;
    curCab = NULL;

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

void CabinetAccess::setNetworkState(bool state)
{
    networkState = state;
}

bool CabinetAccess::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    return true;
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
    return;//废弃
    if(isStore)
    {
        Goods* storeGoods = storeList->getGoodsById(goodsId);
        curGoods = storeGoods;
//        storeGoods->curNum++;
        QString info = QString("已存入%1%2   共需存入存入%3%4").arg(storeGoods->curNum).arg(storeGoods->unit).arg(storeGoods->totalNum).arg(storeGoods->unit);
        ui->name->setText(storeGoods->name);
//        ui->tip->setText("请扫码存入");
        showTips("请扫码存入", false);
        ui->info->setText(info);
        ui->onekey->hide();
        if(this->isHidden())
            this->show();
    }
}

void CabinetAccess::clickOpen()
{
    ui->frame_pri->hide();
    if(config->state == STATE_FETCH)
    {
        ui->cancel->hide();
        ui->onekey->hide();
        defaultValue = true;
        ui->info->clear();

//        curCab = info;
        ui->name->setText(QString());
//        ui->tip->setText("请取货并扫描条形码");
        showTips("请取货并扫描条形码", false);

        if(this->isHidden())
            this->show();
    }
    else if(config->state == STATE_REFUN)
    {
        ui->cancel->hide();
        ui->onekey->hide();
        defaultValue = true;
        ui->info->clear();

//        curCab = info;
        ui->name->setText(QString());
//        ui->tip->setText("请扫描条码退货");
        showTips("请扫描条码退货", false);

        if(this->isHidden())
            this->show();
    }
    if(config->state == STATE_BACK)
    {
        ui->cancel->hide();
        ui->onekey->hide();
        defaultValue = true;
        ui->info->clear();

//        curCab = info;
        ui->name->setText(QString());
//        ui->tip->setText("请取货并扫描条形码");
        showTips("请扫描条码还货", false);

        if(this->isHidden())
            this->show();
    }
}

//void CabinetAccess::scanOpen(QString goodsId)
//{
//    if(isStore)
//    {
//        if(curGoods != NULL)
//        {
//            if(curGoods->goodsId != goodsId)//扫描了另一样物品的条码
//            {
//                save();//把当前已经扫描的货物存了
//            }
//        }

//        Goods* storeGoods = storeList->getGoodsById(goodsId);
//        curGoods = storeGoods;
//        if(storeGoods->curNum >= storeGoods->totalNum)
//        {
//            storeGoods->curNum = storeGoods->totalNum;
//            ui->tip->setText("提示：已全部存入");
//        }
//        else
//        {
//            storeGoods->curNum++;
//            ui->tip->setText("提示：请继续扫描或者点清数量一键存入");
//        }
//        QString info = QString("已存入%1%2   共需存入存入%3%4").arg(storeGoods->curNum).arg(storeGoods->unit).arg(storeGoods->totalNum).arg(storeGoods->unit);
//        ui->name->setText(storeGoods->name);
//        ui->info->setText(info);
//        ui->onekey->show();
//        if(this->isHidden())
//            this->show();
//    }
//    else
//    {
//        addr = config->checkCabinetByBarCode(goodsId);
//        config->list_cabinet[addr.cabinetSeqNUM]->list_case[addr.caseIndex]->list_goods[addr.goodsIndex]->outNum++;
//        ui->name->setText(config->list_cabinet[addr.cabinetSeqNUM]->list_case[addr.caseIndex]->list_goods[addr.goodsIndex]->name);
//        ui->tip->setText("正在取出");
//    }
//}

void CabinetAccess::scanOpen(QString goodsId, QString goodsCode)
{
    ui->frame_pri->hide();
    if(config->state == STATE_STORE)
    {
        if(curGoods != NULL)
        {
            if(curGoods->goodsId != goodsId)//扫描了另一样物品的条码
            {
                save();//把当前已经扫描的货物存了
            }
        }

        ui->cancel->hide();
        ui->onekey->hide();
        Goods* storeGoods = storeList->getGoodsById(goodsId);
        curGoods = storeGoods;
        if(storeGoods->curNum >= storeGoods->totalNum)
        {
            storeGoods->curNum = storeGoods->totalNum;
//            ui->tip->setText("提示：已全部存入");
            showTips("提示：已全部存入",false);
        }
        else
        {
            storeGoods->curNum++;
//            ui->tip->setText("正在存入");
            showTips("正在存入", false);
        }
        ui->name->setText(storeGoods->name);

//        ui->onekey->show();
        if(this->isHidden())
            this->show();
    }
    else if(config->state == STATE_FETCH)
    {
        if(curGoods)
            delete curGoods;

        curGoods = SqlManager::searchGoodsByCode(goodsCode);
        if(curGoods)
        {
            curGoods->codes<<goodsCode;
    //        addr = config->checkCabinetByBarCode(goodsId);
    //        qDebug()<<"fetch1"<<config->list_cabinet[addr.cabinetSeqNum]->list_case[addr.caseIndex]->list_goods[addr.goodsIndex]->outNum;
    //        config->list_cabinet[addr.cabinetSeqNum]->list_case[addr.caseIndex]->list_goods[addr.goodsIndex]->outNum++;
    //        qDebug()<<"fetch outnum"<<config->list_cabinet[addr.cabinetSeqNum]->list_case[addr.caseIndex]->list_goods[addr.goodsIndex]->outNum;
            ui->name->setText(curGoods->name);
            ui->info->setText(goodsCode);
            //        ui->tip->setText("正在取出");
            if(networkState)
            {
                showTips("正在取出", false);
            }
            else
            {
                showTips(QString("%1\n取出成功,离线取货请检查物品有效期").arg(goodsCode) ,false);
            }
        }
        else
        {
            showTips("不识别的条码", false);
        }

    }
    else if(config->state == STATE_REFUN)
    {
        if(curGoods)
            delete curGoods;

        curGoods = SqlManager::searchGoodsByCode(goodsCode);
        curGoods->codes<<goodsCode;
        if(curGoods)
        {
            ui->name->setText(curGoods->name);
            //        ui->tip->setText("正在退货");
            showTips("正在退货", false);
        }
        else
        {
            showTips("不识别的条码", false);
        }
    }
    else if(config->state == STATE_BACK)
    {
//        if(curGoods)
//            delete curGoods;

//        curGoods = SqlManager::searchGoodsByCode(goodsCode);
//        if(curGoods)
//        {
//            curGoods->codes<<goodsCode;
//            ui->name->setText(curGoods->name);
        showTips("正在还货", false);
//        }
//        else
//        {
//            showTips("无效条码", false);
//        }
    }
}


void CabinetAccess::save()
{
    if(isStore)
    {
        if(curGoods == NULL)
            return;
//        int storeNum = curGoods->curNum-curGoods->storeNum;
        curGoods->storeNum = curGoods->curNum;
//        emit saveStore(curGoods, storeNum);
        curGoods = NULL;
    }
    else
    {
        if(curCab == NULL)
            return;

        curCab->clearFetchNum();
        curCab = NULL;
//        emit saveFetch(curCab->name,ui->info->text().toInt());
    }
}

void CabinetAccess::storeFailed(QString msg)
{
    curGoods->curNum--;
    showTips(msg, true);
}

void CabinetAccess::fetchFailed(QString msg)
{
//    if(config->list_cabinet[addr.cabinetSeqNUM]->list_case[addr.caseIndex]->list_goods[addr.goodsIndex]->outNum > 0)
//        config->list_cabinet[addr.cabinetSeqNum]->list_case[addr.caseIndex]->list_goods[addr.goodsIndex]->outNum--;
//        qDebug()<<"fetchFailed outnum"<<config->list_cabinet[addr.cabinetSeqNum]->list_case[addr.caseIndex]->list_goods[addr.goodsIndex]->outNum;
//    else
//        config->list_cabinet[addr.cabinetSeqNUM]->list_case[addr.caseIndex]->list_goods[addr.goodsIndex]->outNum = 0;
    showTips(msg, true);
}

void CabinetAccess::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.fillRect(this->rect(), QColor(22, 52, 73, 200));
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CabinetAccess::showEvent(QShowEvent *)
{
    keyBoard->move(this->geometry().x()+this->geometry().width()+10, this->geometry().y());
}

void CabinetAccess::showTips(QString msg, bool isDangerous)
{
    if(isDangerous)
    {
        ui->tip->setStyleSheet("font: 20px \"微软雅黑\";\
                               color: rgb(215,26,33);\
                               background-color: rgb(255, 255, 255);");
    }
    else
    {
        ui->tip->setStyleSheet("font: 20px \"微软雅黑\";\
                               color: rgb(40, 122, 35);\
                               background-color: rgb(255, 255, 255);");
    }
    ui->tip->setText(msg);
}

void CabinetAccess::on_onekey_clicked()
{
    curGoods->curNum = curGoods->totalNum;
    QString info = QString("已存入%1%2   共需存入存入%3%4").arg(curGoods->curNum).arg(curGoods->unit).arg(curGoods->totalNum).arg(curGoods->unit);
    ui->name->setText(curGoods->name);
//    ui->tip->setText("提示：已全部存入");
    showTips("提示：已全部存入", false);
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
//    if(!isStore)
//    {
    this->hide();
    keyBoard->hide();
//    if(config->state == STATE_FETCH)

//    }
//    else
//    {
//        ui->tip->setText("正在存入");
//    }
}

void CabinetAccess::input(int val)
{
    if(defaultValue)
    {
        ui->info->setText("1");
    }
    strInput = ui->info->text();
    strInput += QString::number(val);
    int numInput = strInput.toInt();

    if(curCab != NULL)
    {
//        numInput = (numInput>curCab->num)?curCab->num:numInput;
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
    ui->pri_single->clear();
    ui->pri_total->clear();
}

void CabinetAccess::recvOptGoodsNum(int num)
{
//    qDebug()<<"[recvOptGoodsNum]"<<config->state<<num;
    ui->frame_pri->hide();
    ui->info->clear();
    if(config->state == STATE_STORE)
    {
//        ui->tip->setText("存入成功");
        showTips("存入成功", false);
        QString info = QString("已存入%1%2   共需存入存入%3%4").arg(curGoods->curNum).arg(curGoods->unit).arg(curGoods->totalNum).arg(curGoods->unit);
        ui->info->setText(info);
//        this->hide();
    }
    else if(config->state == STATE_FETCH)
    {
        ui->info->setText(QString("已取出，剩余%1×(%2)%3").arg(SqlManager::getGoodsCount(curGoods->packageId)).arg(curGoods->packageType).arg(curGoods->unit));
        showTips("取出成功", false);
    }
    else if(config->state == STATE_REFUN)
    {
//        int outNum = config->list_cabinet[addr.cabinetSeqNum]->list_case[addr.caseIndex]->list_goods[addr.goodsIndex]->outNum;
//        int goodsType = config->list_cabinet[addr.cabinetSeqNum]->list_case[addr.caseIndex]->list_goods[addr.goodsIndex]->goodsType;
//        QString unit = config->list_cabinet[addr.cabinetSeqNum]->list_case[addr.caseIndex]->list_goods[addr.goodsIndex]->unit;
//        ui->info->setText(QString("已退%1×(%5)%2，剩余%3×(%5)%4").arg(outNum).arg(unit).arg(num).arg(unit).arg(goodsType));
//        ui->tip->setText("退货成功");
        showTips("退货成功", false);
    }
    else if(config->state == STATE_BACK)
    {
        ui->info->setText(curGoods->codes.at(0));
        showTips("还货成功", false);
    }
}

void CabinetAccess::show()
{
//    this->showMaximized();
    this->showFullScreen();
}

void CabinetAccess::setPrice(float single, float total)
{
    ui->frame_pri->show();
    ui->pri_single->setText(QString("%1 元").arg(QString::number(single,'f',2)));
    ui->pri_total->setText(QString("%1 元").arg(QString::number(total,'f',2)));
}
