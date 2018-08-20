#include "controldevice.h"
#include <linux/hidraw.h>
#include <linux/hiddev.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <QDebug>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#define DEV_LOCK_CTRL "/dev/ttymxc2"   //底板串口
#define DEV_RFID_CTRL "/dev/ttymxc4"    //rfid网关串口
//#define DEV_LOCK_CTRL "/dev/ttymxc3"   //开发板右侧串口
#define DEV_CARD_READER "/dev/hidraw0"
#define DEV_CODE_SCAN "/dev/hidraw1"

char dev_path[2][24] = {0};

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
    qDebug("deviceInit");
    //初始化锁控:波特率,数据位,奇偶校验,停止位
//    com_lock_ctrler = new QSerialPort(DEV_LOCK_CTRL);
//    com_lock_ctrler->com_init(38400,0,8,'N',1);
    get_path();
    qDebug()<<"[get dev path1]"<<dev_path[0];
    qDebug()<<"[get dev path2]"<<dev_path[1];
    //控制串口初始化
    comLockCtrlInit(38400, 8, 0, 1);
    connect(com_lock_ctrl, SIGNAL(readyRead()), this, SLOT(readLockCtrlData()));
//    int ret = com_lock_ctrl->write(QByteArray("test"));
//    qDebug()<<"[write to lock]"<<DEV_LOCK_CTRL<<ret<<QByteArray("test");
    //rfid网关串口初始化
    comRfidInit(38400, 8, 0, 1);
    connect(com_rfid_gateway, SIGNAL(readyRead()), this, SLOT(readRfidGatewayData()));
//    com_rfid_gateway->write(QByteArray::fromHex("fe0700000009ff"));
//    timer.start(1000);
//    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
//    qDebug()<<"[write to rfid]"<<DEV_RFID_CTRL<<ret<<QByteArray::fromHex("fe0700000005ff").toHex();

    //初始化读卡器
    hid_card_reader = new QHid(this);
    hid_card_reader->hidOpen(dev_path[0]);
    connect(hid_card_reader, SIGNAL(hidRead(QByteArray)), this, SLOT(readCardReaderData(QByteArray)));

    //初始化扫码设备
    hid_code_scan = new QHid(this);
    hid_code_scan->hidOpen(dev_path[1]);
    connect(hid_code_scan, SIGNAL(hidRead(QByteArray)), this, SLOT(readCodeScanData(QByteArray)));
}

bool ControlDevice::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    getDevState();
    return true;
}

