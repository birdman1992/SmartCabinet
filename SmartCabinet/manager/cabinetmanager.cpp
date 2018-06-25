#include "cabinetmanager.h"

CabinetManager* CabinetManager::m = new CabinetManager;

CabinetManager *CabinetManager::manager()
{
    return m;
}

CabinetManager::CabinetManager(QObject *parent) : QObject(parent)
{

}
