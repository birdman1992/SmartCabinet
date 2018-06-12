#include "goodscheckinfo.h"
#include <QDebug>

GoodsCheckInfo::GoodsCheckInfo()
{

}

CheckList::CheckList()
{

}

CheckList::CheckList(CheckList *l)
{
    strTime = l->strTime;
    startTime = l->startTime;
    finishTime = l->finishTime;
    departCode = l->departCode;
    listInfo = l->listInfo;
}

void CheckList::setCheckDateTime(QString str)
{
    if(str.length()<40)
        return;

    QString timeStart = str.left(19);
    QString timeFinish = str.mid(23,19);
    qDebug()<<"[setCheckDateTime]"<<timeStart<<"-"<<timeFinish;
    startTime = QDateTime::fromString(timeStart, "yyyy-MM-dd hh:mm:ss");
    finishTime = QDateTime::fromString(timeFinish, "yyyy-MM-dd hh:mm:ss");
    strTime = timeStart+"-"+timeFinish;
}

void CheckList::addInfo(GoodsCheckInfo * info)
{
    listInfo<<info;
}


CheckTableInfo::CheckTableInfo()
{

}
