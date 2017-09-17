#include "networkset.h"
#include "ui_networkset.h"
#include <QPainter>

NetworkSet::NetworkSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetworkSet)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    dev_network = NULL;
    config = NULL;
}

NetworkSet::~NetworkSet()
{
    delete ui;
}

bool NetworkSet::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    return true;
}

void NetworkSet::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.fillRect(this->rect(), QColor(22, 52, 73, 200));
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void NetworkSet::showEvent(QShowEvent *)
{
    initNetwork();
}

void NetworkSet::on_ok_clicked()
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
//    if(ui->gateway->text() != dev_gateway)
//    {
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
//    }
    dev_network->saveNetwork();
    updateNetInfo();
}

void NetworkSet::updateNetInfo()
{
    dev_ip = dev_network->ip();
    dev_gateway = dev_network->gateway();
    dev_netmask = dev_network->netmask();
    qDebug()<<dev_ip<<dev_gateway<<dev_netmask;
    ui->addr->setText(dev_ip);
    ui->netmask->setText(dev_netmask);
    ui->gateway->setText(dev_gateway);
}

void NetworkSet::initNetwork()
{
//    qDebug("[QNetInterface1]");
//    QNetworkInterface::interfaceFromName("eth0");
//    qDebug()<<QNetworkInterface::interfaceFromName("eth1").flags();
//    return;

    if(dev_network != NULL)
        delete dev_network;
#ifdef SIMULATE_ON
    dev_network = new QNetInterface("eth0");
#else
    dev_network = new QNetInterface("eth1");
#endif
//    qDebug("[QNetInterface2]");
    updateNetInfo();
}

void NetworkSet::on_pushButton_clicked()
{
    this->close();
}

void NetworkSet::on_ok_2_clicked()
{
    config->setServerAddress(ui->addr->text());
    emit updateServerAddress();
}
