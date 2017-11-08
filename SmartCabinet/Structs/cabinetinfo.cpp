#include "cabinetinfo.h"
#include <QDebug>
#include <QStringList>
#include <QFontMetrics>


CabinetInfo::CabinetInfo()
{
    ctrlIndex = 0;
    ctrlSeq = 0;
}

int CabinetInfo::caseSearch(QString name)
{
    int i = 0;

    for(i=0; i<list_goods.count(); i++)
    {//qDebug()<<"[caseSearch]"<<i<<list_goods.count()<<list_goods.at(i)->name<<name;
        if(list_goods.at(i)->name == name)
        {
            return i;
        }
    }

    return -1;
}

int CabinetInfo::barcodeSearch(QString id)
{
    int i = 0;

    for(i=0; i<list_goods.count(); i++)
    {
        if(list_goods.at(i)->packageId == id)
        {
            return i;
        }
    }
    return -1;
}

int CabinetInfo::goodsIdSearch(QString id)
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

void CabinetInfo::clearFetchNum()
{
    int i = 0;
//    if(list_goods.isEmpty())
//        return;
//    qDebug()<<"clearFetchNum<<<<<<<<<<<<<<"<<list_goods.isEmpty();
    for(i=0; i<list_goods.count(); i++)
    {
        list_goods.at(i)->outNum = 0;
    }
}

QString CabinetInfo::geteElidedText(QFont font, QString str, int MaxWidth)
{
    QFontMetrics fontWidth(font);
    int width = fontWidth.width(str);  //计算字符串宽度
    qDebug()<<font;
//    qDebug()<<"[geteElidedText]"<<str<<fontWidth.width(str)<<MaxWidth<<fontWidth.width("...(10)×50");  //qDebug获取"abcdefg..." 为60
    if(width>=MaxWidth)  //当字符串宽度大于最大宽度时进行转换
    {
        str = fontWidth.elidedText(str,Qt::ElideRight, MaxWidth);  //右部显示省略号
    }
    return str;   //返回处理后的字符串
}

QStringList CabinetInfo::caseShowStr(QFont font, int maxWidth)
{
//    QString ret = QString();
    int i = 0;
    QStringList ret;

    for(i=0; i<list_goods.count(); i++)
    {
        QString str = list_goods.at(i)->name;

        if(!list_goods.at(i)->abbName.isEmpty())
            str = list_goods.at(i)->abbName;

        str = geteElidedText(font, str, maxWidth-67) + "...";


        if(!str.isEmpty())
        {
            str += QString("(%1)×%2").arg(list_goods.at(i)->goodsType).arg(list_goods.at(i)->num);
//            str += "\n";
//            ret += str;
            qDebug()<<"[show str]"<<str;
            ret<<str;
        }
    }

    return ret;
}

GoodsInfo::GoodsInfo()
{
    outNum = 0;
}

GoodsInfo::GoodsInfo(GoodsInfo &info)
{
    name = info.name;
    id = info.id;
    num = info.num;
    packageId = info.packageId;
    unit = info.unit;
    Py = info.Py;
    goodsType = info.goodsType;
    abbName = info.abbName;
    outNum = 0;
}

void GoodsInfo::resetInfo(GoodsInfo &info)
{
    name = info.name;
    id = info.id;
    num = info.num;
    packageId = info.packageId;
    unit = info.unit;
    Py = info.Py;
    goodsType = info.goodsType;
    abbName = info.abbName;
    outNum = info.outNum;
}

QString GoodsInfo::nameWithType()
{
    return QString(name + QString("(%1)").arg(goodsType));
}

GoodsInfo& GoodsInfo::operator=(GoodsInfo &info)
{
    this->name = info.name;
    this->id = info.id;
    this->num = info.num;
    this->packageId = info.packageId;
    this->unit = info.unit;
    this->Py = info.Py;
    this->goodsType = info.goodsType;
    this->abbName = info.abbName;
    this->outNum = info.outNum;
    qDebug()<<"[operator=]"<<this->packageId<<this->num;
    return *this;
}
