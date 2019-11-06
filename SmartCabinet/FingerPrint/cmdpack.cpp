#include "cmdpack.h"
#include <QDebug>

CmdPack::CmdPack(quint8 _sid, quint8 _did, quint16 _cmd, quint16 _len, QByteArray _data):
    sid(_sid),
    did(_did),
    cmd(_cmd),
    len(_len)
//    data(_data)
{
    if(len<=16)
    {
        data = QByteArray(16, 0);
        data.replace(0, _data.size(), _data);
    }
}

CmdPack::CmdPack(quint16 _cmd, quint16 _len, QByteArray _data):
    cmd(_cmd),
    len(_len)
{
    sid = 0;
    did = 0;
    if(len<=16)
    {
        data = QByteArray(16, 0);
        data.replace(0, _data.size(), _data);
    }
}

/*
    quint16 prefix;//包识别码
    quint8 sid;//源标识
    quint8 did;//目标标识
    quint16 cmd;//命令字
    quint16 len;//数据长度
    QByteArray data;//命令参数 16byte,大于16字节的为数据包,prefix=0xA55A,16byte则是命令包
    quint16 cks;//校验和
*/
QByteArray CmdPack::packData()
{
    int totalSize = sizeof(prefix) + sizeof(sid) + sizeof(did) + sizeof(cmd) + sizeof(len) + sizeof(cks) + data.size();
    if(totalSize == 26)
        prefix = 0xAA55;
    else
        prefix = 0xA55A;

    QByteArray ret = QByteArray(totalSize, 0);
    char* pos = ret.data();

    *(quint16*)pos = prefix;
    pos += 2;
    *(quint8*)pos = sid;
    pos += 1;
    *(quint8*)pos = did;
    pos += 1;
    *(quint16*)pos = cmd;
    pos += 2;
    *(quint16*)pos = len;
    pos += 2;
    memcpy(pos, data.data(), data.size());
    pos += data.size();

    cks = 0;//清零，开始计算校验值
    for(int i=0; i<ret.size(); i++)
    {
        cks = (cks + ret[i]) & 0xffff;
    }
    *(quint16*)pos = cks;
    return ret;
}

ResponsePack::ResponsePack(QByteArray _data)
{
    prefix = 0;
    len = 0;
    appendData(_data);
}

QByteArray ResponsePack::appendData(QByteArray _data)
{
    magicCode = 0xffff;
    packData.append(_data);

    if(prefix == 0)//等待包头
    {
        if(packData.size() < 8)
            return QByteArray();

        char* pos = packData.data();
        prefix = *(quint16*)pos;
        if((prefix != 0x55aa) && (prefix != 0x5aa5))//包头不对
        {
            if(_data.right(6).toHex().toUpper() == "000000000000" )
            {
                magicCode = prefix;
            }
            prefix = 0;
            len = 0;
            packData.clear();
            return QByteArray();
        }
        pos += 2;
        sid = *(quint8*)pos;
        pos += 1;
        did = *(quint8*)pos;
        pos += 1;
        rcm = *(quint16*)pos;
        pos += 2;
        len = *(quint16*)pos;
    }

    int dataLen = len<16?16:len;//data的长度
    if(packData.size() < (dataLen + 10))//需要等待更多数据
        return QByteArray();

    //开始完整包校验
    cks = 0;//清零，开始计算校验值
    for(int i=0; i<(dataLen + 8); i++)
    {
        cks = (cks + packData[i]) & 0xffff;
    }
    quint16 packCks = *(quint16*)(packData.data() + dataLen + 8);
    if(cks != packCks)
    {
        qDebug()<<"[ResponsePack] pack check error:"<<cks<<packCks;
        prefix = 0;
        len = 0;
        packData.remove(0, len+10);
        return QByteArray();
    }
    data = packData.mid(8, len);
//    qDebug()<<"packdata:"<<packData.mid(0, dataLen+10).toHex();
    QByteArray ret = packData.left(dataLen+10);
    packData.remove(0, dataLen+10);
    return ret;
}

quint16 ResponsePack::getMagicCode()
{
    return magicCode;
}
