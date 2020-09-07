#ifndef RFIDDEVHUB_H
#define RFIDDEVHUB_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QMap>
#include <QString>
#include <QList>
#include <QVariantMap>
#include <QAbstractTableModel>
#include <QStandardItemModel>

#include "Rfid/RfidTest/rfidreader.h"
#include "manager/rfreaderconfig.h"

class RfidDevHub : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit RfidDevHub(QObject *parent = nullptr);
    void addDevice(QString addr, quint16 port, QString type);
    QList<RfidReader*> deviceList();
    RfidReader* device(QString devIp);
    void delDevice(QString devIp);
public slots:

private:
    enum Col{
        dev_addr,
        dev_state,
        dev_type,
        dev_del,
        dev_col_count,
    };

signals:
    void reportEpc(QString epc, bool isOutside);
    void devInfoChanged(QList<QVariantMap>);

protected:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
//    QMap<QString>
    QTcpServer* sev;
    QMap<QString, RfidReader*> list_device;
    QList<QVariantMap> list_dev_info;
    QStringList headerList;

private slots:
    void newConnection();
    void updateDevInfo();
    void devStateChanged();
    void initDevices();
};

#endif // RFIDDEVHUB_H
