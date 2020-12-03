#ifndef RFREADERCONFIG_H
#define RFREADERCONFIG_H

#include <QObject>
#include <QSettings>
#include <QStringList>
#include <QBitArray>
#include <QMetaEnum>
#include "manager/singleton.h"
#include "defines.h"

enum DevAction{
    RF_REP=1,//库存
    RF_FETCH=2,//取出
    RF_WARNING=4,//警报
    RF_AUTO=8,//智能
    RF_RANGE_SMALL=16,
};

class RfReaderConfig : public QObject, public Singleton<RfReaderConfig>
{
    Q_OBJECT
    friend class Singleton<RfReaderConfig>;
public:
    QByteArray getConfIntens(QString devName);
    QByteArray getAntPower(QString devName);
    DevAction getDeviceAction(QString devName);
    int getGrandThreshold(QString devName);//梯度阈值
    quint16 getDevicePort(QString devName);
    QBitArray getAntState(QString devName);

    void setConfIntens(QString devName, QByteArray confIntens);
    void setAntPower(QString devName, QByteArray antPow);
    void setGrandThreshold(QString devName, int grandThre);
    void setDeviceAction(QString devName, DevAction devAct);
    void setDevicePort(QString devName, quint16 port);
    void setAntState(QString devName, QBitArray state);
    void createDevice(QString devName, int port, DevAction devType);
    QStringList getConfigGroups();
    void delDevice(QString devName);

private:
    explicit RfReaderConfig(QObject *parent = nullptr);

    QString configPath;
    void removeConfig(QString path);
    QVariant getConfig(QString key, QVariant defaultRet);

    void setConfig(QString key, QVariant value);

    void createDefaultConfig(QString devName);
signals:

};

#endif // RFREADERCONFIG_H
