#ifndef QNETINTERFACE_H
#define QNETINTERFACE_H

#include <QObject>
#include <QString>
#include <QNetworkInterface>

class QNetInterface : public QObject
{
    Q_OBJECT

public:
    explicit QNetInterface(QString name, QObject *parent = 0);
    QString ip();
    QString netmask();
    QString broadcast();
    void setIp(QString);

    bool isValid();

private:
    QNetworkInterface interface;
    QNetworkAddressEntry netEntry;
    QNetworkInterface getNetworkInterface(QString name);
    void getNetworkInfo();
signals:

public slots:
};

#endif // QNETINTERFACE_H
