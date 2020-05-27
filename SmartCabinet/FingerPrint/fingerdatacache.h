#ifndef FINGERDATACACHE_H
#define FINGERDATACACHE_H
#include <QMap>
#include <QByteArray>

class FingerDataCache
{
public:
    FingerDataCache(QMap<int, QByteArray> dataCache);
    QByteArray current();
    QByteArray next();
    int curFingerID();
//    void seek(int pos);

private:
    QMap<int, QByteArray> cache;
//    int _index;
    bool readFinish;
    QMap<int, QByteArray>::iterator it;
};

#endif // FINGERDATACACHE_H