void ControlDevice::simulateInit()
{
    qDebug("simulateInit");
    dev_simulate = new DeviceSimulate();
    dev_simulate->show();
    connect(dev_simulate, SIGNAL(sendCardReaderData(QByteArray)), this, SLOT(readCardReaderData(QByteArray)));
    connect(dev_simulate, SIGNAL(sendCodeScanData(QByteArray)), this, SLOT(readCodeScanData(QByteArray)));
    connect(dev_simulate, SIGNAL(sendRfidData(QByteArray)), this, SLOT(readRfidData(QByteArray)));
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

void ControlDevice::comRfidInit(int baudRate, int dataBits, int Parity, int stopBits)
{
    com_rfid_gateway = new QextSerialPort(DEV_RFID_CTRL);
    //设置波特率
    com_rfid_gateway->setBaudRate((BaudRateType)baudRate);
//    qDebug() << (BaudRateType)baudRate;
    //设置数据位
    com_rfid_gateway->setDataBits((DataBitsType)dataBits);
    //设置校验
    switch(Parity){
    case 0:
        com_rfid_gateway->setParity(PAR_NONE);
        break;
    case 1:
        com_rfid_gateway->setParity(PAR_ODD);
        break;
    case 2:
        com_rfid_gateway->setParity(PAR_EVEN);
        break;
    default:
        com_rfid_gateway->setParity(PAR_NONE);
        qDebug("set to default : PAR_NONE");
        break;
    }
    //设置停止位
    switch(stopBits){
    case 1:
        com_rfid_gateway->setStopBits(STOP_1);
        break;
    case 0:
        qDebug() << "linux system can't setStopBits : 1.5!";
        break;
    case 2:
        com_rfid_gateway->setStopBits(STOP_2);
        break;
    default:
        com_rfid_gateway->setStopBits(STOP_1);
        qDebug("set to default : STOP_1");
        break;
    }
    //设置数据流控制
    com_rfid_gateway->setFlowControl(FLOW_OFF);
//    com_rfid_gateway->setTimeout(5000);

    if(com_rfid_gateway->open(QIODevice::ReadWrite)){
        qDebug() <<DEV_RFID_CTRL<<"open success!";
    }else{
        qDebug() <<DEV_RFID_CTRL<< "未能打开串口"<<":该串口设备不存在或已被占用" <<  endl ;
        return;
    }

}

void ControlDevice::openCase(int seqNum, int index)
{
    qDebug()<<"openCase"<<seqNum<<index;
    if((seqNum<0) || (seqNum>=config->list_cabinet.count()))
        return;
    if((index<0) || (index>=config->list_cabinet[seqNum]->list_case.count()))
        return;

    int ctrl_seqNum = config->list_cabinet[seqNum]->list_case[index]->ctrlSeq;
    int ctrl_index = config->list_cabinet[seqNum]->list_case[index]->ctrlIndex;

    lockCtrl(ctrl_seqNum, ctrl_index);
}

void ControlDevice::lockCtrl(int ioNum)
{
    QByteArray qba = QByteArray::fromHex("FA0100FF");
    qba[2] = ioNum;
    qDebug()<<"[lockCtrl]"<<qba.toHex();
//    com_lock_ctrler->com_write(qba);

#ifndef SIMULATE_ON
    com_lock_ctrl->write(qba);
#endif
}

void ControlDevice::lockCtrl(int seqNum, int ioNum)
{
    QByteArray qba = QByteArray::fromHex("fa000100ff");
    qba[1] = seqNum;
    qba[3] = ioNum;
//    qba[1] = config->list_cabinet[seqNum]->list_case[ioNum]->ctrlSeq;
//    qba[3] = config->list_cabinet[seqNum]->list_case[ioNum]->ctrlIndex;
    qDebug()<<"[lockCtrl]"<<qba.toHex();
#ifndef SIMULATE_ON
    com_lock_ctrl->write(qba);
#endif
}

void ControlDevice::rfidCtrl(QString id)
{
    QByteArray qba = QByteArray("fe07ff").insert(4, id.toLocal8Bit());

    qba = QByteArray::fromHex(qba);
    qba[1] = qba.size();
    qDebug()<<"[rfidCtrl]"<<qba.toHex();

#ifndef SIMULATE_ON
    com_rfid_gateway->write(qba);
#endif
}

void ControlDevice::openLock(int seqNum, int index)
{
//    int ctrlNum = (seqNum <= 0)?index:(6+(seqNum-1)*8+index);
//    qDebug()<<"[openLock]"<<seqNum<<index;
//    lockCtrl(ctrlNum);
    QByteArray qba = QByteArray::fromHex("fa000100ff");
    qba[1] = seqNum;
    qba[3] = index;

    lockCtrl(seqNum, index);
}

void ControlDevice::getLockState()
{
    QByteArray qba = QByteArray::fromHex("fa000200ff");
    int i = 0;

#ifndef SIMULATE_ON
    disconnect(com_lock_ctrl, SIGNAL(readyRead()), this, SLOT(readLockCtrlData()));
#endif

    for(i=0; i<config->list_cabinet.count(); i++)
    {
        qba[1] = i;
#ifndef SIMULATE_ON
    com_lock_ctrl->write(qba.data(), qba.size());
    if(com_lock_ctrl->waitForReadyRead(200))
    {
        QByteArray bak = com_lock_ctrl->readAll();
        if(bak[2]|bak[3]|bak[4])//有锁未关
        {
            config->cabVoice.voicePlay(VOICE_CLOSE_DOOR);
            break;
        }
    }
#endif
        qDebug()<<"[getLockState]"<<qba.toHex();
        config->cabVoice.voicePlay(VOICE_CLOSE_DOOR);
    }
#ifndef SIMULATE_ON
    connect(com_lock_ctrl, SIGNAL(readyRead()), this, SLOT(readLockCtrlData()));
#endif
}

void ControlDevice::readyForNewCar(GoodsCar car)
{
    curCar = car;
    qDebug()<<"readyForNewCar"<<curCar.listId<<curCar.rfid;
    rfidCtrl(car.rfid);
}

void ControlDevice::readLockCtrlData()
{
    ::usleep(20000);
    QByteArray qba = com_lock_ctrl->readAll();
    qDebug()<<"[readLockCtrlData]"<<qba.toHex();
//    emit lockCtrlData(qba);
}

void ControlDevice::readRfidGatewayData()
{
    ::usleep(20000);
    QByteArray qba = com_rfid_gateway->readAll();
    qDebug()<<"[readRfidGatewayData]"<<qba.toHex();

    readRfidData(qba);
}

void ControlDevice::timeout()
{
    rfidCtrl("00000005");
}

void ControlDevice::readCardReaderData(QByteArray qba)
{
    QString upStr = QString(qba);
    qba = upStr.toUpper().toLocal8Bit();
    qDebug()<<"[readCardReaderData]"<<qba;
    emit cardReaderData(qba);
}

void ControlDevice::readCodeScanData(QByteArray qba)
{
//    int index = qba.indexOf("-");
//    qba = (index==-1)?qba:qba.left(index);
//    qDebug()<<"[readCodeScanData]"<<qba;
//    emit codeScanData(qba);
    config->clearTimeoutFlag();
    qDebug()<<"[readCodeScanData]"<<qba;
    emit codeScanData(qba);
}

void ControlDevice::readRfidData(QByteArray qba)
{
    qDebug()<<"[readRfidData]"<<qba.toHex();
    if(qba.at(1) != qba.size())
        return;

    if( (qba.at(0)==(char)0xfc) && (qba.at(qba.size()-1)==(char)0xff ) )
    {
        QString rfid = QString(qba.mid(2,4).toHex());
        if(rfid == curCar.rfid)
            emit readyListData(curCar.listId);
    }
}


int ControlDevice::get_dev_info(char *dev_name,USBINFO* uInfo)
{
    int fd;
    int res;
    char buf[256];
    struct hidraw_report_descriptor rpt_desc;
    struct hidraw_devinfo info;

    /* Open the Device with non-blocking reads. In real life,
     don't use a hard coded path; use libudev instead. */
    fd = open(dev_name, O_RDWR|O_NONBLOCK);
    if (fd < 0)
    {
        perror("Unable to open device");
        return 1;
    }
    memset(&rpt_desc, 0x0, sizeof(rpt_desc));
    memset(&info, 0x0, sizeof(info));
    memset(buf, 0x0, sizeof(buf));

    // Get Raw Info
    res = ioctl(fd, HIDIOCGRAWINFO, &info);
    if (res < 0)
        perror("HIDIOCGRAWINFO");
    else
    {
        uInfo->vid = info.vendor;
        uInfo->pid = info.product;
    }
    close(fd);
    return 0;

}
int ControlDevice::get_path(void)
{
    int event = 0;
    int count = 0;
    char path[24] = {"/dev/"};
    DIR* pDir = NULL;
    USBINFO* usb_info = (USBINFO *)malloc(sizeof(USBINFO));
    struct dirent *pFile = NULL;
    if ((pDir=opendir("/dev/")) == NULL)
    {
        printf("Update: Open update directory error!");
        return 0;
    }
    else
    {
        while((pFile = readdir(pDir)) != NULL)
        {
            if(pFile->d_type == 2)		//device
            {
                if(strstr(pFile->d_name,"hidraw")!=NULL)
                {
                    sprintf(path,"/dev/%s",pFile->d_name);
                    get_dev_info(path,usb_info);
                    if(usb_info->vid==3944 && usb_info->pid==22630)	// touch
                    {
                        switch(count)
                        {
                        case 2:
                            event = 4;
                            break;
                        case 1:
                            event = 5;
                            break;
                        case 0:
                            event = 6;
                            break;
                        default:
                            event = 0;
                            break;
                        }
                    }
                    else if((usb_info->vid==2303) && (usb_info->pid==9))	// RFID
                        snprintf(dev_path[0],20,"%s",path);
                    else if(usb_info->vid==1155 && usb_info->pid==17)	// scan
                        snprintf(dev_path[1],20,"%s",path);
                    else if(usb_info->vid==8208 && usb_info->pid==30264)	// new scan
                        snprintf(dev_path[1],20,"%s",path);
                    count++;
                    //	printf("vid: %lu\t,pid: %lu\n",usb_info->vid,usb_info->pid);
                    memset(path,0x00,24);
                }
            }
        }
    }
    closedir(pDir);
    free(usb_info);
    return event;
}

void ControlDevice::getDevState()
{
    config->setCardReaderState( strlen(dev_path[0])!=0 );
    config->setCodeScanState( strlen(dev_path[1])!=0 );
    qDebug()<<"[rfid dev]"<<strlen(dev_path[0]);
    qDebug()<<"[scan dev]"<<strlen(dev_path[1]);
}
