#ifndef CONTROLDEVICE_H
#define CONTROLDEVICE_H

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QTimer>
#include <QMap>
#include "Device/Hid/qhid.h"
#include "Device/Qextserial/qextserialport.h"
#include "Device/devicesimulate.h"
#include "Device/QDeviceWatcher/qdevicewatcher.h"
#include "cabinetconfig.h"
#include "defines.h"
//#include "Device/SerialPort/qserialport.h"
#include "Structs/goodscar.h"
#ifdef PC
    #define SIMULATE_ON  //打开仿真
#else
    #define LOG_ON //打开日志
#endif
//#define TCP_API  //使用新的api

typedef struct ui{
    long vid;
    long pid;
}USBINFO;

class ControlDevice : public QObject
{
    Q_OBJECT
public:
    explicit ControlDevice(QObject *parent = 0);
    ~ControlDevice();
    bool installGlobalConfig(CabinetConfig *globalConfig);
private:
    QHid* hid_card_reader;//读卡器设备
    QHid* hid_code_scan;//扫码设备
    QextSerialPort* com_lock_ctrl;//柜门锁控制器
    QextSerialPort* com_rfid_gateway;//rfid网关
    QextSerialPort* com_card_reader;//串口读卡器
    DeviceSimulate* dev_simulate;//设备仿真器
    CabinetConfig* config;//全局配置
    QList<QByteArray> lockCtrlCmd;//控制协议
    QList<quint32> list_card_reader_id;
    QList<quint32> list_scan_id;
    QMap<quint32, QHid*> map_dev;
    QDeviceWatcher* devWatcher;
    QTimer* timer_beat;
    GoodsCar curCar;
    QTimer timer;
    bool scanState;
    bool cardReaderState;

    void deviceInit();//设备初始化
    void simulateInit();//仿真器初始化
    void ctrlCmdInit();//协议初始化

    void comLockCtrlInit(int baudRate, int dataBits, int Parity, int stopBits);
    void lockCtrl(int ioNum);
    void lockCtrl(int seqNum, int ioNum);//锁控板编号,控制口编号
    void rfidCtrl(QString id);

    int get_dev_info(char *dev_name, USBINFO *uInfo);
    int get_path();
    void initDeviceIdList();
    void getDevState();
    void comRfidInit(int baudRate, int dataBits, int Parity, int stopBits);
    void comCardReaderInit(int baudRate, int dataBits, int Parity, int stopBits);
    QByteArray tty2UsbData(QByteArray ttyData);
    quint32 deviceId(quint16 vId, quint16 pId);//给hid设备生成一个设备id
signals:
    void cardReaderTimeout();//读卡超时
    void lockCtrlData(QByteArray);//暂无
    void cardReaderData(QByteArray);//当前可用
    void codeScanData(QByteArray);//当前可用
    void readyListData(QString listCode);//预备存入单号

public slots:
    void openCase(int seqNum, int index);//柜格映射IO参数
    void openLock(int seqNum, int index);//直接锁控IO参数
    void getLockState();
    void readyForNewCar(GoodsCar car);
    void hidStateChanged(quint16 pId, quint16 vId, bool action);

private slots:
    void readLockCtrlData();
    void readCardReaderData(QByteArray);
    void readCodeScanData(QByteArray);
    void readRfidData(QByteArray);
    void readRfidGatewayData();
    void timeout();
    void readSerialCardReader();
};

#endif // CONTROLDEVICE_H
