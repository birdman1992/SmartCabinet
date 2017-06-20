#ifndef CABINETINFO_H
#define CABINETINFO_H
#include <QString>
#include <QList>

class GoodsInfo
{
public:
    GoodsInfo();
    GoodsInfo(GoodsInfo &info);
    QString name;//耗材名
    QString id;//耗材id
    QString packageId;
    QString unit;//计数单位
    int num;//耗材数量
    int outNum;//取出数量
    QChar Py;
};

class CabinetInfo//单个柜格的耗材信息
{
public:
    CabinetInfo();
    int caseSearch(QString name);
    int barcodeSearch(QString id);
    int goodsIdSearch(QString id);
    void clearFetchNum();
    QString caseShowStr();
    QList<GoodsInfo*> list_goods;
};

#endif // CABINETINFO_H
