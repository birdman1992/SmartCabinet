#ifndef QHID_H
#define QHID_H

#include <QObject>
#include <QThread>
#include <QString>
#include <qglobal.h>
#include "hidapi.h"

class QHid : public QThread
{
    Q_OBJECT
public:
    explicit QHid(QObject *parent = 0);
    bool hidOpen(unsigned short vId, unsigned short pId);
    void hidClose();
    void hidReopen();
    void run();
    bool isOpen();

    quint32 deviceId();
private:
    hid_device* handle;
    quint16 v;
    quint16 p;

signals:
    void hidRead(QByteArray qba);
};

#endif // QHID_H
