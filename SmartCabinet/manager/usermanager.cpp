#include "usermanager.h"
#include "defines.h"

//static UserManager* manager;
UserManager* UserManager::m = new UserManager;

UserManager *UserManager::manager()
{
    return m;
}

UserManager::UserManager(QObject *parent) : QObject(parent)
{

}
