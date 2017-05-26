#ifndef CABINETINFO_H
#define CABINETINFO_H
#include <QString>

class CabinetInfo//单个柜格的耗材信息
{
public:
    CabinetInfo();
    QString name;//耗材名
    QString id;//耗材id
    QString unit;//计数单位
    int num;//耗材数量
};

#endif // CABINETINFO_H
