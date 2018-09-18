#ifndef CASEADDRESS_H
#define CASEADDRESS_H
#include <QPoint>


class CaseAddress//用于描述柜格位置
{
public:
    CaseAddress();
    void setAddress(QPoint pos);
    void clear();
    int cabinetSeqNum;//智能柜顺序编号
    int caseIndex;//柜格编号
    int goodsIndex;//物品柜格内序号
};

#endif // CASEADDRESS_H
