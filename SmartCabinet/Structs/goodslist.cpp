#include "goodslist.h"

GoodsList::GoodsList()
{

}

GoodsList::~GoodsList()
{
    qDeleteAll(list_goods.begin(), list_goods.end());
    list_goods.clear();
}

void GoodsList::addGoods(QString goodsId, QString goodsName, int goodsNum)
{
    Goods* goods = new Goods(goodsId, goodsName, goodsNum);
    list_goods<<goods;
    map_goods.insert(goodsId, goods);
}

void GoodsList::goodsIn(QString goodsId)
{
    Goods *goods = map_goods.value(goodsId);
    goods->curNum++;
    goods->finish = (goods->curNum == goods->totalNum);
}

void GoodsList::goodsOut(QString goodsId)
{
    Goods *goods = map_goods.value(goodsId);
    goods->curNum--;
    goods->finish = (goods->curNum == goods->totalNum);
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



Goods::Goods(QString goodsId ,QString goodsName, int goodsNum)
{
    totalNum = goodsNum;
    name = goodsName;
    finish = false;
    curNum = 0;
    Id = goodsId;
}
