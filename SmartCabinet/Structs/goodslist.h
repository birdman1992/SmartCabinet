#ifndef GOODSLIST_H
#define GOODSLIST_H
#include <qstring.h>
#include <QStringList>
#include <qlist.h>
#include <QMap>
#include <QPoint>

class Goods
{
public:
    Goods();
    Goods(QString _goodsId, QString goodsName, int goodsNum);
    Goods(Goods* goods);
    QStringList codes;
    QString name;//耗材名
    QString abbName;//简称
    QString goodsId;//耗材id
    QString packageId;//包ID
    QString size;//规格
    QString unit;//计数单位
    QString roomName;
    QString proName;//生产商
    QString supName;//供应商
    QString optName;//操作人
    QString optTime;//操作时间
    QString batch;//批次
    QString traceId;//条码
    int lifeDay;//效期天数
    QString productTime;//生产日期
    QString lifeTime;//有效期至
    int threshold;//预警数据(包)
    int maxThreshold;//最大数量(包)
    int packageType;//打包规格
    int packageCount;//包数
    int goodsCount;//拆散数量
    int num;//耗材数量
    int outNum;//取出数量
    int col;
    int row;
    float price;//单价
    float sumCount;//总价
    QString aioInNum;//入库数
    QString aioOutNum;//出库数

    QString listCode;
    QString Py;
    QPoint pos;
    int batchNumber;
    int takeCount;
    int singlePrice;
    int inStorageId;
    int totalNum;
    int waitNum;
    int curNum;//未写入柜子信息的已存入数量
    int storeNum;//写入柜子信息的已存入数量
    bool finish;
    QString nameWithType();
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

    bool legalList;//存货单是否 true:合法  false:不合法
    QMap<QString, Goods*> map_goods;
    QString barcode;
    QString departName;
    QList<Goods*> list_goods;
};

class StoreList
{
public:
    StoreList();
    ~StoreList();
    QString printTime;
    QString barCode;
    QString departName;//科室
    QString departCode;//柜子号
    QString hosName;//医院
    bool legalList;

    QMap<QString, QString> rfidMap;//rfid:code
    QMap<QString, QString> codeMap;//code:package_id
    QMap<QString, Goods*> goodsMap;//package_id:goods_info
};

#endif // GOODSLIST_H
