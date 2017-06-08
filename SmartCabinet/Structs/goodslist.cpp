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
    list_goods<<_goods;
    map_goods.insert(_goods->packageBarcode, _goods);
    qDebug()<<"[addtoMap]"<<_goods->packageBarcode;
//       map_goods.insert(_goods->goodsId, _goods);
}

void GoodsList::goodsIn(QString goodsId, int num)
{
    Goods *goods = map_goods.value(goodsId, NULL);
    if(goods == NULL)
        return;
    goods->curNum+=num;
    goods->finish = (goods->curNum == goods->totalNum);
}

void GoodsList::goodsOut(QString goodsId, int num)
{
    Goods *goods = map_goods.value(goodsId, NULL);
    if(goods == NULL)
        return;
    goods->curNum-=num;
    goods->finish = (goods->curNum == goods->totalNum);
}

Goods *GoodsList::getGoodsById(QString goodsId)
{qDebug()<<"check"<<goodsId;
    if(map_goods.isEmpty())
        return NULL;
    return map_goods.value(goodsId, NULL);
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
