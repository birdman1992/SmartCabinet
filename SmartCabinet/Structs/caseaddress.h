#ifndef CASEADDRESS_H
#define CASEADDRESS_H


class CaseAddress//用于描述柜格位置
{
public:
    CaseAddress();
    int cabinetSeqNUM;//智能柜顺序编号
    int caseIndex;//柜格编号
};

#endif // CASEADDRESS_H