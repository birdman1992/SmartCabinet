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
    void setCabPos(int pos);
    void addCase(CabinetInfo* info);//用于读取配置信息添加到信息列表尾部

    int getIndexByName(QString findName);//根据药品名检索柜格下标
    void consumableIn(int index);//药品存放
    void consumableOut(int index);//药品取出
    int cabinetPosNum();//获取位置编号
    void clearSelectState(int row);//清除选中状态
    void showMsg(QString msg, bool showBigCharacter);
    void setCaseName(QString name, int index);//设置柜格存放物品名
    bool isInLeft();//true:柜子在主柜左边 false:柜子在主柜右边

    QList<CabinetInfo*> list_case;//柜格列表

private slots:
    void on_tableWidget_cellClicked(int row, int column);

signals:
    void caseSelect(int caseIndex, int seqNum);//柜格下标,柜子序号
    void logoClicked();

private:
    Ui::Cabinet *ui;
    bool isMainCabinet;//是否为主柜
    int seqNum;//顺序编号
    int posNum;//位置编号
    int caseNum;//柜格数
    int state;//0:

    void setCase(CabinetInfo* info);
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // CABINET_H