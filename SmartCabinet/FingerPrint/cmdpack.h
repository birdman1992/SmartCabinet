#ifndef CMDPACK_H
#define CMDPACK_H

#include <qglobal.h>
#include <QByteArray>

class CmdPack
{
public:
    CmdPack(quint16 _cmd, QByteArray _data = QByteArray());
    CmdPack(quint8 _sid, quint8 _did, quint16 _cmd, QByteArray _data = QByteArray());

    QByteArray packData();

private:
    quint16 prefix;//包识别码
    quint8 sid;//源标识
    quint8 did;//目标标识
    quint16 cmd;//命令字
    quint16 len;//数据长度
    QByteArray data;//命令参数 16byte,大于16字节的为数据包,prefix=0xA55A,16byte则是命令包prefix=0xAA55
    quint16 cks;//校验和
};

class ResponsePack
{
public:
    ResponsePack(QByteArray _data = QByteArray());
    QByteArray appendData(QByteArray _data);
    void initData(QByteArray _data);
    quint16 getMagicCode();

    QByteArray packData;
    quint16 prefix;//包识别码
    quint8 sid;//源标识
    quint8 did;//目标标识
    quint16 rcm;//响应
    quint16 len;//数据长度
    quint16 ret;//结果码
    QByteArray data;//响应数据 14byte
    quint16 cks;//校验和
    quint16 magicCode;
    int dataPos;
};

#endif // CMDPACK_H
