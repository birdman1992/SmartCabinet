#include "userprintmanager.h"

UserPrintManager* UserPrintManager::m = new UserPrintManager;

UserPrintManager *UserPrintManager::manager()
{
    return m;
}

UserPrintManager::UserPrintManager()
{

}
