#ifndef STORELISTMANAGER_H
#define STORELISTMANAGER_H
#include <QString>
#include <QVariant>
#include <QList>
#include "Structs/goodslist.h"

class StoreListManager
{
public:
    static StoreListManager* manager();
    QStringList getStoreCacheList();//获取未完成存货单缓存列表
    QStringList getStoreCacheCodes(QString goodsId);//通过物品id获取已存入的条码
    void setStoreList(QString lCode);
    GoodsList* recoverGoodsList(QString listCode);
    void creatStoreCache(GoodsList* goodsList);
    void removeStoreCache(QString listCode);
    bool storeGoodsCode(QString goodsCode);//存入一个条码
    QString getErrorMsg();

private:
    StoreListManager();
    void setConfig(QString key, QVariant value);
    QVariant getConfig(QString key, QVariant defaultRet);
    void removeConfig(QString path);
    QStringList getGroups();

    static StoreListManager* m;
    QString errorMsg;
    QString curList;
    QString configPath;
    QString scanDataTrans(QString code);
};

#endif // STORELISTMANAGER_H
