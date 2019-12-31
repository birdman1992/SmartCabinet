#ifndef CHECKMANAGER_H
#define CHECKMANAGER_H
#include <qglobal.h>
#include <qbytearray.h>

class CheckManager
{
public:
    CheckManager();
    static quint16 CRC16(QByteArray data);
};

#endif // CHECKMANAGER_H
