#ifndef LOCKMANAGER_H
#define LOCKMANAGER_H
#include "defines.h"
#include <QByteArray>

class LockManager
{
public:
    static LockManager* manager();
    void setLockCtrl(int cabSeq, int cabIndex, int ctrlSeq, int ctrlIndex);
    QByteArray getLockCtrlSeq(int cabSeq);
    QByteArray getLockCtrlIndex(int cabSeq);

private:
    LockManager();
    static LockManager* m;
};

#endif // LOCKMANAGER_H
