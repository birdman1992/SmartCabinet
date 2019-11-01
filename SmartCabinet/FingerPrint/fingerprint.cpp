#include "fingerprint.h"
#include "ui_fingerprint.h"
#include <QDebug>
#include <QTimer>

FingerPrint::FingerPrint(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FingerPrint)
{
    ui->setupUi(this);
    socketCan = new QSocketCan(this);
    socketCan->start();
    connect(socketCan, SIGNAL(moduleActive(int)), this, SLOT(moduleActived(int)));
    QTimer::singleShot(2000, this, SLOT(on_test_clicked()));
}

FingerPrint::~FingerPrint()
{
    delete ui;
}

void FingerPrint::moduleActived(int id)
{
    emit requireOpenLock(0, id);
}

void FingerPrint::on_test_clicked()
{
    CmdPack* testPack = new CmdPack(0x40,4,QByteArray::fromHex("0184"));
    QByteArray pData = testPack->packData();
    qDebug()<<pData.toHex();
    socketCan->sendData(0x001,pData);
}



void FingerPrint::on_test_upload_clicked()
{

}

void FingerPrint::on_test_download_clicked()
{

}
