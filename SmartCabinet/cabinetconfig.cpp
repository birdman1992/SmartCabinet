#include "cabinetconfig.h"
#include <QVariant>
#include <QDebug>
#include <qstring.h>
#include <QTextCodec>
#include <qobject.h>
#include <QApplication>
#include <QtCore/qmath.h>
#include "Json/cJSON.h"
#include "defines.h"
#include "funcs/chineseletterhelper.h"
#include "Device/controldevice.h"

CabinetConfig::CabinetConfig()
{
    state = STATE_NO;
    sleepFlag = 0;
    cardReaderIsOk = false;
    codeScanIsOk = false;
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

    readCabinetConfig();
    readUserConfig();
}

CabinetConfig::~CabinetConfig()
{
    qDeleteAll(list_user.begin(), list_user.end());
    list_user.clear();
}

void CabinetConfig::saveFetchList(QByteArray _data)
{
    QSettings settings(CAB_CACHE, QSettings::IniFormat);
    settings.beginGroup("Fetch");
    int index = settings.beginReadArray("fetch");
    settings.endArray();
    settings.beginWriteArray("fetch");
    settings.setArrayIndex(index);
    settings.setValue("list", _data);
    settings.endArray();
    settings.endGroup();
    settings.sync();
}

QList<QByteArray> CabinetConfig::getFetchList()
{
    int i = 0;
    QList<QByteArray> ret;
    ret.clear();

    QSettings settings(CAB_CACHE, QSettings::IniFormat);
    settings.beginGroup("Fetch");
    int length = settings.beginReadArray("fetch");
    if(length == 0)
    {
        settings.endArray();
        settings.endGroup();
        return ret;
    }

    for(i=0; i<length; i++)
    {
        settings.setArrayIndex(i);
        QByteArray qba = settings.value("list").toByteArray();
        ret<<qba;
    }
    settings.endArray();
    settings.remove("");
    settings.endGroup();
    return ret;
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

QString CabinetConfig::getServerAddress()
{
    return serverAddr;
}

void CabinetConfig::setServerAddress(QString addr)
{
    serverAddr = addr;
    QSettings settings(CONF_CABINET,QSettings::IniFormat);
    settings.setValue("SERVER", addr);
    settings.sync();
    //    restart();
}

int CabinetConfig::getSysVolem()
{
    QSettings settings(CONF_CABINET,QSettings::IniFormat);
    return settings.value("vol", QVariant(90)).toInt();
}

void CabinetConfig::setSysVolem(int vol)
{
    QSettings settings(CONF_CABINET,QSettings::IniFormat);
    settings.setValue("vol", vol);
    settings.sync();
    QString cmd;
    QStringList params;
    vol++;
#ifdef SIMULATE_ON
    cmd = "amixer";
    params<<"cset"<<"name=\'Master Playback Volume\'"<<QString::number(volTodB(vol));
#else
    cmd = "amixer";
    params<<"cset"<<"name=\'Headphone Volume\'"<<QString::number(volTodB(vol));
#endif

    QProcess pro;
    qDebug()<<cmd<<params;
    pro.start(cmd, params);
    pro.waitForFinished(100);
}

int CabinetConfig::volTodB(int vol)
{
#ifdef SIMULATE_ON
    return qLn((double)vol/100)/qLn(10)*10+63;
#else
    return qLn((double)vol/100)/qLn(10)*10+127;
#endif
}

void CabinetConfig::clearConfig()
{
    QSettings settings(CONF_CABINET,QSettings::IniFormat);
    QString ID = settings.value("CabinetId").toString();
    settings.clear();
    if(!ID.isEmpty())
        settings.setValue("CabinetId", ID);
    restart();
}

void CabinetConfig::clearCabinet()
{
    QSettings settings(CONF_CABINET,QSettings::IniFormat);
    QString ID = settings.value("CabinetId").toString();
    settings.clear();
    if(!ID.isEmpty())
        settings.setValue("CabinetId", ID);
}

void CabinetConfig::wakeUp(int minutes)
{
    sleepFlag = minutes;
    timeoutFlag = 0;
}

void CabinetConfig::clearTimeoutFlag()
{
    timeoutFlag = 0;
}

int CabinetConfig::getSleepFlag()
{
    return sleepFlag;
}

void CabinetConfig::setCardReaderState(bool ok)
{
    cardReaderIsOk = ok;
}

bool CabinetConfig::getCardReaderState()
{
    return cardReaderIsOk;
}

void CabinetConfig::setCodeScanState(bool ok)
{
    codeScanIsOk = ok;
}

bool CabinetConfig::getCodeScanState()
{
    return codeScanIsOk;
}

bool CabinetConfig::sleepFlagTimeout()
{
    if(sleepFlag == 0)
        return false;

    timeoutFlag++;
    qDebug()<<"[sleepFlagTimeout]"<<timeoutFlag<<sleepFlag;

    return (timeoutFlag >= sleepFlag);//超时
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
    qDebug()<<"[readUserConfig]";
    QSettings settings(CONF_USER,QSettings::IniFormat);
    int i = 0;
    settings.beginGroup(QString("Users"));
    int index = settings.beginReadArray("user");

    for(i=0; i<index; i++)
    {
        settings.setArrayIndex(i);
        UserInfo* info = new UserInfo;
        info->name = settings.value("name", QString()).toString();
        info->power = settings.value("power", -1).toInt();
        info->cardId = settings.value("cardId", QString()).toString();
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
    firstUse = false;

    QSettings settings(CONF_CABINET,QSettings::IniFormat);

    cabinetId = settings.value("CabinetId", QString()).toString();

    qDebug()<<"readconfig size"<<settings.value("Cabinet0/case0/size", 0).toInt();

    if(cabinetId.isEmpty())
    {
        firstUse = true;
//        return;
    }

    int cabNum = settings.value("CabNum",0).toInt();
    if(cabNum == 0)
    {
        firstUse = true;
        return;
    }

    caseWidth = 1080/cabNum;

    serverAddr = settings.value("SERVER", QString()).toString();
    if(serverAddr.isEmpty())
    {
        firstUse = true;
//        return;
    }

    int i = 0;
    int j = 0;
    int k = 0;

    if(!list_cabinet.isEmpty())
    {
        qDeleteAll(list_cabinet.begin(),list_cabinet.end());
        list_cabinet.clear();
    }

    for(i=0; i<cabNum; i++)
    {
        Cabinet* cab = new Cabinet();
        int pos = settings.value(QString("Cab%1PosNum").arg(i)).toInt();
        cab->CabinetInit(caseWidth, i, pos, CAB_CASE_0_NUM,(i==0));
        list_cabinet<<cab;
    }

    settings.beginGroup("Cabinet0");
    QByteArray ctrlSeq = settings.value("ctrlSeq", QByteArray()).toByteArray();
    QByteArray ctrlIndex = settings.value("ctrlIndex", QByteArray()).toByteArray();

    for(j=0; j<CAB_CASE_1_NUM; j++)
    {
//        if(j == 1)
//            continue;
        int arr_size = settings.beginReadArray(QString("case%1").arg(j));

        for(k=0; k<arr_size; k++)
        {
            settings.setArrayIndex(k);
            GoodsInfo* info = new GoodsInfo;
            info->abbName = settings.value("abbName", QString()).toString();
            info->name = settings.value("name").toString();
            info->num = settings.value("num").toInt();
            info->outNum = 0;
            info->id = settings.value("id").toString();
            info->unit = settings.value("unit").toString();
            info->packageId = settings.value("packageId").toString();
            info->Py = getPyCh(info->name);//qDebug()<<"[PY]"<<info->Py;
            info->goodsType = getGoodsType(info->packageId);
//            qDebug()<<"[getGoodsType]"<<info->packageId<<info->goodsType;
            list_cabinet[0]->addCase(info,j,(cabNum == 3));//qDebug()<<"[read conf]"<<j;
            list_cabinet[0]->setCtrlWord(j, ctrlSeq, ctrlIndex);
        }
        settings.endArray();
    }
    settings.endGroup();
//    settings.endArray();
    for(i=1; i<cabNum; i++)
    {
        settings.beginGroup(QString("Cabinet%1").arg(i));
        QByteArray ctrlSeq = settings.value("ctrlSeq", QByteArray::fromHex("00000000000000000000000000000000")).toByteArray();
        QByteArray ctrlIndex = settings.value("ctrlIndex", QByteArray::fromHex("00000000000000000000000000000000")).toByteArray();

//        settings.beginReadArray(QString("Cabinet%1").arg(i));
        for(j=0; j<CAB_CASE_0_NUM; j++)
        {
            int arr_size = settings.beginReadArray(QString("case%1").arg(j));

            for(k=0; k<arr_size; k++)
            {
                settings.setArrayIndex(k);
                GoodsInfo* info = new GoodsInfo;
                info->abbName = settings.value("abbName", QString()).toString();
                info->name = settings.value("name").toString();
                info->num = settings.value("num").toInt();
                info->id = settings.value("id").toString();
                info->unit = settings.value("unit").toString();
                info->packageId = settings.value("packageId").toString();
                info->goodsType = getGoodsType(info->packageId);
                info->Py = getPyCh(info->name);//qDebug()<<"[PY]"<<info->Py;
                list_cabinet[i]->addCase(info,j,(cabNum == 3));
                list_cabinet[i]->setCtrlWord(j, ctrlSeq, ctrlIndex);
            }
            settings.endArray();
        }
        settings.endGroup();
    }
    creatCabinetJson();
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

    settings.beginGroup("Cabinet0");
    settings.setValue("cabinetSize",QVariant(CAB_CASE_1_NUM));
//    settings.beginWriteArray(QString("Cabinet0"));
    for(j=0; j<CAB_CASE_1_NUM; j++)
    {
        settings.beginWriteArray(QString("case%1").arg(j));
        settings.setArrayIndex(0);
        settings.setValue("name",QVariant(QString()));
        settings.setValue("num", QVariant(0));
        settings.setValue("unit",QVariant(QString()));
        settings.setValue("id",QVariant(QString()));
        settings.setValue("packageId",QVariant(QString()));
        settings.endArray();
    }
    settings.endGroup();
//    settings.endArray();

    for(i=1; i<qba.size(); i++)
    {
        settings.beginGroup(QString("Cabinet%1").arg(i));
        settings.setValue("cabinetSize",QVariant(CAB_CASE_0_NUM));
//        settings.beginWriteArray(QString("Cabinet%1").arg(i));
        for(j=0; j<CAB_CASE_0_NUM; j++)
        {
            settings.beginWriteArray(QString("case%1").arg(j));
            settings.setArrayIndex(0);
            settings.setValue("name",QVariant(QString()));
            settings.setValue("num", QVariant(0));
            settings.setValue("unit",QVariant(QString()));
            settings.setValue("id",QVariant(QString()));
            settings.setValue("packageId",QVariant(QString()));
            settings.endArray();
        }
        settings.endGroup();
//        settings.endArray();
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
    int goodsIndex = 0;
    CaseAddress ret;

    for(i=0; i<list_cabinet.count(); i++)
    {
        for(j=0; j<list_cabinet.at(i)->list_case.count(); j++)
        {
            goodsIndex = list_cabinet.at(i)->list_case.at(j)->caseSearch(name);
            if(goodsIndex != -1)
            {
                ret.cabinetSeqNum = i;
                ret.caseIndex = j;
                ret.goodsIndex = goodsIndex;
                qDebug()<<"[checkCabinetByName]"<<ret.cabinetSeqNum<<ret.caseIndex<<ret.goodsIndex;
                return ret;
            }
        }
    }

    return ret;
}

CaseAddress CabinetConfig::checkCabinetByBarCode(QString id)
{
    int i = 0;
    int j = 0;
    int goodsIndex = 0;
    CaseAddress ret;

    for(i=0; i<list_cabinet.count(); i++)
    {
        for(j=0; j<list_cabinet.at(i)->list_case.count(); j++)
        {//qDebug()<<i<<j;
            goodsIndex = list_cabinet.at(i)->list_case.at(j)->barcodeSearch(id);
            if(goodsIndex != -1)
            {
                ret.cabinetSeqNum = i;
                ret.caseIndex = j;
                ret.goodsIndex = goodsIndex;
                qDebug()<<ret.cabinetSeqNum<<ret.caseIndex<<ret.goodsIndex;
                return ret;
            }
        }
    }

    return ret;
}

CaseAddress CabinetConfig::checkCabinetByGoodsId(QString id)
{
    int i = 0;
    int j = 0;
    int goodsIndex = 0;
    CaseAddress ret;

    for(i=0; i<list_cabinet.count(); i++)
    {
        for(j=0; j<list_cabinet.at(i)->list_case.count(); j++)
        {//qDebug()<<i<<j;
            goodsIndex = list_cabinet.at(i)->list_case.at(j)->goodsIdSearch(id);
            if(goodsIndex != -1)
            {
                ret.cabinetSeqNum = i;
                ret.caseIndex = j;
                ret.goodsIndex = goodsIndex;
                qDebug()<<ret.cabinetSeqNum<<ret.caseIndex<<ret.goodsIndex;
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

int CabinetConfig::getCaseWidth()
{
    return caseWidth;
}

void CabinetConfig::removeConfig(CaseAddress addr)
{
    if((addr.cabinetSeqNum<0) || (addr.caseIndex<0) || (addr.goodsIndex<0))
        return;

    QSettings settings(CONF_CABINET, QSettings::IniFormat);
    settings.beginGroup(QString("Cabinet%1").arg(addr.cabinetSeqNum));
    int len = settings.beginReadArray(QString("case%1").arg(addr.caseIndex));
    QList<GoodsInfo*> l_temp;

    int i = 0;
    for(i=0; i<len; i++)
    {
        settings.setArrayIndex(i);
        GoodsInfo* info = new GoodsInfo();
        info->abbName = settings.value("abbName", QString()).toString();
        info->id = settings.value("id", QString()).toString();
        info->name = settings.value("name", QString()).toString();
        info->num = settings.value("num", 0).toInt();
        info->packageId = settings.value("packageId", QString()).toString();
        info->unit = settings.value("unit", QString()).toString();
        l_temp<<info;
    }
    settings.endArray();
    settings.endGroup();
    settings.remove(QString("Cabinet%1/case%2").arg(addr.cabinetSeqNum).arg(addr.caseIndex));

    GoodsInfo* info = l_temp.takeAt(addr.goodsIndex);
    delete(info);
    settings.beginGroup(QString("Cabinet%1").arg(addr.cabinetSeqNum));

    settings.beginWriteArray(QString("case%1").arg(addr.caseIndex));
    i=0;
    if(l_temp.isEmpty())
    {
        settings.setArrayIndex(0);
        settings.setValue("name",QVariant(QString()));
        settings.setValue("num", QVariant(0));
        settings.setValue("unit",QVariant(QString()));
        settings.setValue("id",QVariant(QString()));
        settings.setValue("packageId",QVariant(QString()));
    }
    else
    {
        for(i=0; i<l_temp.count(); i++)
        {
            settings.setArrayIndex(i);
            GoodsInfo* info = l_temp.at(i);
            settings.setValue("abbName", QVariant(info->abbName));
            settings.setValue("id", QVariant(info->id));
            settings.setValue("name", QVariant(info->name));
            settings.setValue("num", QVariant(info->num));
            settings.setValue("packageId", QVariant(info->packageId));
            settings.setValue("unit", QVariant(info->unit));
        }
    }
    settings.endArray();

    settings.endGroup();
    settings.sync();

    list_cabinet[addr.cabinetSeqNum]->updateCabinetCase(addr);
}

void CabinetConfig::setConfig(CaseAddress addr, GoodsInfo *info)
{
    Cabinet* curCab = list_cabinet[addr.cabinetSeqNum];
    CabinetInfo* curInfo = curCab->list_case[addr.caseIndex];
    if(addr.goodsIndex != curInfo->list_goods.count())
    {
        qDebug()<<"[setConfig]"<<"address is invalid.";
    }

    curInfo->list_goods<<info;

    QSettings settings(CONF_CABINET,QSettings::IniFormat);
    settings.beginGroup(QString("Cabinet%1").arg(addr.cabinetSeqNum));
    settings.beginWriteArray(QString("case%1").arg(addr.caseIndex));

    settings.setArrayIndex(addr.goodsIndex);
    settings.setValue("abbName", QVariant(info->abbName));
    settings.setValue("id", QVariant(info->id));
    settings.setValue("name", QVariant(info->name));
    settings.setValue("num", QVariant(info->num));
    settings.setValue("packageId", QVariant(info->packageId));
    settings.setValue("unit", QVariant(info->unit));
    settings.sync();
    settings.endArray();
    settings.endGroup();

    list_cabinet[addr.cabinetSeqNum]->updateCabinetCase(addr);
}

QByteArray CabinetConfig::creatCabinetJson()
{
    QByteArray chesetCode = cabinetId.toLocal8Bit();
    QByteArray cabinetPos = getCabinetPos().toHex();
    QByteArray cabinetSize = getCabinetSize().toHex();
    QByteArray ret;

    cJSON* json;
    json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "chesetCode", cJSON_CreateString(chesetCode.data()));
    cJSON_AddItemToObject(json, "cabinetPos", cJSON_CreateString(cabinetPos.data()));
    cJSON_AddItemToObject(json, "cabinetSize", cJSON_CreateString(cabinetSize.data()));

    cJSON* cabinetCtrlConfig = cJSON_CreateArray();
    QSettings settings(CONF_CABINET,QSettings::IniFormat);

    int i = 0;
    for(i=0; i<settings.value("CabNum").toInt(); i++)
    {
        settings.beginGroup(QString("Cabinet%1").arg(i));
        QByteArray ctrlSeq = settings.value("ctrlSeq", QByteArray("00000000000000000000000000000000")).toByteArray();
        QByteArray ctrlIndex = settings.value("ctrlIndex", QByteArray("00000000000000000000000000000000")).toByteArray();
        settings.endGroup();

        cJSON* obj = cJSON_CreateObject();
        cJSON_AddItemToObject(obj, "ctrlSeq", cJSON_CreateString(ctrlSeq.data()));
        cJSON_AddItemToObject(obj, "ctrlIndex", cJSON_CreateString(ctrlIndex.data()));
        cJSON_AddItemToArray(cabinetCtrlConfig, obj);
    }
    cJSON_AddItemToObject(json, "cabinetCtrlConfig", cabinetCtrlConfig);

    ret = QByteArray(cJSON_Print(json));
    cJSON_Delete(json);

//    qDebug()<<"[creatCabinetJson]"<<ret;
    return ret;
}

QByteArray CabinetConfig::getCabinetPos()
{
    QSettings settings(CONF_CABINET,QSettings::IniFormat);
    QByteArray ret;
    int i=0;
    int cabNum = settings.value("CabNum", QVariant(0)).toInt();
//    qDebug()<<"CabNum"<<cabNum;
    ret.resize(cabNum);
//    qDebug()<<ret.size();

    for(i=0; i<cabNum; i++)
    {
        ret[i] = settings.value(QString("Cab%1PosNum").arg(i),0).toInt();
//        qDebug()<<ret.toHex();
    }

    return ret;
}

QByteArray CabinetConfig::getCabinetSize()
{
    QSettings settings(CONF_CABINET,QSettings::IniFormat);
    QByteArray ret;
    int i=0;
    int cabNum = settings.value("CabNum", QVariant(0)).toInt();

    ret.resize(cabNum);

    for(i=0; i<cabNum; i++)
    {
        ret[i] = settings.value(QString("Cabinet%1/cabinetSize").arg(i),0).toInt();
    }

    return ret;
}

void CabinetConfig::searchByPinyin(QString ch)
{
    int i;

    for(i=0; i<list_cabinet.count(); i++)
    {
        list_cabinet[i]->searchByPinyin(ch);
    }
}

void CabinetConfig::clearSearch()
{
    int i;

    for(i=0; i<list_cabinet.count(); i++)
    {
        list_cabinet[i]->clearSearch();
    }
}

//添加新用户
void CabinetConfig::addNewUser(UserInfo *info)
{
    if(info == NULL)
    {
        qDebug()<<"[addNewUser]"<<"user info is null";
    }

    if(checkUserLocal(info->cardId) != NULL)
        return;
    qDebug()<<"[addNewUser]"<<info->cardId;

    QSettings settings(CONF_USER,QSettings::IniFormat);
    settings.beginGroup(QString("Users"));
    int index = settings.beginReadArray("user");
    settings.endArray();
    settings.beginWriteArray("user");
    settings.setArrayIndex(index);
    settings.setValue("name",QVariant(info->name));
    settings.setValue("power", QVariant(info->power));
    settings.setValue("cardId",QVariant(info->cardId));
    settings.endArray();
    settings.endGroup();
    list_user<<info;
}

void CabinetConfig::restart()
{
        qApp->closeAllWindows();
#ifdef SIMULATE_ON
    QProcess::startDetached(qApp->applicationFilePath(), QStringList());
#else
//    qDebug()<<"[restart] /home/qtdemo";
    QStringList args;
    args.append("-qws");
    QProcess::startDetached(qApp->applicationFilePath(),args);
#endif

}

UserInfo* CabinetConfig::checkUserLocal(QString userId)
{
    int i = 0;

    for(i=0; i<list_user.count(); i++)
    {
        qDebug()<<list_user.at(i)->cardId<<userId;
        if(list_user.at(i)->cardId == userId)
            return list_user.at(i);
    }

    return NULL;
}

bool CabinetConfig::checkManagers(QString userId)
{
    QFile fManager("/home/config/managers.ini");

    if(!fManager.exists())
        return false;

    fManager.open(QFile::ReadOnly);
    QString managerStr = QString(fManager.readAll());
    fManager.close();

    QStringList managers = managerStr.split(' ');
//    qDebug()<<"[managers]"<<managers<<userId<<managers.indexOf(userId);
//    qDebug()<<managers.at(7);

    if(managers.isEmpty())
        return false;
    if(managers.indexOf(userId) == -1)
        return false;
    else
        return true;
}

int CabinetConfig::getGoodsType(QString packageId)
{
    int index = packageId.indexOf('-');
    if(index == -1)
        return 1;

    return packageId.right(packageId.size()-index-1).toInt();
}

QString CabinetConfig::getPyCh(QString str)
{
    return ChineseLetterHelper::GetFirstLettersAll(str);
//    QString ret = QString();
//    int i = 0;

//    for(i=0; i<str.length(); i++)
//    {
//        ret.append(str2py(str.at(i)));
//    }
//    return ret;
}

QChar CabinetConfig::str2py(QChar ch)
{
    QChar ret;
    QTextCodec* pCodec = QTextCodec::codecForName("gb2312");
    if(!pCodec) return QChar(' ');
    QByteArray qba = pCodec->fromUnicode(QString(ch));


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
    else if(tmp >= 52980 && tmp <= 53688) ret = 'X';
    else if(tmp >= 53689 && tmp <= 54480) ret = 'Y';
    else if(tmp >= 54481 && tmp <= 55289) ret = 'Z';
    else ret = ch;

    return ret;
}

QString CabinetConfig::scanDataTrans(QString code)
{
    int index = code.indexOf("-");
    if(index == -1)
        return code;

    code = code.right(code.size()-index-1);

    index = code.lastIndexOf("-");
    if(index == -1)
        return code;

    return code.left(index);
}
