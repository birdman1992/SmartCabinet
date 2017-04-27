/****************************************
 * 信息储存类
 *
 *
 *
 *
 *
* *************************************/
#ifndef MEDINF_H
#define MEDINF_H
#include <QString>

class MedInf
{
public:
    MedInf();
    int num;//--序号
    QString name;//--药材名字
    QString Features;//--药材功能
    QString application;//--药材的应用
    QString ShelfLife;//--药材保质期
    QString ProductionDate;//--药材生产日期
};

#endif // MEDINF_H
