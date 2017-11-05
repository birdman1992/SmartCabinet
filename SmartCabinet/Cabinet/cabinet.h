#ifndef CABINET_H
#define CABINET_H

#include <QWidget>
#include <QList>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include "Structs/cabinetinfo.h"
#include "Structs/caseaddress.h"
#include "Widgets/casepanel.h"

namespace Ui {
class Cabinet;
}

class Cabinet : public QWidget
{
    Q_OBJECT

public:
    explicit Cabinet(QWidget *parent = 0);
    ~Cabinet();

    void CabinetInit(int _width, int seq, int pos, int num, bool mainCab);//顺序编号,位置编号,柜格数,是否为主柜
    void setCabPos(int pos);
    void setCabType(int _type);
    void checkCase(int index);
    void addCase(GoodsInfo *info, int caseIndex, bool doubleCol);//用于读取配置信息添加到信息列表尾部
    void updateCase(int caseIndex);
    void setCtrlWord(int caseIndex, QByteArray seq, QByteArray index);
    int getMaxshowNum(int caseIndex);
    bool haveEmptyPos(int caseIndex);

    int getIndexByName(QString findName);//根据药品名检索柜格下标
    void consumableIn(CaseAddress addr, int num=1);//药品存放
    void consumableOut(CaseAddress addr, int num=1);//药品取出
    void updateGoodsNum(CaseAddress addr, int num);//更新物品数量
    void updateCabinetCase(CaseAddress addr);
    int cabinetPosNum();//获取位置编号
    void clearSelectState(int row);//清除选中状态
    void showMsg(QString msg, bool showBigCharacter);
    void setCaseName(GoodsInfo info, int index);//设置柜格存放物品名
    bool isInLeft();//true:柜子在主柜左边 false:柜子在主柜右边
    void searchByPinyin(QString ch);
    void clearSearch();

    QList<CabinetInfo*> list_case;//柜格列表

private slots:
    void on_tableWidget_cellClicked(int row, int column);

signals:
    void caseSelect(int caseIndex, int seqNum);//柜格下标,柜子序号
    void logoClicked();

private:
    Ui::Cabinet *ui;
    int state;
    int caseWidth;
    bool isMainCabinet;//是否为主柜
    int seqNum;//顺序编号
    int posNum;//位置编号
    int caseNum;//柜格数
    int cabType;//柜子型号:0：副柜，1：单列主柜，2：双列主柜
    QLabel* logo;

    void caseDraw(int _type);
    void setCase(CabinetInfo* info);
    void setCaseState(int index, int numState);//设置柜格状态
    int getCaseState();
    QString cellStyle(QColor rgb);
    QString checkStyle();
    bool eventFilter(QObject *obj, QEvent *event);
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent *);
};

#endif // CABINET_H
