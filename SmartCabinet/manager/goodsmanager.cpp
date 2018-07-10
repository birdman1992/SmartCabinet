#include "goodsmanager.h"
#include "defines.h"

GoodsManager* GoodsManager::m = new GoodsManager;

GoodsManager *GoodsManager::manager()
{
    return m;
}

void GoodsManager::addGoodsCodes(QString goodsId, QStringList codes)
{
    setConfig(QString("%1/codes").arg(goodsId), codes);
}

QStringList GoodsManager::getGoodsCodes(QString goodsId)
{
    return getConfig(QString("%1/codes").arg(goodsId), QStringList()).toStringList();
}

QString GoodsManager::getGoodsByCode(QString code)
{
    return codeMap.value(code, QString());
}

QStringList GoodsManager::getGoodsList()
{
    QSettings settings(configPath, QSettings::IniFormat);
    return settings.childGroups();
}

void GoodsManager::removeCode(QString code)
{
    QString goodsId = getGoodsByCode(code);
    QStringList codes = getConfig(QString("%1/codes").arg(goodsId), QStringList());
    codes.removeOne(code);
    setConfig(QString("%1/codes").arg(goodsId), codes);
}

GoodsManager::GoodsManager(QObject *parent) : QObject(parent)
{
    configPath = CONF_GOODS_INFO;
}

void GoodsManager::updateCodeMap()
{
    if(codeMap.isEmpty())
        return;

    codeMap.clear();

    QStringList goodsList = getGoodsList();
    foreach(QString id, goodsList)
    {
        QStringList codes = getGoodsCodes(id);
        foreach(QString code, codes)
        {
            codeMap.insert(code, id);
        }
    }
}

/********base functions*******/
void GoodsManager::setConfig(QString key, QVariant value)
{
    QSettings settings(configPath, QSettings::IniFormat);
    settings.setValue(key, value);
    settings.sync();
}

QVariant GoodsManager::getConfig(QString key, QVariant defaultRet)
{
    QSettings settings(configPath, QSettings::IniFormat);
    return settings.value(key, defaultRet);
}

void GoodsManager::removeConfig(QString path)
{
    QSettings settings(configPath, QSettings::IniFormat);
    settings.remove(path);
}
/********base functions end*******/
