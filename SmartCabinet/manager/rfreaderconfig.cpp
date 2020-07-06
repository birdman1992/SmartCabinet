#include "rfreaderconfig.h"

RfReaderConfig::RfReaderConfig(QObject *parent) :
    QObject(parent),configPath(CONF_RFID_READER)
{

}

/**
 * @brief RfReaderConfig::createDefaultConfig
 * [ip]
 * confIntens=0x500a0a0a0a0a0a0a    天线置信强度
 * antPower=0x1e1e1e1e1e1e1e1e      天线功率
 */
void RfReaderConfig::createDefaultConfig(QString devName)
{
    if(getConfigGroups().contains(devName))
        return;

    setConfig(devName+"/"+"confIntens", QByteArray::fromHex("0x500a0a0a0a0a0a0a"));
    setConfig(devName+"/"+"antPower", QByteArray::fromHex("0x1e1e1e1e1e1e1e1e"));
}

QByteArray RfReaderConfig::getConfIntens(QString devName)
{
    createDefaultConfig(devName);
    return getConfig(devName+"/"+"confIntens",QVariant()).toByteArray();
}

QByteArray RfReaderConfig::getAntPower(QString devName)
{
    createDefaultConfig(devName);
    return getConfig(devName+"/"+"antPower",QVariant()).toByteArray();
}

QByteArray RfReaderConfig::setConfIntens(QString devName, QByteArray confIntens)
{
    setConfig(devName+"/"+"confIntens",confIntens);
}

QByteArray RfReaderConfig::setAntPower(QString devName, QByteArray antPow)
{
    setConfig(devName+"/"+"antPower",antPow);
}

/********base functions*******/
void RfReaderConfig::setConfig(QString key, QVariant value)
{
    QSettings settings(configPath, QSettings::IniFormat);
    settings.setValue(key, value);
    settings.sync();
}

QVariant RfReaderConfig::getConfig(QString key, QVariant defaultRet)
{
    QSettings settings(configPath, QSettings::IniFormat);
    return settings.value(key, defaultRet);
}

QStringList RfReaderConfig::getConfigGroups()
{
    QSettings settings(configPath, QSettings::IniFormat);
    return settings.childGroups();
}

void RfReaderConfig::removeConfig(QString path)
{
    QSettings settings(configPath, QSettings::IniFormat);
    settings.remove(path);
}
/********base functions end*******/
