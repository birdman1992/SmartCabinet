#include "goodslist.h"
#include <QDebug>

GoodsList::GoodsList()
{
    legalList = false;
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
        qDebug()<<"addGoods"<<list_goods[repeatIndex]->takeCount<<_goods->takeCount;
        list_goods[repeatIndex]->takeCount += _goods->takeCount;
        list_goods[repeatIndex]->waitNum = list_goods[repeatIndex]->takeCount;
        list_goods[repeatIndex]->totalNum = list_goods[repeatIndex]->takeCount;
    }
    else
    {
        list_goods<<_goods;
        map_goods.insert(_goods->packageId, _goods);
        qDebug()<<"[addtoMap]"<<_goods->packageId<<_goods->abbName;
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
        if(list_goods.at(i)->packageId == _goods->packageId)
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
    scanCache.clear();
    rejectList.clear();
}

Goods::Goods(QString _goodsId , QString goodsName, int goodsNum)
{
    storeNum = 0;
    totalNum = goodsNum;
    name = goodsName;
    finish = false;
    curNum = 0;
    goodsId = _goodsId;
    scanCache.clear();
    rejectList.clear();
}

Goods::Goods(Goods *goods)
{
    rejectList.clear();
    scanCache.clear();
    abbName = goods->abbName;
    name = goods->name;
    goodsId = goods->goodsId;
    size = goods->size;
    unit = goods->unit;
    pos = goods->pos;
    codes = goods->codes;
    packageId = goods->packageId;
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

QString Goods::nameWithType()
{
//    return QString(name + QString("[%1](%2)").arg(abbName.split('/').last()).arg(packageType));
    return QString(name + QString("[%1](%2)").arg(abbName).arg(packageType));
}

StoreList::StoreList()
{

}

StoreList::~StoreList()
{

}
