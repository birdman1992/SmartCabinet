#include "caseaddress.h"

CaseAddress::CaseAddress()
{
    cabinetSeqNum = -1;
    caseIndex = -1;
    goodsIndex = -1;
}

void CaseAddress::setAddress(QPoint pos)
{
    cabinetSeqNum = pos.x();
    caseIndex = pos.y();
}

void CaseAddress::clear()
{
    cabinetSeqNum = -1;
    caseIndex = -1;
    goodsIndex = -1;
}
