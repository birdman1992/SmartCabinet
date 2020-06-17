#include "rfidpackage.h"
#include <stdlib.h>
#include <QDebug>


RfidResponse::RfidResponse()
{
    packData = QByteArray();
    dataCache = QByteArray();
}

bool RfidResponse::appendData(QByteArray _data)
{
    dataCache.append(_data);

    while(((unsigned char)dataCache[0] != 0x5a) && (dataCache.size() > 0))
    {
//        qDebug()<<"[loss]:1byte";
        dataCache.remove(0,1);
    }

    if(dataCache.size() < 9)
        return false;

    char* pos = dataCache.data();
    MEM_FETCH(head, pos);
    MEM_FETCH(word, pos);
    MEM_FETCH(len, pos);
    len = ntohs(len);
    word = ntohl(word);
    mid = word & 0xff;
    if(dataCache.size() < len+9)
        return false;

    paramData = QByteArray(pos, len);
    pos += len;
    MEM_FETCH(cks, pos);
    cks = ntohs(cks);
    packData = dataCache.left(9 + len);
    dataCache.remove(0, 9 + len);
//    qDebug()<<"[loss]:"<<9+len<<"byte";

    if(CheckManager::CRC16(packData) != cks)
    {
        qDebug()<<"[RfidResponse]:package check err."<<cks<<CheckManager::CRC16(packData);
        packData.clear();
        return false;
    }
    return true;
}

RfidCmd::RfidCmd(quint8 _mid, QByteArray _data)
{
    head = 0x5a;
    mid = _mid;
    paramData = QByteArray(_data);
    word = htonl(0x00010200 | mid);
    len = htons(paramData.size());
}

QByteArray RfidCmd::packData()
{
    QByteArray ret = QByteArray(paramData.size() + 9, 0);
    char* pos = ret.data();
    MEM_ASSIGN(pos, head);
    MEM_ASSIGN(pos, word);
    MEM_ASSIGN(pos, len);
    MEM_ASSIGN_ARRAY(pos, paramData);
    cks = htons(CheckManager::CRC16(ret));
    MEM_ASSIGN(pos, cks);
    return ret;
}
