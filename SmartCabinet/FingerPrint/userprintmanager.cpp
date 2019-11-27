#include "userprintmanager.h"
#include <QDebug>

#define FINGER_CONFIG "/home/config/finger_print"

UserPrintManager* UserPrintManager::m = new UserPrintManager;

UserPrintManager::UserPrintManager()
{
    if(!QDir(FINGER_CONFIG).exists())
    {
        QDir dir;
        dir.mkdir(FINGER_CONFIG);
    }
}

UserPrintManager *UserPrintManager::manager()
{
    return m;
}

int UserPrintManager::getNewUserID(QString cardId)
{
    int ret = getFingerId(cardId);
    if(ret != -1)
        return ret;

    QDir dir(FINGER_CONFIG);
    QStringList userList = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for(int i=1; ; i++)
    {
        if(userList.indexOf(QString::number(i)) == -1)
            return i;
    }
}

void UserPrintManager::setUserInfo(int userId, QString cardId, QString name, QByteArray fingerData)
{
    QString userConfig = QString(FINGER_CONFIG) + "/users.ini";
    QString fingerDir = QString(FINGER_CONFIG) + QString("/%1").arg(userId);
    QString fingerConfig = fingerDir + QString("/user.ini");

    QDir dir(fingerDir);
    if(!dir.exists())
        dir.mkdir(fingerDir);

    QSettings userSettings(userConfig, QSettings::IniFormat);
    userSettings.setValue(QString("%1/fingerID").arg(cardId), userId);
    userSettings.setValue(QString("%1/name").arg(cardId), name);
    userSettings.sync();

    QSettings fingerSettings(fingerConfig ,QSettings::IniFormat);
    fingerSettings.setValue("cardID", cardId);
    fingerSettings.setValue("name", name);
    fingerSettings.sync();

    QFile f(fingerDir+"/user.fpt");
    if(!f.open(QFile::WriteOnly))
    {
        qDebug()<<"[setUserInfo] open user.fpt failed";
    }

    f.write(fingerData);
    f.close();
}

void UserPrintManager::setCtrlConfig(QByteArray confSeq, QByteArray confIndex)
{
    qDebug()<<"[setCtrlConfig]"<<confSeq.toHex()<<confIndex.toHex();
    QString userConfig = QString(FINGER_CONFIG) + "/users.ini";
    QSettings userSettings(userConfig, QSettings::IniFormat);
    userSettings.setValue("LockSeq", confSeq);
    userSettings.setValue("LockIndex", confIndex);
    userSettings.sync();
}

int UserPrintManager::getFingerId(QString cardId)
{
    QString userConfig = QString(FINGER_CONFIG) + "/users.ini";
    QSettings userSettings(userConfig, QSettings::IniFormat);
    return userSettings.value(QString("%1/fingerID").arg(cardId), -1).toInt();
}

QByteArray UserPrintManager::getCardId(int fingerId)
{
    QString fingerDir = QString(FINGER_CONFIG) + QString("/%1").arg(fingerId);
    QString fingerConfig = fingerDir + QString("/user.ini");

    QDir dir(fingerDir);
    if(!dir.exists())
        return QByteArray();

    QSettings fingerSettings(fingerConfig, QSettings::IniFormat);
    QByteArray ret = fingerSettings.value(QString("cardID"), QString()).toString().toLocal8Bit();
    return ret;
}

QByteArray UserPrintManager::getSeqConfig()
{
    QString userConfig = QString(FINGER_CONFIG) + "/users.ini";
    QSettings userSettings(userConfig, QSettings::IniFormat);
    return userSettings.value("LockSeq", QByteArray::fromHex("00000000000000000000000000000000")).toByteArray();
}

QByteArray UserPrintManager::getIndexConfig()
{
    QString userConfig = QString(FINGER_CONFIG) + "/users.ini";
    QSettings userSettings(userConfig, QSettings::IniFormat);
    return userSettings.value("LockIndex", QByteArray::fromHex("000102030405060708090a0b0c0d0e0f")).toByteArray();
}


