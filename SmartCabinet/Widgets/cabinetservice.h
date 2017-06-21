#ifndef CABINETSERVICE_H
#define CABINETSERVICE_H

#include <QWidget>
#include <QNetworkInterface>
#include <QShowEvent>
#include "Device/Network/qnetinterface.h"

namespace Ui {
class CabinetService;
}

class CabinetService : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetService(QWidget *parent = 0);
    ~CabinetService();

signals:
    void winSwitch(int);

private slots:
    void on_back_clicked();

private:
    Ui::CabinetService *ui;
    QNetInterface* dev_network;
    void showEvent(QShowEvent*);
};

#endif // CABINETSERVICE_H
