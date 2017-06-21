#include "cabinetservice.h"
#include "ui_cabinetservice.h"
#include "defines.h"
#include <QDebug>
#include <arpa/inet.h>
#include "Device/controldevice.h"

CabinetService::CabinetService(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetService)
{
    ui->setupUi(this);
#ifdef SIMULATE_ON
    dev_network = new QNetInterface("eth0");
#else
    dev_network = new QNetInterface("eth1");
#endif
}

CabinetService::~CabinetService()
{
    delete ui;
}

void CabinetService::on_back_clicked()
{
    winSwitch(INDEX_CAB_SHOW);
}

void CabinetService::showEvent(QShowEvent *)
{
    ui->addr->setText(dev_network->ip());
}
