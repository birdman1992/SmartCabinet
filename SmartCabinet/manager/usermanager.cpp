#include "usermanager.h"
#include "defines.h"
#include <QString>

//static UserManager* manager;
UserManager* UserManager::m = new UserManager;

UserManager *UserManager::manager()
{
    return m;
}

NUserInfo *UserManager::getUserInfo(QString card_no)
{
    NUserInfo* info = new NUserInfo();

    QString groupName = card_no;
    info->card_no = getConfig(groupName+"/"+"card_no", QString()).toString();
    info->real_name = getConfig(groupName+"/"+"real_name", QString()).toString();
    info->role_id = getConfig(groupName+"/"+"role_id", 0).toInt();
    info->role_name = getConfig(groupName+"/"+"role_name", QString()).toString();
    return info;
}

void UserManager::setUserInfo(NUserInfo *info)
{
    if(info == NULL)
        return;

    QString groupName = info->card_no;
    setConfig(groupName+"/"+"card_no", info->card_no);
    setConfig(groupName+"/"+"real_name", info->real_name);
    setConfig(groupName+"/"+"role_id", info->role_id);
    setConfig(groupName+"/"+"role_name", info->role_name);
}

UserManager::UserManager(QObject *parent) : QObject(parent)
{
    configPath = CONF_USER_INFO;
}

/********base functions*******/
void UserManager::setConfig(QString key, QVariant value)
{
    QSettings settings(configPath, QSettings::IniFormat);
    settings.setValue(key, value);
    settings.sync();
}

QVariant UserManager::getConfig(QString key, QVariant defaultRet)
{
    QSettings settings(configPath, QSettings::IniFormat);
    return settings.value(key, defaultRet);
}

QStringList UserManager::getConfigGroups()
{
    QSettings settings(configPath, QSettings::IniFormat);
    return settings.childGroups();
}

void UserManager::removeConfig(QString path)
{
    QSettings settings(configPath, QSettings::IniFormat);
    settings.remove(path);
}
/********base functions end*******/

NUserInfo::NUserInfo()
{

}
