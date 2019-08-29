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
        IO_C_0=64,
        IO_C_2=66,
        IO_C_4=68,
        IO_H_11=235,
        IO_H_7=231,
        IO_H_6=230,
        IO_D_24=120,
        IO_G_11=203,
        IO_G_10=202,
        IO_G_12=204,
        IO_BEEP=205,
        IO_B_2=34,
        IO_B_3=35,
        IO_L_5=357,
        IO_L_4=356,
        IO_L_3=355,
        IO_L_2=354,
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
    void ioExport(IO_NUM port_num);//注册io
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
