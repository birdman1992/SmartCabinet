#include "cabinetconfig.h"
#include <QVariant>
#include <QDebug>
#include "defines.h"

CabinetConfig::CabinetConfig()
{
    if(!QDir("config").exists())
    {
        QDir dir;
        dir.mkdir("config");
    }
    readUserConfig();
}

CabinetConfig::~CabinetConfig()
{
    qDeleteAll(list_user.begin(), list_user.end());
    list_user.clear();
}

bool CabinetConfig::isFirstUse()
{
    return firstUse;
}

void CabinetConfig::addUser(UserInfo *info)
{
    addNewUser(info);
}

void CabinetConfig::readUserConfig()
{
    if(!QFile(CONF_USER).exists())
    {
        firstUse = true;
        return;
    }
    QSettings settings(CONF_USER,QSettings::IniFormat);
    int i = 0;
    firstUse = false;

    userNum = settings.value("userNum").toInt();//读取用户数量

    for(i=0; i<userNum; i++)
    {
        UserInfo* info = new UserInfo;

        info->userId = settings.value(QString("user%1/id").arg(i)).toString();

        list_user<<info;
    }
}

void CabinetConfig::readCabinetConfig()
{
    if(!QFile(CONF_CABINET).exists())
    {
        return;
    }

    QSettings settings(CONF_USER,QSettings::IniFormat);
//    int size = settings.beginReadArray("cabinets");
}

//添加新用户
void CabinetConfig::addNewUser(UserInfo *info)
{
    if(info == NULL)
    {
        qDebug()<<"[addNewUser]"<<"user info is null";
    }

    QSettings settings(CONF_USER,QSettings::IniFormat);
    settings.beginGroup(QString("user%1").arg(list_user.count()));
    settings.setValue("id",QVariant(info->userId));
    settings.endGroup();
    list_user<<info;
    firstUse = false;
}
