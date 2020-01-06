#include "signalmanager.h"

SignalManager* SignalManager::m = new SignalManager;

SignalManager *SignalManager::manager()
{
    return m;
}

SignalManager::SignalManager(QObject *parent) : QObject(parent)
{

}
