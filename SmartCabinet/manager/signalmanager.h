#ifndef SIGNALMANAGER_H
#define SIGNALMANAGER_H

#include <QObject>
#include "defines.h"
#include "Rfid/RfidTest/rfidmanager.h"

class SignalManager : public QObject
{
    Q_OBJECT
public:
    static SignalManager* manager();

signals:
    void epcAccess(QStringList epcs, UserOpt optType);
    void epcAccess(QStringList fetchEpcs, QStringList backEpcs);
    void epcInfoUpdate();
    void epcConsumed(QStringList epcs);
    void epcStore(QMap<QString ,QVariantMap>);
    void doorState(int id, bool isOpen);
    void accessSuccess(QString msg);
    void accessFailed(QString msg);




private:
    explicit SignalManager(QObject *parent = NULL);
    static SignalManager* m;

};

#endif // SIGNALMANAGER_H
