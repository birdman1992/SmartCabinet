#ifndef CONTROLDEVICE_H
#define CONTROLDEVICE_H

#include <QObject>
#include <QByteArray>
#include <QList>
#include "Device/Hid/qhid.h"
#include "Device/Qextserial/qextserialport.h"
#include "Device/devicesimulate.h"
//#include "Device/SerialPort/qserialport.h"

#define SIMULATE_ON  //打开仿真

class ControlDevice : public QObject
{
    Q_OBJECT
public:
    explicit ControlDevice(QObject *parent = 0);
private:
    QHid* hid_card_reader;//读卡器设备
    QHid* hid_code_scan;//扫码设备
    QextSerialPort* com_lock_ctrl;//柜门锁控制器
//    QSerialPort* com_lock_ctrler;
    DeviceSimulate* dev_simulate;//设备仿真器
    QList<QByteArray> lockCtrlCmd;//控制协议

    void deviceInit();//设备初始化
    void simulateInit();//仿真器初始化
    void ctrlCmdInit();//协议初始化

    void comLockCtrlInit(int baudRate, int dataBits, int Parity, int stopBits);
    void lockCtrl(int ioNum);


signals:
    void cardReaderTimeout();//读卡超时
    void lockCtrlData(QByteArray);//暂无
    void cardReaderData(QByteArray);//当前可用
    void codeScanData(QByteArray);//当前可用

public slots:
    void openLock(int seqNum, int index);

private slots:
    void readLockCtrlData();
    void readCardReaderData(QByteArray);
    void readCodeScanData(QByteArray);
};

#endif // CONTROLDEVICE_H
