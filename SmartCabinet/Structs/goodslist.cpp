#include "goodslist.h"
#include <QDebug>

GoodsList::GoodsList()
{
    map_goods.clear();
}

GoodsList::~GoodsList()
{
    qDeleteAll(list_goods.begin(), list_goods.end());
    list_goods.clear();
    map_goods.clear();
}

void GoodsList::addGoods(Goods *_goods)
{
    int repeatIndex = 0;
    if(goodsIsRepeat(_goods, &repeatIndex))
    {
        list_goods[repeatIndex]->takeCount += _goods->takeCount;
    }
    else
    {
        list_goods<<_goods;
        map_goods.insert(_goods->packageBarcode, _goods);
        qDebug()<<"[addtoMap]"<<_goods->packageBarcode<<_goods->abbName;
        //       map_goods.insert(_goods->goodsId, _goods);
    }
}

void GoodsList::goodsIn(QString goodsId, int)
{
    Goods *goods = map_goods.value(goodsId, NULL);
    if(goods == NULL)
        return;

    goods->finish = (goods->curNum >= goods->totalNum);
    qDebug()<<"[goodsIn]"<<goods->finish<<goods->curNum<<goods->totalNum;
}

void GoodsList::goodsOut(QString goodsId, int num)
{
    Goods *goods = map_goods.value(goodsId, NULL);
    if(goods == NULL)
        return;
    goods->curNum-=num;
    goods->finish = (goods->curNum == goods->totalNum);
}

bool GoodsList::goodsIsRepeat(Goods *_goods, int *index)
{
    int i=0;

    if(index != NULL)
        *index = -1;

    for(i=0; i<list_goods.count(); i++)
    {
        if(list_goods.at(i)->packageBarcode == _goods->packageBarcode)
        {
            if(index != NULL)
                *index = i;
            return true;
        }
    }

    return false;
}

Goods *GoodsList::getGoodsById(QString goodsId)
{qDebug()<<"check"<<goodsId;
    if(map_goods.isEmpty())
        return NULL;
    return map_goods.value(goodsId, NULL);
}

bool GoodsList::isFinished()
{
    return listCheck();
}

bool GoodsList::listCheck()
{
    int i = 0;

    for(i=0; i<list_goods.count(); i++)
    {
        if(!list_goods.at(i)->finish)
            return false;
    }
    return true;
}



Goods::Goods()
{
    storeNum = 0;
    curNum = 0;
    finish = false;
    pos = QPoint(-1, -1);
}

Goods::Goods(QString _goodsId , QString goodsName, int goodsNum)
{
    storeNum = 0;
    totalNum = goodsNum;
    name = goodsName;
    finish = false;
    curNum = 0;
    goodsId = _goodsId;
}

Goods::Goods(Goods *goods)
{
    abbName = goods->abbName;
    name = goods->name;
    goodsId = goods->goodsId;
    size = goods->size;
    unit = goods->unit;
    pos = goods->pos;
    codes = goods->codes;
    packageBarcode = goods->packageBarcode;
    roomName = goods->roomName;
    packageType = goods->packageType;
    batchNumber = goods->batchNumber;
    takeCount = goods->takeCount;
    singlePrice = goods->singlePrice;
    inStorageId = goods->inStorageId;
    totalNum = goods->totalNum;
    curNum = goods->curNum;//未写入柜子信息的已存入数量
    storeNum = goods->storeNum;//写入柜子信息的已存入数量
    finish = goods->finish;
}
