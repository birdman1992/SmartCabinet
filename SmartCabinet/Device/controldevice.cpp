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

#define DEV_TEMP "/dev/ttymxc1"     //温度
#define DEV_LOCK_CTRL "/dev/ttymxc2"   //底板串口
#define DEV_RFID_CTRL "/dev/ttymxc4"    //rfid网关串口
#define TTY_CARD_READER "/dev/ttymxc3"   //开发板右侧串口
#define DEV_CARD_READER "/dev/hidraw0"
#define DEV_CODE_SCAN "/dev/hidraw1"

char dev_path[2][24] = {{0},{0}};

ControlDevice::ControlDevice(QObject *parent) : QObject(parent)
{
    hid_card_reader = NULL;
    hid_code_scan = NULL;
    initDeviceIdList();
#ifdef SIMULATE_ON
    simulateInit();
#else

#endif
    devWatcher = new QDeviceWatcher(this);
    connect(devWatcher, SIGNAL(deviceStateChanged(quint16 , quint16 , bool )), this, SLOT(hidStateChanged(quint16 , quint16 , bool )));
    devWatcher->start();
    deviceInit();

}

ControlDevice::~ControlDevice()
{
    if(hid_card_reader != NULL)
        hid_card_reader->hidClose();

    if(hid_code_scan != NULL)
        hid_code_scan->hidClose();
}

//设备初始化
void ControlDevice::deviceInit()
{
    qDebug("deviceInit");
    comCardReaderInit(9600, 8, 0, 1);
    connect(com_card_reader, SIGNAL(readyRead()), this, SLOT(readSerialCardReader()));
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
    cardReaderState = false;
    foreach (quint32 devId, list_card_reader_id)
    {
        if(hid_card_reader->hidOpen(devId>>16, devId&0xffff))
        {
            map_dev.insert(devId, hid_card_reader);
            cardReaderState =  true;
            qDebug()<<"[CARD READER] open success";
            break;
        }
    }
    connect(hid_card_reader, SIGNAL(hidRead(QByteArray)), this, SLOT(readCardReaderData(QByteArray)));

    //初始化扫码设备
    hid_code_scan = new QHid(this);
    scanState = false;
    foreach (quint32 devId, list_scan_id)
    {
        if(hid_code_scan->hidOpen(devId>>16, devId&0xffff))
        {
            map_dev.insert(devId, hid_code_scan);
            scanState =  true;
            qDebug()<<"[CARD READER] open success";
            break;
        }
    }
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

void ControlDevice::comCardReaderInit(int baudRate, int dataBits, int Parity, int stopBits)
{
    com_card_reader = new QextSerialPort(TTY_CARD_READER);
    //设置波特率
    com_card_reader->setBaudRate((BaudRateType)baudRate);
//    qDebug() << (BaudRateType)baudRate;
    //设置数据位
    com_card_reader->setDataBits((DataBitsType)dataBits);
    //设置校验
    switch(Parity){
    case 0:
        com_card_reader->setParity(PAR_NONE);
        break;
    case 1:
        com_card_reader->setParity(PAR_ODD);
        break;
    case 2:
        com_card_reader->setParity(PAR_EVEN);
        break;
    default:
        com_card_reader->setParity(PAR_NONE);
        qDebug("set to default : PAR_NONE");
        break;
    }
    //设置停止位
    switch(stopBits){
    case 1:
        com_card_reader->setStopBits(STOP_1);
        break;
    case 0:
        qDebug() << "linux system can't setStopBits : 1.5!";
        break;
    case 2:
        com_card_reader->setStopBits(STOP_2);
        break;
    default:
        com_card_reader->setStopBits(STOP_1);
        qDebug("set to default : STOP_1");
        break;
    }
    //设置数据流控制
    com_card_reader->setFlowControl(FLOW_OFF);
//    com_card_reader->setTimeout(5000);

    if(com_card_reader->open(QIODevice::ReadWrite)){
        qDebug() <<TTY_CARD_READER<<"open success!";
        cardReaderState =  true;
        qDebug()<<"[CARD READER] open success";
    }else{
        qDebug() <<"/dev/ttymxc1"<< "未能打开串口"<<":该串口设备不存在或已被占用" <<  endl ;
        return;
    }

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

//action:true->added false->removed
void ControlDevice::hidStateChanged(quint16 pId, quint16 vId, bool action)
{
    qDebug()<<"[action]"<<action;
    qDebug("vid:0x%x pid:0x%x", vId, pId);
    qDebug("--------------------------------");
    if(action)//added
    {
        quint32 devId = deviceId(vId,pId);
        QHid* dev = map_dev.value(devId, NULL);
        if(dev == NULL)//设备未被成功打开过
        {
            if(list_scan_id.indexOf(devId)>0)//是扫码设备
            {
                if(hid_code_scan->hidOpen(vId, pId))
                {
                    map_dev.insert(devId, hid_code_scan);
                    scanState =  true;
                    qDebug()<<"[CODE SCAN] open success";
                    getDevState();
                }
            }
            else if(list_card_reader_id.indexOf(devId)>0)//是读卡器设备
            {
                if(hid_card_reader->hidOpen(vId, pId))
                {
                    map_dev.insert(devId, hid_card_reader);
                    scanState =  true;
                    qDebug()<<"[CARD READER] open success";
                    getDevState();
                }
            }
        }
        else//设备被成功打开过，重新打开设备
        {
            dev->hidReopen();
        }
    }
    else
    {

    }
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

QByteArray ControlDevice::tty2UsbData(QByteArray ttyData)
{
    return ttyData.mid(6,8);
}

quint32 ControlDevice::deviceId(quint16 vId, quint16 pId)
{
    return (vId<<16)|pId;
}

//0209019B4193A6E703->9B4193A6
void ControlDevice::readSerialCardReader()
{
    QByteArray qba = com_card_reader->readAll().toHex();
    QByteArray usbCartId = tty2UsbData(qba);
    QString upStr = QString(usbCartId);
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
                    snprintf(path, 24, "/dev/%s",pFile->d_name);
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
                        snprintf(dev_path[0],24,"%s",path);
                    else if((usb_info->vid==1534) && (usb_info->pid==4130))	// new card reader
                        snprintf(dev_path[0],24,"%s",path);
                    else if(usb_info->vid==1155 && usb_info->pid==17)	// scan
                        snprintf(dev_path[1],24,"%s",path);
                    else if(usb_info->vid==8208 && usb_info->pid==30264)	// new scan
                        snprintf(dev_path[1],24,"%s",path);
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

void ControlDevice::initDeviceIdList()
{
    list_card_reader_id<<((2303<<16)|9);
    list_card_reader_id<<((1534<<16)|4130);

    list_scan_id<<((1155<<16)|17);
    list_scan_id<<((8208<<16)|30264);
    list_scan_id<<((0x23d0<<16)|0x0c80);
    list_scan_id<<((0x1eab<<16)|0x1d03);
}

void ControlDevice::getDevState()
{
    config->setCardReaderState(cardReaderState);
    config->setCodeScanState(scanState);
    qDebug()<<"[rfid dev]"<<cardReaderState;
    qDebug()<<"[scan dev]"<<scanState;
}
