#include "cabinetinfo.h"

CabinetInfo::CabinetInfo()
{

}

bool CabinetInfo::caseSearch(QString name)
{
    int i = 0;

    for(i=0; i<list_goods.count(); i++)
    {
        if(list_goods.at(i)->name == name)
        {
            return true;
        }
    }

    return false;
}

QString CabinetInfo::caseShowStr()
{
    QString ret = QString();
    int i = 0;

    for(i=0; i<list_goods.count(); i++)
    {
        QString str = list_goods.at(i)->name;
        if(!str.isEmpty())
        {
            str += str+QString("×%1").arg(list_goods.at(i)->num);
            str += "\n";
        }
    }
    return ret;
}
