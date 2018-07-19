#ifndef GOODSCHECKINFO_H
#define GOODSCHECKINFO_H
#include <QString>
#include <QDateTime>
#include <QList>

class GoodsCheckInfo
{
public:
    GoodsCheckInfo();
    GoodsCheckInfo(GoodsCheckInfo& info);
    int outPackNum(int outGoodsNum);
    int addPack();
    int redPack();
    QString name;//耗材名
    QString id;//耗材id
    QString unit;//计数单位
    QString packageBarCode;
    QString goodsSize;
    QString producerName;
    int type;
    int num_in;
    int num_out;
    int num_back;
    int num_cur;
    int num_pack;
};

class CheckTableInfo
{
public:
    CheckTableInfo();
    QString id;
    QString sTime;
    QString eTime;
};

class CheckList
{
public:
    CheckList();
    CheckList(CheckList* l);
    void setCheckDateTime(QString str);
    void addInfo(GoodsCheckInfo*);
    QString checkListId;
    QString strTime;
    QDateTime startTime;
    QDateTime finishTime;
    QString departCode;
    QList<GoodsCheckInfo*> listInfo;
};

#endif // GOODSCHECKINFO_H
