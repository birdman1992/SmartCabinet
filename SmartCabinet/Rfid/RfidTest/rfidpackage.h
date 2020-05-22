#ifndef RFIDPACKAGE_H
#define RFIDPACKAGE_H

#include <QObject>
#include <QByteArray>
#include <arpa/inet.h>
#include "checkmanager.h"

#define typeof(x) __typeof__(x)
//把数字变量写入缓冲区
#define MEM_ASSIGN(a,b) ({*(typeof(b)*)(a)=(b);(a)+=sizeof(b);})
#define MEM_ASSIGN_ARRAY(a,b) ({memcpy((a), (b).data(), (b).size());(a)+=(b).size();})
//从缓冲区读取数字变量值
#define MEM_FETCH(a,b) ({(a)=*(typeof(a)*)(b);(b)+=sizeof(a);})
//#define MEM_FETCH_ARRAY(a,b) (a=QByteArray())

class RfidCmd
{
public:
    RfidCmd(quint8 _mid, QByteArray _data = QByteArray());
    QByteArray packData();
private:
    quint8 head;
    quint32 word;
//    quint8 addr;
    quint16 len;
    QByteArray paramData;
    quint16 cks;
    quint8 mid;
};

class RfidResponse
{
public:
    RfidResponse();
    bool appendData(QByteArray _data=QByteArray());

    QByteArray dataCache;
    QByteArray packData;

    quint8 head;
    quint32 word;
    quint16 len;
    QByteArray paramData;
    quint16 cks;
    quint8 mid;
};

#endif // RFIDPACKAGE_H
