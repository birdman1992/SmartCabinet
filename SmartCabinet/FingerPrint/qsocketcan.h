#ifndef QSOCKETCAN_H
#define QSOCKETCAN_H
#include <QThread>
#include <QByteArray>
#include <QList>
#include "cmdpack.h"

class QSocketCan : public QThread
{
    Q_OBJECT
public:
    QSocketCan(QObject* parent=NULL);
    void sendData(quint32 canId, QByteArray canData);

signals:
    void canData(int id, QByteArray data);
    void moduleActive(int id);

private:
    int s;//fd
    QList<ResponsePack*> list_cache;
    void initCacheList();
    void run();

};

#endif // QSOCKETCAN_H
