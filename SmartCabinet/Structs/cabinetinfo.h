#ifndef CABINETINFO_H
#define CABINETINFO_H
#include <QString>
#include <QList>
#include <QFont>
#include <QStringList>

class GoodsInfo
{
public:
    GoodsInfo();
    GoodsInfo(GoodsInfo &info);
    void resetInfo(GoodsInfo &info);
    QString nameWithType();
    GoodsInfo &operator=(GoodsInfo &info);

    QString abbName;//简称
    QString name;//耗材名
    QString id;//耗材id
    QString packageId;
    QString unit;//计数单位
    QString size;
    int goodsType;//打包规格
    int num;//耗材数量
    int outNum;//取出数量
    int col;
    int row;
    int price;
    QString Py;
    QStringList traceIds;
};

class CabinetInfo//单个柜格的耗材信息
{
public:
    CabinetInfo();
    void clearList();
    int caseSearch(QString name);
    int barcodeSearch(QString id);
    int goodsIdSearch(QString id);
    int ctrlSeq;
    int ctrlIndex;
    void clearFetchNum();
    QStringList caseShowStr(QFont font, int maxWidth);
    QList<GoodsInfo*> list_goods;
private:
    QString geteElidedText(QFont font, QString str, int MaxWidth);
};

#endif // CABINETINFO_H
