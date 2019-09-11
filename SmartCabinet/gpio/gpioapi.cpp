#include "gpioapi.h"
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <QDebug>


GpioApi::GpioApi(QObject *parent) : QThread(parent)
{
    printIONums();
    listOutIo.clear();
    mapListenIo.clear();
}

GpioApi::~GpioApi()
{
    this->quit();
    this->wait();
    foreach(int fd, mapListenIo.keys())
    {
        ioUnExport(mapListenIo.value(fd));
        close(fd);
    }
    mapListenIo.clear();
}

void GpioApi::printIONums()
{
//    qDebug()<<QString("C_0=%1").arg(calc_port_num('c', 0));
//    qDebug()<<QString("C_2=%1").arg(calc_port_num('c', 2));
//    qDebug()<<QString("C_4=%1").arg(calc_port_num('c', 4));
//    qDebug()<<QString("H_11=%1").arg(calc_port_num('h', 11));
//    qDebug()<<QString("H_7=%1").arg(calc_port_num('h', 7));
//    qDebug()<<QString("H_6=%1").arg(calc_port_num('h', 6));
//    qDebug()<<QString("D_24=%1").arg(calc_port_num('d', 24));
//    qDebug()<<QString("G_11=%1").arg(calc_port_num('g', 11));
//    qDebug()<<QString("G_10=%1").arg(calc_port_num('g', 10));
//    qDebug()<<QString("G_12=%1").arg(calc_port_num('g', 12));
//    qDebug()<<QString("B_2=%1").arg(calc_port_num('b', 2));
//    qDebug()<<QString("B_3=%1").arg(calc_port_num('b', 3));
//    qDebug()<<QString("L_5=%1").arg(calc_port_num('l', 5));
//    qDebug()<<QString("L_4=%1").arg(calc_port_num('l', 4));
//    qDebug()<<QString("L_3=%1").arg(calc_port_num('l', 3));
//    qDebug()<<QString("L_2=%1").arg(calc_port_num('l', 2));
}

void GpioApi::addOutIO(IO_NUM portNum)
{
    if(!ioExport(portNum))
    {
        qDebug()<<"[addOutIO]"<<portNum<<"failed.";
        return;
    }
    ioDirectionSet(portNum, true);
    listOutIo<<portNum;
}

void GpioApi::setOutIO(GpioApi::IO_NUM portNum ,int val)
{
    FILE *p=NULL;
    char str[256];
    if(listOutIo.indexOf(portNum) == -1)
    {
        qDebug()<<"[setOutIO]:"<<portNum<<"not an out io";
    }
    sprintf(str, "/sys/class/gpio/gpio%d/value", portNum);
    p = fopen(str,"w");
    fprintf(p,"%d",val>0 ? 1 : 0);
    fclose(p);
}

void GpioApi::addInIO(IO_NUM portNum)
{
    ioExport(portNum);
    ioDirectionSet(portNum, false);

    char str[256];
    sprintf(str, "/sys/class/gpio/gpio%d/value", portNum);
    int fd = open(str, O_RDONLY);
    if(fd == -1)
    {
        qWarning()<<"[GpioApi]:addInIO: open failed.";
        return;
    }
    mapListenIo.insert(fd, portNum);
}

int	GpioApi::calc_port_num(char port, int num)
{
    int	port_num;

    switch(port){
    case 'a':
    case 'A':
        port_num = num;
        break;
    case 'b':
    case 'B':
        port_num = 32 + num;
        break;
    case 'c':
    case 'C':
        port_num = 64 + num;
        break;
    case 'd':
    case 'D':
        port_num = 96 + num;
        break;
    case 'e':
    case 'E':
        port_num = 128 + num;
        break;
    case 'f':
    case 'F':
        port_num = 160 + num;
        break;
    case 'g':
    case 'G':
        port_num = 192 + num;
        break;
    case 'h':
    case 'H':
        port_num = 224 + num;
        break;
    case 'l':
    case 'L':
        port_num = 352 + num;
        break;
    default:
        port_num = 0;
    }
    return port_num;
}

bool GpioApi::ioExport(IO_NUM port_num)
{
    FILE *p=NULL;

    p = fopen("/sys/class/gpio/export","w");
    if(p == NULL)
        return false;
    fprintf(p,"%d",port_num);
    fclose(p);
    return true;
}

void GpioApi::ioUnExport(IO_NUM port_num)
{
    FILE *p=NULL;

    p = fopen("/sys/class/gpio/unexport","w");
    fprintf(p,"%d",port_num);
    fclose(p);
}

void GpioApi::ioDirectionSet(IO_NUM port_num, bool outIo)
{
    FILE *p=NULL;
    char str[256];

    if(outIo)
    {
        sprintf(str, "/sys/class/gpio/gpio%d/direction", port_num);
        p = fopen(str,"w");
        fprintf(p,"out");
        fclose(p);
    }
    else
    {
        sprintf(str, "/sys/class/gpio/gpio%d/direction", port_num);
        p = fopen(str,"w");
        fprintf(p,"in");
        fclose(p);

        memset(str, 0, sizeof(str));
        sprintf(str, "/sys/class/gpio/gpio%d/edge", port_num);
        p = fopen(str,"w");
        fprintf(p,"none");
        fflush(p);
        fprintf(p,"both");
        fclose(p);
    }
}

void GpioApi::run()
{
    struct pollfd fds[20];
    int i = 0;
    foreach (int fd, mapListenIo.keys())
	{
        fds[i].fd = fd;
        fds[i].events  = POLLPRI;
        i++;
	}
    runFlag = true;

    while(runFlag)
    {
        int ret = poll(fds, i,-1);
        if( ret == -1 )
        {
            printf("[GpioApi]:run: poll failed ! error message:%s\n", strerror(errno));
            break;
        }
        for(int j=0; j<i; j++)
        {
            if( fds[j].revents & POLLPRI)
            {
                int gpio_fd = fds[j].fd;
                ret = lseek(gpio_fd,0,SEEK_SET);
                if( ret == -1 )
                {
                    printf("[GpioApi]:run: lseek failed ! error message:%s\n", strerror(errno));
                    break;
                }
                char buf[2];
                int ret = read(gpio_fd,buf,sizeof(buf));
                if( ret == -1 )
                {
                    printf("[GpioApi]:run: read failed ! error message:%s\n", strerror(errno));
                    break;
                }
                IO_NUM ioNum = mapListenIo.value(gpio_fd);
                emit ioChanged(ioNum, buf[0]-48);
//                qDebug()<<ret<<ioNum<<"ioChanged"<<buf[0];
            }
        }
    }
}
//void GpioApi::run()
//{
//    fd_set fds;
//    int maxFd = mapListenIo.keys().last()+1;
//    runFlag = true;

//    while(runFlag)
//    {
//        FD_ZERO(&fds);
//        foreach (int fd, mapListenIo.keys())
//        {
//            FD_SET(fd, &fds);
//        }
//        int rc = select(maxFd, &fds, NULL, NULL, NULL);
//        if (rc < 0)
//        {
//            //select函数出错，跳出循环
//            printf("[GpioApi]:run: select failed ! error message:%s\n", strerror(errno));
//            break;
//        }

//        foreach (int fd, mapListenIo.keys())
//        {
//            FD_ISSET(fd, &fds);
//            char buf[2];
//            IO_NUM ioNum = mapListenIo.value(fd);
//            int ret = read(fd, buf, sizeof(buf));
//            qDebug()<<rc<<ret<<ioNum<<"ioChanged"<<buf[0];
//        }
//        usleep(300000);
//    }
//}
