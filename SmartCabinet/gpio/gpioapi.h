#ifndef GPIOAPI_H
#define GPIOAPI_H

#include <QObject>
#include <QThread>
#include <QMap>

class GpioApi : public QThread
{
    Q_OBJECT
public:
    enum IO_NUM
    {
        J320_L1 = 205,
        J320_L2 = 204,
        J320_L3 = 149,
        J320_L4 = 147,
        J313_FAN = 158,
        J312_L1 = 159,
        J312_L2 = 161,
        J312_L3 = 163,
        J312_L4 = 165,
    };

public:
    explicit GpioApi(QObject *parent = 0);
    ~GpioApi();
    void printIONums();//打印io号
    void addOutIO(IO_NUM portNum);//添加输出io
    void setOutIO(IO_NUM portNum, int val);//设置输出IO
    void addInIO(IO_NUM portNum);//添加输入io

private:
    void run();
    bool ioExport(IO_NUM port_num);//注册io
    void ioUnExport(IO_NUM port_num);//注销io
    void ioDirectionSet(IO_NUM port_num, bool outIo);//设置io方向,outIo true:输出 false:输入
    int calc_port_num(char port, int num);//计算io号

    QMap<int, IO_NUM> mapListenIo;//监听io列表
    QList<IO_NUM> listOutIo;
    volatile bool runFlag;
signals:
    void ioChanged(GpioApi::IO_NUM io, int value);

public slots:
};

#endif // GPIOAPI_H
