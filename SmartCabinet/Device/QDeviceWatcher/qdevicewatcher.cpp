#include "qdevicewatcher.h"
#include <QDebug>
#include <QRegExp>
#include <unistd.h>

QDeviceWatcher::QDeviceWatcher(QObject *parent) : QThread(parent)
{
    qDebug()<<"[QDeviceWatcher]";
    memset((void*)buf, 0, sizeof(buf));
    watchDeviceList.clear();
    this->start();
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
    qDebug("run");

    while(loopFlag)
    {
        qDebug("loop");
        len=recvmsg(sockfd,&msg,0);
        qDebug("asdasdasdsad");
        if(!loopFlag)
            return;

        if(len<0)
            printf("receive error\n");
        else if((len<32)||(len>(signed)sizeof(buf)))
            printf("invalid message\n");
        for(i=0;i<len;i++)
            if(*(buf+i)=='\0')
                buf[i]='\n';

        msgFilter(QString(buf));
    }
}

void QDeviceWatcher::netLinkInit()
{
    struct sockaddr_nl sa;
    memset(&sa,0,sizeof(sa));
    sa.nl_family=AF_NETLINK;
    sa.nl_groups=NETLINK_KOBJECT_UEVENT;
    sa.nl_pid = getpid(); //both is ok

    memset(&msg,0,sizeof(msg));

    iov.iov_base=(void *)buf;
    iov.iov_len=sizeof(buf);
    msg.msg_name=(void *)&sa;
    msg.msg_namelen=sizeof(sa);
    msg.msg_iov=&iov;
    msg.msg_iovlen=1;

    sockfd=socket(AF_NETLINK,SOCK_RAW,NETLINK_KOBJECT_UEVENT);
    if(sockfd==-1)
        printf("socket creating failed:%s\n",strerror(errno));
    if(bind(sockfd,(struct sockaddr *)&sa,sizeof(sa))==-1)
        printf("bind error:%s\n",strerror(errno));
}

void QDeviceWatcher::msgFilter(QString msg)
{
    qDebug()<<"[msgFilter]"<<msg;
    qDebug("--------------------------------");

    QRegExp reg;
}
