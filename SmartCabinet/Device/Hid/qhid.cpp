#include "qhid.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <QDebug>
#include <QByteArray>
extern "C"
{
    #include "hidapi.h"
}

static unsigned char tab[] = {0, 0, 0, 0, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 0, 0, 0, 0, 0, 45, 61, 91, 93, 92, 0, 59, 39, 96, 44, 46, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 33, 64, 35, 36, 37, 94, 38, 42, 40, 41, 0, 0, 0, 0, 0, 95, 43, 123, 125, 124, 0, 58, 34, 126, 60, 62, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

QHid::QHid(QObject *parent) : QThread(parent)
{
    handle = NULL;
    hid_init();
}

void QHid::run()
{
    int res = 0;
    int flag =0;
    char c,g;
    unsigned char rst[100];
    unsigned char buf[10];
    memset(rst, 0, 100);
    while (1)
    {
        res = hid_read(handle, buf, sizeof(buf));
        if(res>0)
        {
//            qDebug()<<QByteArray((const char*)buf, 10).toHex();
            if(buf[2] == 0x00)
            {
                continue;
            }
            if(buf[2] == 0x28)
            {
                printf("READ:%s\n",rst);
                emit hidRead(QByteArray((char*)rst, flag));
                memset(rst, 0, 50);
                flag = 0;
                continue;
            }
            c = buf[2];
            g = (buf[0]!=0);
            rst[flag] = tab[g*128+c];
//            printf("%x %x %x %c\n", g,buf[0], buf[2], rst[flag]);
            flag++;
        }
    }

    hidClose();
}

bool QHid::hidOpen(unsigned short vId, unsigned short pId)
{
    handle = hid_open(vId, pId, NULL);
    if(!handle)
    {
        qWarning("Device:v:%d p:%d open failed.",vId, pId);
        return false;
    }
    this->start();
    return true;
}

void QHid::hidClose()
{
    if(handle != NULL)
    {
        hid_close(handle);
        hid_exit();
        handle = NULL;
    }
}
