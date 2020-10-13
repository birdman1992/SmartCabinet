#include "tempmanager.h"
#include <QApplication>
#include <QSettings>
#include <QtGlobal>
#include "config.h"

TempManager* TempManager::m = new TempManager;

TempManager *TempManager::manager()
{
    return m;
}

void TempManager::addTempDevice(QString devIp, TempCase *dev)
{
    if(dev->devId().isEmpty())
        return;

    setConfig(QString("%1/%2").arg(dev->devId()).arg("IP"), devIp);
}

QString TempManager::searchDeviceForIp(QString devIp)
{
    QSettings settings(configPath, QSettings::IniFormat);
    QStringList devices = settings.childGroups();
    foreach(QString device, devices)
    {
        if(settings.value(QString("%1/IP").arg(device)).toString() == devIp)
        {
            return device;
        }
    }
    return QString();
}

QString TempManager::searchNameForIp(QString devIp)
{
    QSettings settings(configPath, QSettings::IniFormat);
    QStringList devices = settings.childGroups();
    foreach(QString device, devices)
    {
        if(settings.value(QString("%1/IP").arg(device)).toString() == devIp)
        {
            QString strName = settings.value(QString("%1/DEV_NAME").arg(device)).toString();
            qDebug()<<"search name"<<strName;
            if(!strName.isEmpty())
            {
                return strName;
            }
        }
    }
    return QString();

}

void TempManager::setDevMaxTemp(QString devId, int t)
{
    setConfig(QString("%1/W_MAX").arg(devId), t);
}

void TempManager::setDevMinTemp(QString devId, int t)
{
    setConfig(QString("%1/W_MIN").arg(devId), t);
}

void TempManager::setDevWarningTemp(QString devId, int t)
{
    setConfig(QString("%1/W_WARN").arg(devId), t);
}

void TempManager::setDevReportTime(QString devId, int t)
{
    setConfig(QString("%1/REPORT_TIME").arg(devId), t);
}

void TempManager::setDevName(QString devId, QString devName)
{
    setConfig(QString("%1/DEV_NAME").arg(devId), devName);
}

void TempManager::setDevSoundOff(QString devId, bool soundOff)
{
    setConfig(QString("%1/DEV_SOUND_OFF").arg(devId), soundOff);
}

QString TempManager::getDeviceIp(QString caseId)
{
    return getConfig(QString("%1/IP").arg(caseId), QString()).toString();
}

int TempManager::getDevMaxTemp(QString devId)
{
    return getConfig(QString("%1/W_MAX").arg(devId), 8).toInt();
}

int TempManager::getDevMinTemp(QString devId)
{
    return getConfig(QString("%1/W_MIN").arg(devId), 2).toInt();
}

int TempManager::getDevWarningTemp(QString devId)
{
    return getConfig(QString("%1/W_WARN").arg(devId), 16).toInt();
}

int TempManager::getDevReportTime(QString devId)
{
    return getConfig(QString("%1/REPORT_TIME").arg(devId), 10).toInt();
}

QString TempManager::getDevName(QString devId)
{
    return getConfig(QString("%1/DEV_NAME").arg(devId), QString()).toString();
}

bool TempManager::getDevSoundOff(QString devId)
{
    return getConfig(QString("%1/DEV_SOUND_OFF").arg(devId), QVariant(false)).toBool();
}

void TempManager::configInit()
{
    appPath = qApp->applicationDirPath();
    configPath = appPath+"/TempDevice.ini";
    qDebug()<<"apppath:"<<appPath<<configPath;
}

QStringList TempManager::getTempRecordList()
{
    QString logPath = appPath + PATH_TEMP_LOG;
    QDir logDir(logPath);
    logDir.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    logDir.setSorting(QDir::Name);
    QStringList devs = logDir.entryList();
    return devs;
}

QVector<QVector2D> TempManager::getDeviceTempRecord(QString devId, QDate startDate, QDate endDate)
{
    QString logPath = appPath + PATH_TEMP_LOG + devId + "/";
    QDir logDir(logPath);
    logDir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    logDir.setSorting(QDir::Name);
    QStringList logs = logDir.entryList();
    QVector<QVector2D> ret = QVector<QVector2D>();
    if(startDate.daysTo(endDate)>7)
        endDate = startDate.addDays(7);

    if(logs.isEmpty())
        return ret;
    foreach(QString f, logs)//过滤startDate前的数据文件
    {
        if(f < startDate.toString("yyyyMMdd"))
            logs.removeOne(f);
        if(f> endDate.toString("yyyyMMdd"))
            logs.removeOne(f);
    }
    foreach(QString f, logs)
    {
        QFile fData(logPath+f);
        if(!fData.open(QFile::ReadOnly))
            continue;

        QDateTime lastDataTime = QDateTime();

        while(1)
        {
            QString fLine = QString(fData.readLine());
            if(fLine.size()<10)
                break;
            QStringList datas = fLine.split(' ', QString::SkipEmptyParts);
            QDateTime cDate = QDateTime::fromString(f+datas.at(0), "yyyyMMddhh:mm:ss");
            if(cDate<=lastDataTime.addSecs(300))
            {
//                qDebug()<<"continue";
                continue;
            }
            lastDataTime = cDate;
            QVector2D v = QVector2D(cDate.addSecs(-3600*8).toTime_t(), datas.at(2).toDouble());
            ret.push_back(v);
        }
        fData.close();
    }
    qDebug()<<ret.size();
    return ret;
}

void TempManager::creatTempData(QString devId, int days, int interval)
{
    QString logPath = appPath + PATH_TEMP_LOG + devId + "/";
    QDateTime cDate = QDateTime::currentDateTime().addDays(-days);
    QDate fileDate = cDate.date();
    QString startTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QFile fData(logPath+fileDate.toString("yyyyMMdd"));
    fData.open(QFile::WriteOnly);

    int state;
    float wData,sData;
    qsrand(cDate.toTime_t());

    qDebug()<<"[creatTempData start]"<<devId<<cDate<<interval;

    while(cDate<QDateTime::currentDateTime())
    {
        if(cDate.date()>fileDate)
        {
            fileDate = cDate.date();
            fData.close();
            fData.setFileName(logPath+fileDate.toString("yyyyMMdd"));
            fData.open(QFile::WriteOnly);
        }
        state = qrand()%120/100;
        wData = (float)(qrand()%100+20)/10.0;
        sData = (float)(qrand()%200+400)/10.0;
        QString writeData = QString("%1 %2 %3 %4 \r\n").arg(cDate.time().toString("hh:mm:ss")).arg(state).arg(wData).arg(sData);
        fData.write(writeData.toLocal8Bit());
        cDate = cDate.addSecs(interval);
//        qDebug()<<writeData;
    }
    fData.close();
    QString endTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    qDebug()<<"[creatTempData finish]"<<startTime<<endTime;
}

TempManager::TempManager()
{

}

void TempManager::setConfig(QString key, QVariant value)
{
    QSettings settings(configPath, QSettings::IniFormat);
    settings.setValue(key, value);
    settings.sync();
}

QVariant TempManager::getConfig(QString key, QVariant defaultRet)
{
    QSettings settings(configPath, QSettings::IniFormat);
    return settings.value(key, defaultRet);
}

void TempManager::removeConfig(QString path)
{
    QSettings settings(configPath, QSettings::IniFormat);
    settings.remove(path);
}
