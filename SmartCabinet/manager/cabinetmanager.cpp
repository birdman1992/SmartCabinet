#include "cabinetmanager.h"
#include "defines.h"
#include <QVariant>
#include <QRegExp>
#include <QStringList>
#include <QSettings>
#include <QDebug>

CabinetManager* CabinetManager::m = new CabinetManager;

CabinetManager *CabinetManager::manager()
{
    return m;
}

void CabinetManager::setCabMap(QString map)
{
    setConfig("col_map", map);
}

void CabinetManager::setCabLayout(QString layout)
{
    setConfig("layout", layout);
}

void CabinetManager::setScrPos(QPoint pos)
{
    setConfig("screen_pos", QString("%1,%2").arg(pos.x()).arg(pos.y()));
}

void CabinetManager::setDepartName(QString name)
{
    setConfig("hospital_department_name", name);
}

void CabinetManager::setHospitalName(QString name)
{
    setConfig("hospital_name", name);
}

void CabinetManager::saveConfig()
{
    setCabLayout(cabLayout);
    setCabMap(cabMap);
    setDepartName(departName);
    setHospitalName(hospitalName);
    setScrPos(scrPos);
}

void CabinetManager::readConfig()
{
    cabLayout = getCabLayout();
    cabMap = getCabMap();
    departName = getDepartName();
    hospitalName = getHospitalName();
    scrPos = getScrPos();
}

void CabinetManager::insertCol(int colPos, QString layout)
{
    QStringList layouts = cabLayout.split('#', QString::SkipEmptyParts);
    layouts.insert(colPos, layout);
    cabLayout = layouts.join("#");

    cabMap.insert(colPos, QString::number(cabMap.length()));

    if(colPos <= scrPos.x())
    {
        scrPos.setX(scrPos.x()+1);
    }
}

QString CabinetManager::getCabMap()
{
    return getConfig("col_map", QString()).toString();
}

QString CabinetManager::getCabLayout()
{
    return getConfig("layout", QString()).toString();
}

QPoint CabinetManager::getScrPos()
{
    QString posStr = getConfig("screen_pos", QString()).toString();
    if(posStr.isEmpty())
        return QPoint(-1, -1);

    QRegExp rep("[0-9]+,[0-9]+");
    if(rep.indexIn(posStr) == -1)
        return QPoint(-1, -1);

    QStringList posList = posStr.split(',', QString::SkipEmptyParts);
    QPoint ret(posList.at(0).toInt(), posList.at(1).toInt());
    qDebug()<<"[getScrPos]";
    return ret;
}

QString CabinetManager::getDepartName()
{
    return getConfig("hospital_department_name", QString()).toString();
}

QString CabinetManager::getHospitalName()
{
    return getConfig("hospital_name", QString()).toString();
}

CabinetManager::CabinetManager(QObject *parent) : QObject(parent)
{
    configPath = CONF_CABINET_INFO;
}

/********base functions*******/
void CabinetManager::setConfig(QString key, QVariant value)
{
    QSettings settings(configPath, QSettings::IniFormat);
    settings.setValue(key, value);
    settings.sync();
}

QVariant CabinetManager::getConfig(QString key, QVariant defaultRet)
{
    QSettings settings(configPath, QSettings::IniFormat);
    return settings.value(key, defaultRet);
}

void CabinetManager::removeConfig(QString path)
{
    QSettings settings(configPath, QSettings::IniFormat);
    settings.remove(path);
}
/********base functions end*******/

