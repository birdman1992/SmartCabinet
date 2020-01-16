#ifndef QDEVICEWATCHER_H
#define QDEVICEWATCHER_H

#include <QObject>
#include <QThread>
#include <QStringList>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <asm/types.h>

#include <sys/socket.h>
#include <linux/netlink.h>

class QDeviceWatcher : public QThread
{
    Q_OBJECT
public:
    explicit QDeviceWatcher(QObject *parent = 0);
    ~QDeviceWatcher();
    int addDevice(QString deviceName);
    int removeDevice(QString deviceName);
    QStringList getDevices();

private:
    QStringList watchDeviceList;//监视设备表
    volatile bool loopFlag;
    char buf[4096];
    int sockfd;
    struct iovec iov;
    struct msghdr msg;

    void run();
    void netLinkInit();
    void msgFilter(QString msg);

signals:
    void deviceAdded(QString deviceName);
    void deviceRemoved(QString deviceName);
    void deviceStateChanged(quint16 pId, quint16 vId, bool action);

public slots:
};

#endif // QDEVICEWATCHER_H
