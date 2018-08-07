#ifndef DAYREPORTINFO_H
#define DAYREPORTINFO_H
#include <QString>

class DayReportInfo
{
public:
    DayReportInfo();
    QString goodsId;
    QString goodsName;
    QString size;
    QString proName;//生产商
    QString supplyName;//供应商
    QString goodsCount;//打包数量
    double price;//单价
    double sumCount;//总价
    int state;//2 正常消耗  4 盘点消耗
    QString optTime;
    QString traceId;
    QString unit;
    QString batchNumber;
    QString optName;
};

#endif // DAYREPORTINFO_H
