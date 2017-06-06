#include "controldevice.h"
#include <QDebug>
#define DEV_LOCK_CTRL "/dev/ttymxc2"
#define DEV_CARD_READER "/dev/hidraw0"
#define DEV_CODE_SCAN "/dev/hidraw1"

ControlDevice::ControlDevice(QObject *parent) : QObject(parent)
{
#ifdef SIMULATE_ON
    simulateInit();
#else
    deviceInit();
#endif
}

//设备初始化
void ControlDevice::deviceInit()
{
    //初始化锁控:波特率,数据位,奇偶校验,停止位
//    com_lock_ctrler = new QSerialPort(DEV_LOCK_CTRL);
//    com_lock_ctrler->com_init(38400,0,8,'N',1);

    comLockCtrlInit(38400, 8, 0, 1);
//    connect(com_lock_ctrl, SIGNAL(readyRead()), this, SLOT(readLockCtrlData()));

    //初始化读卡器
    hid_card_reader = new QHid(this);
    hid_card_reader->hidOpen(DEV_CARD_READER);
    connect(hid_card_reader, SIGNAL(hidRead(QByteArray)), this, SLOT(readCardReaderData(QByteArray)));

    //初始化扫码设备
    hid_code_scan = new QHid(this);
    hid_code_scan->hidOpen(DEV_CODE_SCAN);
    connect(hid_code_scan, SIGNAL(hidRead(QByteArray)), this, SLOT(readCodeScanData(QByteArray)));
}

void ControlDevice::simulateInit()
{
    dev_simulate = new DeviceSimulate();
    dev_simulate->show();
    connect(dev_simulate, SIGNAL(sendCardReaderData(QByteArray)), this, SLOT(readCardReaderData(QByteArray)));
    connect(dev_simulate, SIGNAL(sendCodeScanData(QByteArray)), this, SLOT(readCodeScanData(QByteArray)));
}

void ControlDevice::ctrlCmdInit()
{

}

void ControlDevice::comLockCtrlInit(int baudRate, int dataBits, int Parity, int stopBits)
{
    com_lock_ctrl = new QextSerialPort(DEV_LOCK_CTRL);
    //设置波特率
    com_lock_ctrl->setBaudRate((BaudRateType)baudRate);
//    qDebug() << (BaudRateType)baudRate;
    //设置数据位
    com_lock_ctrl->setDataBits((DataBitsType)dataBits);
    //设置校验
    switch(Parity){
    case 0:
        com_lock_ctrl->setParity(PAR_NONE);
        break;
    case 1:
        com_lock_ctrl->setParity(PAR_ODD);
        break;
    case 2:
        com_lock_ctrl->setParity(PAR_EVEN);
        break;
    default:
        com_lock_ctrl->setParity(PAR_NONE);
        qDebug("set to default : PAR_NONE");
        break;
    }
    //设置停止位
    switch(stopBits){
    case 1:
        com_lock_ctrl->setStopBits(STOP_1);
        break;
    case 0:
        qDebug() << "linux system can't setStopBits : 1.5!";
        break;
    case 2:
        com_lock_ctrl->setStopBits(STOP_2);
        break;
    default:
        com_lock_ctrl->setStopBits(STOP_1);
        qDebug("set to default : STOP_1");
        break;
    }
    //设置数据流控制
    com_lock_ctrl->setFlowControl(FLOW_OFF);
//    com_lock_ctrl->setTimeout(5000);

    if(com_lock_ctrl->open(QIODevice::ReadWrite)){
        qDebug() <<DEV_LOCK_CTRL<<"open success!";
    }else{
        qDebug() <<DEV_LOCK_CTRL<< "未能打开串口"<<":该串口设备不存在或已被占用" <<  endl ;
        return;
    }

}

void ControlDevice::lockCtrl(int ioNum)
{
    QByteArray qba = QByteArray::fromHex("FA0100FF");
    qba[2] = ioNum;
    qDebug()<<"[lockCtrl]"<<qba.toHex();
//    com_lock_ctrler->com_write(qba);
    com_lock_ctrl->write(qba);
}

void ControlDevice::openLock(int seqNum, int index)
{
    int ctrlNum = (seqNum <= 0)?index:(6+(seqNum-1)*8+index);
    qDebug()<<"[openLock]"<<seqNum<<index<<ctrlNum;
#ifndef SIMULATE_ON
    lockCtrl(ctrlNum);
#endif
}

void ControlDevice::readLockCtrlData()
{
    QByteArray qba = com_lock_ctrl->readAll();
    qDebug()<<"[readLockCtrlData]"<<qba.toHex();
    emit lockCtrlData(qba);
}

void ControlDevice::readCardReaderData(QByteArray qba)
{
    qDebug()<<"[readCardReaderData]"<<qba;
    emit cardReaderData(qba);
}

void ControlDevice::readCodeScanData(QByteArray qba)
{
    int index = qba.indexOf("-");
    qba = (index==-1)?qba:qba.left(index);
    qDebug()<<"[readCodeScanData]"<<qba;
    emit codeScanData(qba);
}
