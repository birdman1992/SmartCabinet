#ifndef RFREADERCONFIG_H
#define RFREADERCONFIG_H

#include <QObject>
#include <QSettings>
#include <QStringList>
#include "manager/singleton.h"
#include "defines.h"

class RfReaderConfig : public QObject, public Singleton<RfReaderConfig>
{
    Q_OBJECT
    friend class Singleton<RfReaderConfig>;
public:
    QByteArray getConfIntens(QString devName);
    QByteArray getAntPower(QString devName);
    QString getDeviceType(QString devName);
    int getGrandThreshold(QString devName);//梯度阈值
    quint16 getDevicePort(QString devName);
    void setConfIntens(QString devName, QByteArray confIntens);
    void setAntPower(QString devName, QByteArray antPow);
    void setGrandThreshold(QString devName, int grandThre);
    void setDeviceType(QString devName, QString devType);
    void setDevicePort(QString devName, quint16 port);
    void createDevice(QString devName, int port, QString devType);
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
