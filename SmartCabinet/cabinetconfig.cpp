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

//读取柜子配置
void CabinetConfig::readCabinetConfig()
{
    if(!QFile(CONF_CABINET).exists())
    {
        return;
    }

    QSettings settings(CONF_USER,QSettings::IniFormat);
//    int size = settings.beginReadArray("cabinets");
}

//创建柜子配置文件  qba:柜子位置信息
void CabinetConfig::creatCabinetConfig(QByteArray qba)
{
    int i = 0;
    int j = 0;
    qDebug()<<"creatCabinetConfig1";
    QSettings settings(CONF_CABINET, QSettings::IniFormat);

    settings.setValue("CabNum",qba.size());
    for(i=0; i<qba.size(); i++)//保存柜子位置编号
    {
        settings.setValue(QString("Cab%1PosNum").arg(i),QVariant(qba[i]));
    }

    settings.beginWriteArray(QString("Cabinet0"));
    for(j=0; j<Main_CAB_CASE_NUM; j++)
    {
        settings.setArrayIndex(j);
        settings.setValue("name",QVariant(QString()));
        settings.setValue("num", QVariant(0));
    }
    settings.endArray();

    for(i=1; i<qba.size(); i++)
    {
        settings.beginWriteArray(QString("Cabinet%1").arg(i));
        for(j=0; j<VICE_CAB_CASE_NUM; j++)
        {
            settings.setArrayIndex(j);
            settings.setValue("name",QVariant(QString()));
            settings.setValue("num", QVariant(0));
        }
        settings.endArray();
    }
    settings.sync();
    qDebug()<<"creatCabinetConfig2";
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
