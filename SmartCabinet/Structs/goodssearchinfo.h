#ifndef GOODSSEARCHINFO_H
#define GOODSSEARCHINFO_H
#include <QString>


class GoodsSearchInfo
{
public:
    GoodsSearchInfo();
    QString name;
    QString size;
    QString unit;
    int type;
    int inventory;
    int applyNum;

};

#endif // GOODSSEARCHINFO_H
