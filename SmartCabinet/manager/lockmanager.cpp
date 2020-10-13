#include "lockmanager.h"
#include <QFile>
#include <QSettings>
#include <QStringList>
#include <QDebug>

LockManager* LockManager::m = new LockManager();

LockManager *LockManager::manager()
{
    return m;
}

void LockManager::setLockCtrl(int cabSeq, int cabIndex, int ctrlSeq, int ctrlIndex)
{
    QSettings settings_new(CONF_LOCK, QSettings::IniFormat);
    QString key_seq = QString("Cabinet%1/%2").arg(cabSeq).arg("ctrlSeq");
    QString key_index = QString("Cabinet%1/%2").arg(cabSeq).arg("ctrlIndex");
    QByteArray wSeq = settings_new.value(key_seq, QByteArray::fromHex("00000000000000000000000000000000")).toByteArray();
    QByteArray wIndex = settings_new.value(key_index, QByteArray::fromHex("00000000000000000000000000000000")).toByteArray();
    ctrlSeq = (ctrlSeq>0xff)?0xff:ctrlSeq;
    ctrlIndex = (ctrlIndex>0xff)?0xff:ctrlIndex;
    wSeq[cabIndex] = ctrlSeq;
    wIndex[cabIndex] = ctrlIndex;
//    qDebug()<<"setLockCtrl"<<cabSeq<<cabIndex<<ctrlSeq<<ctrlIndex;
    settings_new.setValue(key_seq, wSeq);
    settings_new.setValue(key_index, wIndex);
    settings_new.sync();
}

QByteArray LockManager::getLockCtrlSeq(int cabSeq)
{
    QSettings settings_new(CONF_LOCK, QSettings::IniFormat);
    QString key_seq = QString("Cabinet%1/%2").arg(cabSeq).arg("ctrlSeq");
    return settings_new.value(key_seq, QByteArray::fromHex("00000000000000000000000000000000")).toByteArray();
}

QByteArray LockManager::getLockCtrlIndex(int cabSeq)
{
    QSettings settings_new(CONF_LOCK, QSettings::IniFormat);
    QString key_index = QString("Cabinet%1/%2").arg(cabSeq).arg("ctrlIndex");
    return settings_new.value(key_index, QByteArray::fromHex("00000000000000000000000000000000")).toByteArray();
}

LockManager::LockManager()
{
    qDebug()<<"[LockManager]";
    if(QFile(CONF_LOCK).exists())
        return;

    QSettings settings_old(CONF_CABINET, QSettings::IniFormat);
    QSettings settings_new(CONF_LOCK, QSettings::IniFormat);
    QStringList cabs = settings_old.childGroups();
    cabs.removeOne("Users");
//    qDebug()<<cabs;
    foreach(QString cab, cabs)
    {
        QString key_seq = QString("%1/%2").arg(cab).arg("ctrlSeq");
        QString key_index = QString("%1/%2").arg(cab).arg("ctrlIndex");
        settings_new.setValue(key_seq, settings_old.value(key_seq, QByteArray::fromHex("00000000000000000000000000000000")).toByteArray());
        settings_new.setValue(key_index, settings_old.value(key_index, QByteArray::fromHex("00000000000000000000000000000000")).toByteArray());
    }
    settings_new.sync();
}
