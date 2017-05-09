#ifndef CABINET_H
#define CABINET_H

#include <QWidget>
#include <QList>
#include "Structs/cabinetinfo.h"

namespace Ui {
class Cabinet;
}

class Cabinet : public QWidget
{
    Q_OBJECT

public:
    explicit Cabinet(QWidget *parent = 0);
    ~Cabinet();

    void CabinetInit(int seq, int pos, int num, bool mainCab);//顺序编号,位置编号,柜格数,是否为主柜
    void addCase(CabinetInfo* info);

private:
    Ui::Cabinet *ui;
    bool isMainCabinet;//是否为主柜
    int seqNum;//顺序编号
    int posNum;//位置编号
    int caseNum;//柜格数
    QList<CabinetInfo*> list_case;//柜格列表
};

#endif // CABINET_H
