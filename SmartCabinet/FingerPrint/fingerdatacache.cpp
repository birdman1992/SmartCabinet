#include "fingerdatacache.h"

FingerDataCache::FingerDataCache(QMap<int, QByteArray> dataCache):
    cache(dataCache)
{
    it = cache.begin();
    readFinish = false;
}

QByteArray FingerDataCache::current()
{
    return it.value();
}

QByteArray FingerDataCache::next()
{
    QByteArray ret;

    if(it == cache.end())
        return QByteArray();
    else
    {
        ret = it.value();
        it++;
    }

    return ret;
}

int FingerDataCache::curFingerID()
{
    return it.key();
}

//void FingerDataCache::seek(int pos)
//{
//    if((pos < 0) || (pos > cache.count() - 1))
//        return;

//    _index = pos;
//}
