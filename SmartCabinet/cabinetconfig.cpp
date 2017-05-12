#include "cabinetconfig.h"
#include <QVariant>
#include <QDebug>
#include "defines.h"

CabinetConfig::CabinetConfig()
{
    state = STATE_NO;//qDebug("a");
    list_user.clear();
    list_cabinet.clear();//qDebug("b");
//    qDebug()<<list_cabinet.count();
    if(!QDir("config").exists())
    {
        QDir dir;
        dir.mkdir("config");
    }

    readUserConfig();
    readCabinetConfig();
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

int CabinetConfig::checkUser(QString userId)
{
    int i = 0;

    for(i=0; i<list_user.count(); i++)
    {
        if(list_user.at(i)->userId == userId)
            return i;
    }
    return -1;
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

    QSettings settings(CONF_CABINET,QSettings::IniFormat);
    int cabNum = settings.value("CabNum").toInt();
    int i = 0;
    int j = 0;

    for(i=0; i<cabNum; i++)
    {
        Cabinet* cab = new Cabinet();
        int pos = settings.value(QString("Cab%1PosNum").arg(i)).toInt();
        cab->CabinetInit(i, pos, VICE_CAB_CASE_NUM,(i==0));qDebug()<<cab;
        list_cabinet<<cab;
    }

    settings.beginReadArray("Cabinet0");
    for(j=0; j<Main_CAB_CASE_NUM; j++)
    {
        settings.setArrayIndex(j);
        CabinetInfo* info = new CabinetInfo;
        info->name = settings.value("name").toString();
        info->num = settings.value("num").toInt();
//        qDebug()<<"[addCase]"<<0<<info->name<<info->num;
        list_cabinet[0]->addCase(info);
    }
    settings.endArray();
    for(i=1; i<cabNum; i++)
    {
        settings.beginReadArray(QString("Cabinet%1").arg(i));
        for(j=0; j<VICE_CAB_CASE_NUM; j++)
        {
            settings.setArrayIndex(j);
            CabinetInfo* info = new CabinetInfo;
            info->name = settings.value("name").toString();
            info->num = settings.value("num").toInt();
//            qDebug()<<"[addCase]"<<i<<info->name<<info->num;
            list_cabinet[i]->addCase(info);
        }
        settings.endArray();
    }
}

//创建柜子配置文件  qba:柜子位置信息
void CabinetConfig::creatCabinetConfig(QByteArray qba)
{
    int i = 0;
    int j = 0;
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

    readCabinetConfig();
}

//void CabinetConfig::writeCabinetConfig(int cabSeq, int caseIndex, CabinetInfo *info)
//{

//}

CaseAddress CabinetConfig::checkCabinetByName(QString name)
{
    int i = 0;
    int j = 0;
    CaseAddress ret;

    for(i=0; i<list_cabinet.count(); i++)
    {
        for(j=0; j<list_cabinet.at(i)->list_case.count(); j++)
        {
            if(list_cabinet.at(i)->list_case.at(j)->name == name)
            {
                ret.cabinetSeqNUM = i;
                ret.caseIndex = j;
                return ret;
            }
        }
    }

    return ret;
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
    settings.setValue("userNum", QVariant(list_user.count()));
    firstUse = false;
}
