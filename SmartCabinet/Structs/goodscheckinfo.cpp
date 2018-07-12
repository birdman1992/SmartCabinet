#include "goodscheckinfo.h"
#include <QDebug>

GoodsCheckInfo::GoodsCheckInfo()
{
    num_pack = 0;
}

GoodsCheckInfo::GoodsCheckInfo(GoodsCheckInfo &info)
{
    name = info.name;
    id = info.id;
    unit = info.unit;
    goodsSize = info.goodsSize;
    producerName = info.producerName;
    packageBarCode = info.packageBarCode;
    num_in = info.num_in;
    num_out = info.num_out;
    num_back = info.num_back;
    num_cur = info.num_cur;
    num_pack = info.num_pack;
    type = info.type;
}

int GoodsCheckInfo::outPackNum(int outGoodsNum)
{
    outGoodsNum = outGoodsNum>num_cur?num_cur:outGoodsNum;
    num_pack = outGoodsNum/type+(outGoodsNum%type);
    if(num_pack*type>num_cur)
        num_pack--;

    return num_pack*type;
}

int GoodsCheckInfo::addPack()
{
    num_pack++;
    if(num_pack*type>num_cur)
        num_pack--;
//    qDebug()<<"addPack"<<num_pack<<num_pack*type;
    return num_pack*type;
}

int GoodsCheckInfo::redPack()
{
//    qDebug()<<"redPack"<<num_pack;
    if(num_pack == 0)
        return -1;

    num_pack--;
//    qDebug()<<num_pack*type;

    return num_pack*type;
}

CheckList::CheckList()
{

}

CheckList::CheckList(CheckList *l)
{
    strTime = l->strTime;
    startTime = l->startTime;
    finishTime = l->finishTime;
    departCode = l->departCode;
    listInfo = l->listInfo;
}

void CheckList::setCheckDateTime(QString str)
{
    if(str.length()<40)
        return;

    QString timeStart = str.left(19);
    QString timeFinish = str.mid(23,19);
    qDebug()<<"[setCheckDateTime]"<<timeStart<<"-"<<timeFinish;
    startTime = QDateTime::fromString(timeStart, "yyyy-MM-dd hh:mm:ss");
    finishTime = QDateTime::fromString(timeFinish, "yyyy-MM-dd hh:mm:ss");
    strTime = timeStart+"-"+timeFinish;
}

void CheckList::addInfo(GoodsCheckInfo * info)
{
    listInfo<<info;
}


CheckTableInfo::CheckTableInfo()
{

}
