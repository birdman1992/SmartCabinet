#ifndef CABINETINFO_H
#define CABINETINFO_H
#include <QString>
#include <QList>

class GoodsInfo
{
public:
    QString name;//耗材名
    QString id;//耗材id
    QString packageId;
    QString unit;//计数单位
    int num;//耗材数量
};

class CabinetInfo//单个柜格的耗材信息
{
public:
    CabinetInfo();
    bool caseSearch(QString name);
    QString caseShowStr();
    QList<GoodsInfo*> list_goods;
};

#endif // CABINETINFO_H
