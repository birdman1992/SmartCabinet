#include "qdevicewatcher.h"
#include <QDebug>
#include <QRegExp>
#include <unistd.h>

QDeviceWatcher::QDeviceWatcher(QObject *parent) : QThread(parent)
{
    qDebug()<<"[QDeviceWatcher]";
    memset((void*)buf, 0, sizeof(buf));
    watchDeviceList.clear();
//    this->start();
}

QDeviceWatcher::~QDeviceWatcher()
{
    loopFlag = false;

}

int QDeviceWatcher::addDevice(QString deviceName)
{
    watchDeviceList<<deviceName;
    return watchDeviceList.count();
}

int QDeviceWatcher::removeDevice(QString deviceName)
{
    if(watchDeviceList.isEmpty())
        return 0;

    int devIndex = watchDeviceList.indexOf(deviceName);

    if(devIndex == -1)
        return 0;

    watchDeviceList.removeAt(devIndex);
    return watchDeviceList.count();
}

QStringList QDeviceWatcher::getDevices()
{
    return watchDeviceList;
}

void QDeviceWatcher::run()
{
    netLinkInit();//socket初始化

    int len = 0;
    int i = 0;
    loopFlag = true;
    qDebug("[QDeviceWatcher]:start");

    while(loopFlag)
    {
        len = recv(sockfd, &buf, sizeof(buf), 0);
        if(!loopFlag)
            return;

        if(len<0)
            qDebug("receive error\n");
        else if((len<32)||(len>(signed)sizeof(buf)))
            qDebug("invalid message\n");
//        for(i=0;i<len;i++)
//            if(*(buf+i)=='\0')
//                buf[i]='\n';

        msgFilter(QString(buf));
    }
}

void QDeviceWatcher::netLinkInit()
{
    struct sockaddr_nl sa;
    memset(&sa,0,sizeof(sa));
    sa.nl_family=AF_NETLINK;
    sa.nl_groups=1;
    sa.nl_pid = getpid(); //both is ok
    int buffersize = sizeof(buf);

    sockfd=socket(AF_NETLINK,SOCK_RAW,NETLINK_KOBJECT_UEVENT);
    if(sockfd==-1)
        qDebug("socket creating failed:%s\n",strerror(errno));

    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize));
    if(bind(sockfd,(struct sockaddr *)&sa,sizeof(sa))==-1)
        qDebug("bind error:%s\n",strerror(errno));
}

void QDeviceWatcher::msgFilter(QString msg)
{
//    qDebug()<<"[msgFilter]"<<msg;

    QRegExp reg;
    reg.setPattern(QString("(.*)@.*:([0-9a-fA-F]{4}):([0-9a-fA-F]{4}).*event"));
    int index = msg.indexOf(reg);
//    qDebug()<<index;
    if((index == -1) || (reg.captureCount() != 3))
        return;

//    qDebug()<<reg.captureCount();

    QString actionStr = reg.cap(1);
    quint16 vId = reg.cap(2).toUShort(0,16);
    quint16 pId = reg.cap(3).toUShort(0,16);
//    qDebug()<<"[action]"<<actionStr;
//    qDebug("vid:0x%x pid:0x%x", vId, pId);
//    qDebug("--------------------------------");
    bool action = (actionStr==QString("add"));
    emit deviceStateChanged(pId, vId, action);
}
