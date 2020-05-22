#ifndef RFIDDEVHUB_H
#define RFIDDEVHUB_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QMap>
#include <QString>
#include <QList>
#include <QVariantMap>

#include "Rfid/RfidTest/rfidreader.h"

class RfidDevHub : public QObject
{
    Q_OBJECT
public:
    explicit RfidDevHub(QObject *parent = nullptr);
    void addDevice(QHostAddress addr, quint16 port);
    QList<RfidReader*> deviceList();

public slots:

signals:
    void reportEpc(QString,int,int);
    void devInfoChanged(QList<QVariantMap>);

private:
//    QMap<QString>
    QTcpServer* sev;
    QList<RfidReader*> list_device;
    QList<QVariantMap> list_dev_info;

private slots:
    void newConnection();
    void updateDevInfo();

};

#endif // RFIDDEVHUB_H
