#ifndef QHID_H
#define QHID_H

#include <QObject>
#include <QThread>
#include <QString>
#include "hidapi.h"

class QHid : public QThread
{
    Q_OBJECT
public:
    explicit QHid(QObject *parent = 0);
    bool hidOpen(unsigned short vId, unsigned short pId);
    void run();

private:
    hid_device* handle;

signals:
    void hidRead(QByteArray qba);
};

#endif // QHID_H
