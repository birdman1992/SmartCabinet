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
    ui->addr->setText(dev_network->ip());
}

bool CabinetService::eventFilter(QObject *w, QEvent *e)
{
    if(w == ui->addr && e->type() == QEvent::FocusIn)
    {
        ui->addr->clear();
    }
    return QWidget::eventFilter(w,e);
}


void CabinetService::on_addr_returnPressed()
{
    this->setFocus();
}

void CabinetService::on_addr_textEdited(const QString &arg1)
{
    qDebug()<<arg1;
}

void CabinetService::on_ok_clicked()
{

}

void CabinetService::on_cancel_clicked()
{

}
