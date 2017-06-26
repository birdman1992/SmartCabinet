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

    ui->addr->installEventFilter(this);
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
    updateNetInfo();
}

bool CabinetService::eventFilter(QObject *w, QEvent *e)
{
    if(w == ui->addr && e->type() == QEvent::FocusIn)
    {
//        ui->addr->clear();
    }
    return QWidget::eventFilter(w,e);
}

void CabinetService::updateNetInfo()
{
    dev_ip = dev_network->ip();
    dev_gateway = dev_network->gateway();
    dev_netmask = dev_network->netmask();

    ui->addr->setText(dev_ip);
    ui->netmask->setText(dev_netmask);
    ui->gateway->setText(dev_gateway);
}


void CabinetService::on_addr_returnPressed()
{
    this->setFocus();
}

//void CabinetService::on_addr_textEdited(const QString &arg1)
//{
//    qDebug()<<arg1;
//}

void CabinetService::on_ok_clicked()
{
    if(ui->addr->text() != dev_ip)
    {
        if(dev_network->numPointCheck(ui->addr->text()))
        {
            dev_ip = ui->addr->text();
            dev_network->setIp(dev_ip);
            ui->warning->clear();
        }
        else
        {
            ui->warning->setText("地址格式错误");
        }
    }
    if(ui->netmask->text() != dev_netmask)
    {
        if(dev_network->numPointCheck(dev_netmask))
        {
            dev_netmask = ui->netmask->text();
            dev_network->setNetmask(dev_netmask);
            ui->warning->clear();
        }
        else
        {
            ui->warning->setText("地址格式错误");
        }
    }
    if(ui->gateway->text() != dev_gateway)
    {
        if(dev_network->numPointCheck(dev_gateway))
        {
            dev_gateway = ui->gateway->text();
            dev_network->setGateway(dev_gateway);
            ui->warning->clear();
        }
        else
        {
            ui->warning->setText("地址格式错误");
        }
    }
    updateNetInfo();
}

void CabinetService::on_cancel_clicked()
{

}
