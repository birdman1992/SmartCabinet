#include "storelistmanager.h"
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QDebug>

#define STORE_CACHE_PATH "/home/StoreList"

StoreListManager* StoreListManager::m = new StoreListManager;

StoreListManager *StoreListManager::manager()
{
    return m;
}

QStringList StoreListManager::getStoreCacheList()
{
    curList = QString();
    QDir dir(STORE_CACHE_PATH);
    if(!dir.exists())
    {
        dir.mkdir(STORE_CACHE_PATH);
    }
    QStringList nameFilter;
    nameFilter<<"*.lst";
    QStringList files = dir.entryList(nameFilter, QDir::Files|QDir::NoDotAndDotDot|QDir::Readable|QDir::Writable, QDir::Name);
    QStringList ret;
    foreach (QString listCode, files)
    {
        listCode = listCode.split('.', QString::SkipEmptyParts).at(0);
        ret<<listCode;
    }
    qDebug()<<"[StoreCacheList]"<<ret;
    return ret;
}

QStringList StoreListManager::getStoreCacheCodes(QString goodsId)
{
    QString configGroup = goodsId + "/";
    return getConfig(configGroup+"codes", QStringList()).toStringList();
}

void StoreListManager::setStoreList(QString lCode)
{
    curList = lCode;
}

GoodsList *StoreListManager::recoverGoodsList(QString listCode)
{
    configPath = STORE_CACHE_PATH+QString("/%1.lst").arg(listCode);
    qDebug()<<"configpath"<<configPath<<QFile::exists(configPath);
    if(!QFile::exists(configPath))
        return NULL;

    GoodsList* list = new GoodsList;
    list->barcode = getConfig("barcode", QString()).toString();
    list->departName = getConfig("departName", QString()).toString();

    QStringList listGoodsId = getGroups();
    foreach (QString goodsId, listGoodsId)
    {
        QString configGroup = goodsId + "/";
        Goods* goods = new Goods;
        goods->goodsId = goodsId;
        goods->name = getConfig(configGroup+"name", QString()).toString();
        goods->size = getConfig(configGroup+"size", QString()).toString();
        goods->packageType = getConfig(configGroup+"packageType", 1).toInt();
        goods->packageBarcode = getConfig(configGroup+"packageBarcode", QString()).toString();
        goods->proName = getConfig(configGroup+"proName", QString()).toString();
        goods->supName = getConfig(configGroup+"supName", QString()).toString();
        goods->totalNum = getConfig(configGroup+"totalNum", 0).toInt();
        goods->takeCount = goods->totalNum;
        goods->codes = getConfig(configGroup+"codes", QStringList()).toStringList();
        goods->waitNum = goods->totalNum-goods->codes.count();
        goods->pos = getConfig(configGroup+"pos", QPoint()).toPoint();
        list->addGoods(goods);
    }
    return list;
}

void StoreListManager::creatStoreCache(GoodsList *goodsList)
{
    configPath = STORE_CACHE_PATH+QString("/%1.lst").arg(goodsList->barcode);
    removeStoreCache(goodsList->barcode);
    //save list info
    setConfig("barcode", goodsList->barcode);
    setConfig("departName", goodsList->departName);
    //save goods info
    foreach (Goods* goods, goodsList->list_goods)
    {
        QString configGroup = goods->packageBarcode + "/";
        setConfig(configGroup+"name", goods->name);
        setConfig(configGroup+"size", goods->size);
        setConfig(configGroup+"packageType", goods->packageType);
        setConfig(configGroup+"packageBarcode", goods->packageBarcode);
        setConfig(configGroup+"proName", goods->proName);
        setConfig(configGroup+"supName", goods->supName);
        setConfig(configGroup+"totalNum", goods->totalNum);
        setConfig(configGroup+"codes", goods->codes);
        setConfig(configGroup+"pos", goods->pos);
    }
}

void StoreListManager::removeStoreCache(QString listCode)
{
    configPath = STORE_CACHE_PATH+QString("/%1.lst").arg(listCode);
    QFile::remove(configPath);
}

bool StoreListManager::storeGoodsCode(QString goodsCode)
{
    QString goodsId = scanDataTrans(goodsCode);
    QStringList groups = getGroups();
    int idx = groups.indexOf(goodsId);
    if(idx == -1)
    {
        errorMsg = "不识别的物品";
        return false;
    }
    QString configGroup = goodsId + "/";
    QStringList codes = getConfig(configGroup+"codes", QStringList()).toStringList();
    idx = codes.indexOf(goodsCode);
    if(idx != -1)
    {
        errorMsg = "重复扫描的物品";
        return false;
    }
    else
    {
        codes<<goodsCode;
        setConfig(configGroup+"codes", codes);
    }
    return true;
}

QString StoreListManager::getErrorMsg()
{
    QString ret = errorMsg;
    errorMsg = QString();
    return ret;
}

StoreListManager::StoreListManager()
{
    configPath = QString();
}

QString StoreListManager::scanDataTrans(QString code)
{
    QStringList strList = code.split("-", QString::SkipEmptyParts);
    if(strList.count() < 4)
        return QString();

    strList.removeLast();
    strList = strList.mid(strList.count()-2, 2);
    QString ret = strList.join("-");
    return ret;
}

/********base functions*******/
void StoreListManager::setConfig(QString key, QVariant value)
{
    QSettings settings(configPath, QSettings::IniFormat);
    settings.setValue(key, value);
    settings.sync();
}

QVariant StoreListManager::getConfig(QString key, QVariant defaultRet)
{
    QSettings settings(configPath, QSettings::IniFormat);
    return settings.value(key, defaultRet);
}

void StoreListManager::removeConfig(QString path)
{
    QSettings settings(configPath, QSettings::IniFormat);
    settings.remove(path);
}

QStringList StoreListManager::getGroups()
{
    QSettings settings(configPath, QSettings::IniFormat);
    return settings.childGroups();
}
