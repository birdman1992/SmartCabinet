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
    setConfig(devName+"/"+"grandThreshold", 20);
    setConfig(devName+"/"+"port", 8888);
    setConfig(devName+"/"+"type", "inside");
    setConfig(devName+"/"+"antState", QBitArray(8, true));
}

QByteArray RfReaderConfig::getConfIntens(QString devName)
{
    createDefaultConfig(devName);
    return getConfig(devName+"/"+"confIntens",QVariant(QByteArray(8,1))).toByteArray();
}

QByteArray RfReaderConfig::getAntPower(QString devName)
{
    createDefaultConfig(devName);
    return getConfig(devName+"/"+"antPower",QVariant(QByteArray(8,30))).toByteArray();
}

DevAction RfReaderConfig::getDeviceAction(QString devName)
{
    int ret = getConfig(devName+"/"+"type", RF_REP).toInt();
    return DevAction(ret);
}

int RfReaderConfig::getGrandThreshold(QString devName)
{
    createDefaultConfig(devName);
    return getConfig(devName+"/"+"grandThreshold",20).toInt();
}

quint16 RfReaderConfig::getDevicePort(QString devName)
{
    createDefaultConfig(devName);
    return getConfig(devName+"/"+"port",8888).toInt();
}

QBitArray RfReaderConfig::getAntState(QString devName)
{
    return getConfig(devName+"/"+"antState", QBitArray(8, true)).toBitArray();
}

void RfReaderConfig::setConfIntens(QString devName, QByteArray confIntens)
{
    setConfig(devName+"/"+"confIntens",confIntens);
}

void RfReaderConfig::setAntPower(QString devName, QByteArray antPow)
{
    setConfig(devName+"/"+"antPower",antPow);
}

void RfReaderConfig::setGrandThreshold(QString devName, int grandThre)
{
    setConfig(devName+"/"+"grandThreshold",grandThre);
}

void RfReaderConfig::setDeviceAction(QString devName, DevAction devAct)
{
    setConfig(devName+"/"+"type",(int)devAct);
}

void RfReaderConfig::setDevicePort(QString devName,quint16 port)
{
    setConfig(devName+"/"+"port",port);
}

void RfReaderConfig::setAntState(QString devName, QBitArray state)
{
    setConfig(devName+"/"+"antState", state);
}

/**
 * @brief RfReaderConfig::createDevice
 * @param devName 设备IP
 * @param port  设备端口
 * @param devType   设备类型:inside/outside
 */
void RfReaderConfig::createDevice(QString devName, int port, DevAction devType)
{
    if(getConfigGroups().contains(devName))
    {
//        qDebug()<<"The device configuration already exists:"<<devName<<"create failed.";
        return;
    }
    setConfig(devName+"/"+"confIntens", QByteArray::fromHex("500a0a0a0a0a0a0a"));
    setConfig(devName+"/"+"antPower", QByteArray::fromHex("1e1e1e1e1e1e1e1e"));
    setConfig(devName+"/"+"grandThreshold", 20);
    setConfig(devName+"/"+"port", port);
    setConfig(devName+"/"+"type", devType);
    setConfig(devName+"/"+"antState", QBitArray(8, true));
}

void RfReaderConfig::delDevice(QString devName)
{
    if(getConfigGroups().contains(devName))
        removeConfig(devName);
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
