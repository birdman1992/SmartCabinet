#include "goodslist.h"

GoodsList::GoodsList()
{

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
    map_goods.insert(_goods->goodsId, _goods);
}

void GoodsList::goodsIn(QString goodsId, int num)
{
    Goods *goods = map_goods.value(goodsId);
    goods->curNum+=num;
    goods->finish = (goods->curNum == goods->totalNum);
}

void GoodsList::goodsOut(QString goodsId, int num)
{
    Goods *goods = map_goods.value(goodsId);
    goods->curNum-=num;
    goods->finish = (goods->curNum == goods->totalNum);
}

Goods *GoodsList::getGoodsById(QString goodsId)
{
    return map_goods.value(goodsId);
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

}

Goods::Goods(QString _goodsId , QString goodsName, int goodsNum)
{
    totalNum = goodsNum;
    name = goodsName;
    finish = false;
    curNum = 0;
    goodsId = _goodsId;
}
