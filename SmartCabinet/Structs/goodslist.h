#ifndef GOODSLIST_H
#define GOODSLIST_H
#include <qstring.h>
#include <qlist.h>
#include <QMap>

class Goods
{
public:
    Goods();
    Goods(QString _goodsId, QString goodsName, int goodsNum);
    Goods(Goods* goods);
    QString name;
    QString abbName;
    QString goodsId;
    QString size;
    QString unit;
    QString packageBarcode;
    QString roomName;
    int packageType;
    int batchNumber;
    int takeCount;
    int singlePrice;
    int inStorageId;
    int totalNum;
    int curNum;//未写入柜子信息的已存入数量
    int storeNum;//写入柜子信息的已存入数量
    bool finish;
};

class GoodsList
{
public:
    GoodsList();
    ~GoodsList();
    void addGoods(Goods* _goods);
    void goodsIn(QString goodsId,int num);
    void goodsOut(QString goodsId, int num);
    bool goodsIsRepeat(Goods* _goods, int *index=NULL);
    Goods* getGoodsById(QString goodsId);
    bool isFinished();
    bool listCheck();

    QMap<QString, Goods*> map_goods;
    QString barcode;
    QList<Goods*> list_goods;
};

#endif // GOODSLIST_H
