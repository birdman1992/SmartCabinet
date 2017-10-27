#include "qhid.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/hiddev.h>
#include <QDebug>
extern "C"
{
    #include "hid.h"
}



QHid::QHid(QObject *parent) : QThread(parent)
{

}

void QHid::run()
{
    struct hiddev_event ev[64];
    unsigned int i,j=0;
    int rd = 0;
    char id[100] = {0};
    setbuf(stdout,NULL);

    while (1)
    {
        rd = read(fd, ev, sizeof(ev));
        if (rd < (int) sizeof(ev[0]))
        {
            if (rd < 0)
                perror("\nevtest: error reading");
            return;
        }

        for (i = 0; i < rd / sizeof(ev[0]); i++)
        {
            if(ev[i].hid && (ev[i].hid)!=0x280000)
            {
                if(((ev[i].hid)>>16)<41)
                    id[j++] = tab[((ev[i].hid)>>16)];
                else
                    id[j++] = ((ev[i].hid)>>16);
            }
            else if((ev[i].hid)==0x280000)
            {
                QByteArray qba(id, strlen(id));
                printf("READ ID:%s\n",id);
                emit hidRead(qba);
                memset(id, 0, 100);
                j=0;
            }
        }
    }
}

void QHid::hidOpen(QString dev)
{
    struct hiddev_devinfo dinfo;
    QByteArray qba = dev.toLocal8Bit();
    fd = open(qba.data(), O_RDWR);
    if (fd == -1)
    {
        fprintf(stderr, "open %s failure\n", qba.data());
        return;
    }
    printf("%s info\n", qba.data());

    if (ioctl(fd, HIDIOCGVERSION, &version) < 0)
        perror("HIDIOCGVERSION");
    else
    {
        printf("HIDIOCGVERSION: %d.%d\n", (version >> 16) & 0xFFFF,version & 0xFFFF);
        if (version != HID_VERSION)
            printf("WARNING: version does not match compile-time version\n");
    }

    if (ioctl(fd, HIDIOCGDEVINFO, &dinfo) < 0)
        perror("HIDIOCGDEVINFO");
    else
    {
        printf("HIDIOCGDEVINFO: bustype=%d busnum=%d devnum=%d ifnum=%d\n"
                "\tvendor=0x%04hx product=0x%04hx version=0x%04hx\n"
                "\tnum_applications=%d\n", dinfo.bustype, dinfo.busnum,
                dinfo.devnum, dinfo.ifnum, dinfo.vendor, dinfo.product,
                dinfo.version, dinfo.num_applications);
    }

    if (ioctl(fd, HIDIOCGNAME(99), name) < 0)
        perror("HIDIOCGNAME");
    else
    {
        name[99] = 0;
        printf("HIDIOCGNAME: %s\n", name);
    }

    show_all_report(fd);
    this->start();
}
