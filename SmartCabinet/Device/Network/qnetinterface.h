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
    bool numPointCheck(QString str);//数点字符串校验
    void setIp(QString _ip);
    void setNetmask(QString _netmask);
    void setGateway(QString _gateway);

    bool isValid();

private:
    QString devName;
    QNetworkInterface interface;
    QNetworkAddressEntry netEntry;
    QNetworkInterface getNetworkInterface(QString name);
    void getNetworkInfo();
signals:

public slots:
};

#endif // QNETINTERFACE_H
