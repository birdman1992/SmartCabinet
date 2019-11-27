#ifndef QSOCKETCAN_H
#define QSOCKETCAN_H
#include <QThread>
#include <QByteArray>
#include <QList>
#include <QMutex>
#include "cmdpack.h"

class QSocketCan : public QThread
{
    Q_OBJECT
public:
    QSocketCan(QObject* parent=NULL);
    void sendData(int canId, QByteArray canData);
    void lockClear();

signals:
    void canData(int id, QByteArray data);
    void moduleActive(int id);
    void doorState(int id, bool isOpen);
    void canDevOK();

private:
    int s;//fd
    quint32 canState;
    QList<ResponsePack*> list_cache;
    QList< QList<QByteArray> > list_data;
    void sendCanData(int canId, QByteArray canData);
    void sendNextData(int canId);
    void canLock(int canId);
    void canUnlock(int canId);
    bool canIsLock(int canId);
    void initCacheList();
    void run();

};

#endif // QSOCKETCAN_H
