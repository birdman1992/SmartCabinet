#include "qsocketcan.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "manager/signalmanager.h"

#include <QDebug>
QMutex dataLock;

QSocketCan::QSocketCan(QObject *parent):
    QThread(parent)
{
    s = -1;
    canState = 0;
    initCacheList();
    SignalManager* sigMan = SignalManager::manager();
    connect(this, SIGNAL(doorState(int, bool)), sigMan, SIGNAL(doorState(int, bool)));
}

void QSocketCan::run()
{
    int nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    struct can_filter rfilter[1];


    /* create socket */
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
        perror("Create socket failed");
        s = -1;
        exit(-1);
    }

    /* set up can interface */
    strcpy(ifr.ifr_name, "can0");
    printf("can port is %s\n",ifr.ifr_name);
    /* assign can device */
    ioctl(s, SIOCGIFINDEX, &ifr);
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    /* bind can device */
    if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Bind can device failed\n");
        close(s);
        exit(-2);
    }

    /* configure receiving */
        /* set filter for only receiving packet with can id 0x1F */
    rfilter[0].can_id = 0x100;
    rfilter[0].can_mask = 0xfff0;
    if(setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter)) < 0)
    {
        perror("set receiving filter error\n");
        close(s);
        exit(-3);
    }

    qDebug("can device ok");
    emit canDevOK();

    /* keep reading */
    while(1)
    {
        nbytes = read(s, &frame, sizeof(frame));
        if(nbytes > 0)
        {
            int id = frame.can_id & 0x0f;
            QByteArray canFrame = QByteArray((char*)frame.data, frame.can_dlc);
//            qDebug()<<"[can frame]"<<id<<canFrame.toHex();
            QByteArray pac = list_cache[id]->appendData(canFrame);
            quint16 mCode = list_cache[id]->getMagicCode();
//            qDebug()<<"<<<mcode>>>"<<mCode<<(mCode & 0xff00)<<(mCode & 0x00ff);
            if((mCode & 0xff00) == 0x0000)//关门
            {
                qDebug()<<"[close] door:"<<id;
                emit doorState(id, false);
            }
            else if((mCode & 0xff00) == 0x0100)//开门
            {
                qDebug()<<"[open] door:"<<id;
                emit doorState(id, true);
            }
            if((mCode & 0x00ff) == 0x0001)//指纹模块触发
            {
                qDebug()<<"[module active]:"<<id;
                emit moduleActive(id);
            }
            if(!pac.isEmpty())
            {
                qDebug()<<"[canData]"<<id<<pac.length()<<":"<<pac.toHex();
                emit canData(id, pac);
                sendNextData(id);
            }
//            printf("%s ID=%#x data length=%d\n", ifr.ifr_name, frame.can_id, frame.can_dlc);
//            for (int i=0; i < frame.can_dlc; i++)
//                printf("%#x ", frame.data[i]);
//            printf("\n");
        }
    }

    close(s);
    return;
}

void QSocketCan::sendCanData(int canId, QByteArray canData)
{
//    struct ifreq ifr;
    if(s == -1)
        return;

    struct can_frame frame;
    int errorFlag;
    /* configure can_id and can data length */
    frame.can_id = canId;
    frame.can_dlc = 8;
    int datalen = canData.size();
    int sendPos = 0;
    while(sendPos < datalen)
    {
        frame.can_id = canId;
        frame.can_dlc = 8;

        if(datalen - sendPos > 8)
        {
            memcpy(frame.data, canData.data()+sendPos, 8);
            errorFlag = 2000;
            while((write(s, &frame, sizeof(frame)) != sizeof(frame)) && errorFlag)
                errorFlag--;
            if(errorFlag == 0)
            {
                qDebug()<<QByteArray((char*)frame.data, frame.can_dlc).toHex();
                qWarning()<<"[SocketCan]:send data failed!";
                return;
            }
//            qDebug()<<QByteArray((char*)frame.data, frame.can_dlc).toHex();
            sendPos += 8;
        }
        else//最后一帧
        {
            frame.can_dlc = datalen - sendPos;
            memcpy(frame.data, canData.data()+sendPos, frame.can_dlc);
            errorFlag = 2000;
            while((write(s, &frame, sizeof(frame)) != sizeof(frame)) && errorFlag)
                errorFlag--;
            if(errorFlag == 0)
            {
                qDebug()<<QByteArray((char*)frame.data, frame.can_dlc).toHex();
                qWarning()<<"[SocketCan]:send data failed!";
                return;
            }
//            qDebug()<<QByteArray((char*)frame.data, frame.can_dlc).toHex();
            sendPos += frame.can_dlc;
        }
    }

//    qDebug()<<"sendPos"<<sendPos;
    canLock(canId);
    qDebug()<<"[SocketCan]"<<canId<<"send data:"<<canData.toHex();
}

void QSocketCan::sendNextData(int canId)
{
    if(list_data[canId].isEmpty())
    {
        canUnlock(canId);
        return;
    }

    sendCanData(canId, list_data[canId].takeFirst());
}

void QSocketCan::sendData(int canId, QByteArray canData)
{
    if(canIsLock(canId))
    {
        list_data[canId]<<canData;
        return;
    }

    sendCanData(canId, canData);
}

void QSocketCan::canLock(int canId)
{
    if(canId > 31)
        return;

    dataLock.lock();
    canState |= 1<<canId;
    dataLock.unlock();
//    qDebug()<<"[LOCK]"<<canState;
}

void QSocketCan::canUnlock(int canId)
{
    if(canId > 31)
        return;

    dataLock.lock();
    canState &= ~(1<<canId);
    dataLock.unlock();
//    qDebug()<<"[ULOCK]"<<canState;
}

bool QSocketCan::canIsLock(int canId)
{
    dataLock.lock();
    bool ret = canState & (1<<canId);
    dataLock.unlock();
//    qDebug()<<"[RLOCK]"<<canState;
    return ret;
}

void QSocketCan::lockClear()
{
    dataLock.lock();
    canState = 0;
    dataLock.unlock();
}

void QSocketCan::initCacheList()
{
    if(!list_cache.isEmpty())
    {
        qDeleteAll(list_cache.begin(), list_cache.end());
        list_cache.clear();
    }

    for(int i=0; i<16; i++)
    {
        list_cache<<new ResponsePack;
        QList<QByteArray> l;
        list_data<<l;
    }
}
