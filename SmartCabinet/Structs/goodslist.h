#ifndef GOODSLIST_H
#define GOODSLIST_H
#include <qstring.h>
#include <qlist.h>
#include <QMap>

class Goods
{
public:
    Goods(QString goodsId, QString goodsName, int goodsNum);
    QString name;
    QString Id;
    int totalNum;
    int curNum;
    bool finish;
};

class GoodsList
{
public:
    GoodsList();
    ~GoodsList();
    void addGoods(QString goodsId, QString goodsName, int goodsNum);
    void goodsIn(QString goodsId);
    void goodsOut(QString goodsId);
    bool listCheck();

    QMap<QString, Goods*> map_goods;
    QString listId;
    QList<Goods*> list_goods;
};

#endif // GOODSLIST_H
