#include "fingerprint.h"
#include "ui_fingerprint.h"
#include <QDebug>
#include <QTimer>

FingerPrint::FingerPrint(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FingerPrint)
{
    ui->setupUi(this);
//    cmdCache.
    socketCan = new QSocketCan(this);
    socketCan->start();
    connect(socketCan, SIGNAL(moduleActive(int)), this, SLOT(moduleActived(int)));
    connect(socketCan, SIGNAL(canData(int,QByteArray)), this, SLOT(recvFingerData(int,QByteArray)));
    QTimer::singleShot(2000, this, SLOT(on_test_clicked()));
}

FingerPrint::~FingerPrint()
{
    delete ui;
}

void FingerPrint::recvFingerData(int canId, QByteArray data)
{
    ResponsePack* rPack = new ResponsePack(data);
    qDebug()<<data.toHex();
}

void FingerPrint::moduleActived(int id)
{
    emit requireOpenLock(0, id);
}

void FingerPrint::on_test_clicked()
{
    cmdGetImage();
}



void FingerPrint::on_test_upload_clicked()
{

}

void FingerPrint::on_test_download_clicked()
{

}

void FingerPrint::cmdGetImage()
{
    CmdPack* packGetImg = new CmdPack(0x20, 0);
    socketCan->sendData(0x0, packGetImg->packData());

}
