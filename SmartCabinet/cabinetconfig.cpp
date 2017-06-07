#include "cabinetconfig.h"
#include <QVariant>
#include <QDebug>
#include <QTextCodec>
#include <qobject.h>
#include "defines.h"

CabinetConfig::CabinetConfig()
{
    state = STATE_NO;
    //    cabId.clear();
    cabinetId.clear();
    list_user.clear();
    list_cabinet.clear();
    //    qDebug()<<list_cabinet.count();
    if(!QDir("/home/config").exists())
    {
        QDir dir;
        dir.mkdir("/home/config");
    }

    readUserConfig();
    readCabinetConfig();
}

CabinetConfig::~CabinetConfig()
{
    qDeleteAll(list_user.begin(), list_user.end());
    list_user.clear();
}

void CabinetConfig::setCabinetId(QString id)
{
    QSettings settings(CONF_CABINET,QSettings::IniFormat);
    settings.setValue("CabinetId",QVariant(id));
    settings.sync();
    cabinetId = id;
    //    qDebug()<<"[setCabinetId]"<<cabId<<&cabId<<&id;
}

QString CabinetConfig::getCabinetId()
{
    qDebug()<<"[getCabinetId]"<<cabinetId<<&cabinetId;
    return cabinetId;
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
        if(list_user.at(i)->cardId == userId)
            return i;
    }
    return -1;
}

void CabinetConfig::readUserConfig()
{
    if(!QFile(CONF_USER).exists())
    {
        //        firstUse = true;
        return;
    }
    QSettings settings(CONF_USER,QSettings::IniFormat);
    int i = 0;
    firstUse = false;

    userNum = settings.value("userNum").toInt();//读取用户数量

    for(i=0; i<userNum; i++)
    {
        UserInfo* info = new UserInfo;

        info->cardId = settings.value(QString("user%1/cardId").arg(i)).toString();

        list_user<<info;
    }
}

//读取柜子配置
void CabinetConfig::readCabinetConfig()
{
    if(!QFile(CONF_CABINET).exists())
    {
        firstUse = true;
        return;
    }

    QSettings settings(CONF_CABINET,QSettings::IniFormat);
    int cabNum = settings.value("CabNum").toInt();
    int i = 0;
    int j = 0;

    cabinetId = settings.value("CabinetId").toString();

    for(i=0; i<cabNum; i++)
    {
        Cabinet* cab = new Cabinet();
        int pos = settings.value(QString("Cab%1PosNum").arg(i)).toInt();
        cab->CabinetInit(i, pos, VICE_CAB_CASE_NUM,(i==0));
        list_cabinet<<cab;
    }

    settings.beginReadArray("Cabinet0");
    for(j=0; j<CAB_CASE_1_NUM-1; j++)
    {
        settings.setArrayIndex(j);
        CabinetInfo* info = new CabinetInfo;
        info->name = settings.value("name").toString();
//        qDebug()<<"[PY]"<<getChineseSpell(info->name)<<info->name;
        info->num = settings.value("num").toInt();
        info->id = settings.value("id").toString();
        info->unit = settings.value("unit").toString();
        info->packageId = settings.value("packageId").toString();
        //        qDebug()<<"[addCase]"<<0<<info->name<<info->num;
        list_cabinet[0]->addCase(info);
    }
    settings.endArray();
    for(i=1; i<cabNum; i++)
    {
        settings.beginReadArray(QString("Cabinet%1").arg(i));
        for(j=0; j<CAB_CASE_0_NUM; j++)
        {
            settings.setArrayIndex(j);
            CabinetInfo* info = new CabinetInfo;
            info->name = settings.value("name").toString();
            info->num = settings.value("num").toInt();
            info->id = settings.value("id").toString();
            info->unit = settings.value("unit").toString();
            info->packageId = settings.value("packageId").toString();
//            qDebug()<<"[PY]"<<getChineseSpell(info->name)<<info->name;
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
    settings.setValue("CabinetId",cabinetId);
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
        settings.setValue("unit",QVariant(QString()));
        settings.setValue("id",QVariant(QString()));
        settings.setValue("packageId",QVariant(QString()));
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
            settings.setValue("unit",QVariant(QString()));
            settings.setValue("id",QVariant(QString()));
            settings.setValue("packageId",QVariant(QString()));
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

int CabinetConfig::getLockId(int seq, int index)
{
    return (seq <= 0)?index:(6+(seq-1)*8+index);
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
    settings.setValue("cardId",QVariant(info->cardId));
    settings.endGroup();
    list_user<<info;
    settings.setValue("userNum", QVariant(list_user.count()));
    firstUse = false;
}

QChar CabinetConfig::getPyCh(QString str)
{
    QChar ret;
    QTextCodec* pCodec = QTextCodec::codecForName("gb2312");
    if(!pCodec) return QChar(' ');
    QByteArray qba = pCodec->fromUnicode(str);

    int tmp = ((qba[0]&0xff)<<8)|(qba[1]&0xff);

    if(tmp >= 45217 && tmp <= 45252) ret = 'A';
    else if(tmp >= 45253 && tmp <= 45760) ret = 'B';
    else if(tmp >= 45761 && tmp <= 46317) ret = 'C';
    else if(tmp >= 46318 && tmp <= 46825) ret = 'D';
    else if(tmp >= 46826 && tmp <= 47009) ret = 'E';
    else if(tmp >= 47010 && tmp <= 47296) ret = 'F';
    else if(tmp >= 47297 && tmp <= 47613) ret = 'G';
    else if(tmp >= 47614 && tmp <= 48118) ret = 'H';
    else if(tmp >= 48119 && tmp <= 49061) ret = 'J';
    else if(tmp >= 49062 && tmp <= 49323) ret = 'K';
    else if(tmp >= 49324 && tmp <= 49895) ret = 'L';
    else if(tmp >= 49896 && tmp <= 50370) ret = 'M';
    else if(tmp >= 50371 && tmp <= 50613) ret = 'N';
    else if(tmp >= 50614 && tmp <= 50621) ret = 'O';
    else if(tmp >= 50622 && tmp <= 50905) ret = 'P';
    else if(tmp >= 50906 && tmp <= 51386) ret = 'Q';
    else if(tmp >= 51387 && tmp <= 51445) ret = 'R';
    else if(tmp >= 51446 && tmp <= 52217) ret = 'S';
    else if(tmp >= 52218 && tmp <= 52697) ret = 'T';
    else if(tmp >= 52698 && tmp <= 52979) ret = 'W';
    else if(tmp >= 52980 && tmp <= 53640) ret = 'X';
    else if(tmp >= 53689 && tmp <= 54480) ret = 'Y';
    else if(tmp >= 54481 && tmp <= 55289) ret = 'Z';
    else ret = str.at(0);

    return ret;
}
