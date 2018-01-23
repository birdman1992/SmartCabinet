#ifndef NETWORKSET_H
#define NETWORKSET_H

#include <QWidget>
#include <QShowEvent>
#include "cabinetconfig.h"
#include "Device/Network/qnetinterface.h"
#include "Device/controldevice.h"

namespace Ui {
class NetworkSet;
}

class NetworkSet : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkSet(QWidget *parent = 0);
    ~NetworkSet();

    bool installGlobalConfig(CabinetConfig *globalConfig);
private slots:
    void on_pushButton_clicked();

    void on_ok_2_clicked();

    void on_ok_clicked();
signals:
    void updateServerAddress();

private:
    Ui::NetworkSet *ui;
    CabinetConfig* config;
    QNetInterface* dev_network;
    QString dev_ip;
    QString dev_netmask;
    QString dev_gateway;
    void paintEvent(QPaintEvent *);
    void showEvent(QShowEvent *);
    void updateNetInfo();
    void initNetwork();
};

#endif // NETWORKSET_H
