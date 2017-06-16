#include "cabinetinfo.h"

CabinetInfo::CabinetInfo()
{

}

int CabinetInfo::caseSearch(QString name)
{
    int i = 0;

    for(i=0; i<list_goods.count(); i++)
    {
        if(list_goods.at(i)->name == name)
        {
            return i;
        }
    }

    return -1;
}

int CabinetInfo::goodsSearch(QString id)
{
    int i = 0;

    for(i=0; i<list_goods.count(); i++)
    {
        if(list_goods.at(i)->id == id)
        {
            return i;
        }
    }

    return -1;
}

QString CabinetInfo::caseShowStr()
{
    QString ret = QString();
    int i = 0;

    for(i=0; i<list_goods.count(); i++)
    {
        QString str = list_goods.at(i)->name;
        if(!str.isEmpty())
        {
            str += str+QString("×%1").arg(list_goods.at(i)->num);
            str += "\n";
        }
    }
    return ret;
}

GoodsInfo::GoodsInfo()
{

}

GoodsInfo::GoodsInfo(GoodsInfo &info)
{
    name = info.name;
    id = info.id;
    num = info.num;
    packageId = info.packageId;
    unit = info.unit;
}
