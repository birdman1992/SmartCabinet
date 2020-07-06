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
    QByteArray setConfIntens(QString devName, QByteArray confIntens);
    QByteArray setAntPower(QString devName, QByteArray antPow);

private:
    explicit RfReaderConfig(QObject *parent = nullptr);

    QString configPath;
    void removeConfig(QString path);
    QVariant getConfig(QString key, QVariant defaultRet);
    QStringList getConfigGroups();
    void setConfig(QString key, QVariant value);

    void createDefaultConfig(QString devName);
signals:

};

#endif // RFREADERCONFIG_H
