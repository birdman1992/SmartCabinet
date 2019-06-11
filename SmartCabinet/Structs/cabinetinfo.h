#ifndef CABINETINFO_H
#define CABINETINFO_H
#include <QString>
#include <QList>
#include <QFont>

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
    QString proName;//生产商
    QString supName;//供应商
    QString optName;//操作人
    QString optTime;//操作时间
    QString size;//规格
    QString batch;//批次
    QString traceId;//条码
    int lifeDay;//效期天数
    int productTime;//生产日期
    int lifeTime;//有效期至
    int threshold;//预警数据(包)
    int maxThreshold;//最大数量(包)
    int goodsType;//打包规格
    int packageCount;//包数
    int goodsCount;//拆散数量
    int num;//耗材数量
    int outNum;//取出数量
    float price;//单价
    float sumCount;//总价
    QString aioInNum;
    QString aioOutNum;

    QString Py;
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
