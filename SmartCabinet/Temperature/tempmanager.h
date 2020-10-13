#ifndef TEMPMANAGER_H
#define TEMPMANAGER_H
#include <QString>
#include <QMap>
#include <QVector2D>
#include "tempcase.h"
class TempCase;

class TempManager
{
public:
    static TempManager* manager();
    void addTempDevice(QString devIp, TempCase* dev);
    QString searchDeviceForIp(QString devIp);//根据ip查询设备id
    QString searchNameForIp(QString devIp);

    void setDevMaxTemp(QString devId, int t);
    void setDevMinTemp(QString devId, int t);
    void setDevWarningTemp(QString devId, int t);
    void setDevReportTime(QString devId, int t);
    void setDevName(QString devId, QString devName);
    void setDevSoundOff(QString devId, bool soundOff);

    QString getDeviceIp(QString caseId);
    int getDevMaxTemp(QString devId);
    int getDevMinTemp(QString devId);
    int getDevWarningTemp(QString devId);
    int getDevReportTime(QString devId);
    QString getDevName(QString devId);
    bool getDevSoundOff(QString devId);
    void configInit();

    QStringList getTempRecordList();//获取有温度记录的设备列表
    QVector<QVector2D> getDeviceTempRecord(QString devId, QDate startDate, QDate endDate);
    void creatTempData(QString devId, int days, int interval);//生成devId从currentDate-days~currentDate的数据


private:
    TempManager();
    static TempManager* m;
    QString appPath;
    QString configPath;
    QMap<QString, TempCase*> devMap;
    void setConfig(QString key, QVariant value);
    QVariant getConfig(QString key, QVariant defaultRet);
    void removeConfig(QString path);
};

#endif // TEMPMANAGER_H
