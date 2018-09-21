#ifndef QNETINTERFACE_H
#define QNETINTERFACE_H

#include <QObject>
#include <QString>
#include <QNetworkInterface>
#include <qstringlist.h>

class QNetInterface : public QObject
{
    Q_OBJECT

public:
    explicit QNetInterface(QString name, QObject *parent = 0);
    QString ip();
    QString netmask();
    QString gateway();
    QString macAddress();
    bool numPointCheck(QString str);//数点字符串校验
    bool setIp(QString _ip);
    bool setNetmask(QString _netmask);
    bool setGateway(QString _gateway);
    bool setMacAddress(QString _macAddr);
    void saveNetwork();
    void initNetwork();
    void creatNetwork();

    bool isValid();

private:
    QString devName;
    QString devGateway;
    QNetworkInterface interface;
    QNetworkAddressEntry netEntry;
    QNetworkInterface getNetworkInterface(QString name);
    void getNetworkInfo();
    void setDeviceName(QString name);
signals:

public slots:
};

#endif // QNETINTERFACE_H
