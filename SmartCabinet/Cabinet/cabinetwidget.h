#ifndef CABINETWIDGET_H
#define CABINETWIDGET_H

#include <QWidget>
#include <QEvent>
#include "Cabinet/cabinet.h"
#include "cabinetconfig.h"
#include "Structs/caseaddress.h"

namespace Ui {
class CabinetWidget;
}

class CabinetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetWidget(QWidget *parent = 0);
    ~CabinetWidget();

    void panel_init(QList<Cabinet *> cabinets);
    bool installGlobalConfig(CabinetConfig *globalConfig);
    void caseLock();
    void caseUnlock();

public slots:
    void caseClicked(int caseIndex, int cabSeqNum);//柜格下标,柜子顺序编号
    void recvScanData(QByteArray);
    void logoClicked();//logo被点击
    void cabinetInit();

signals:
    void winSwitch(int);

private:
    Ui::CabinetWidget *ui;
    CabinetConfig* config;
    bool clickLock;//点击锁,如果为true，点击无效
    int selectCab;//选中的柜子顺序编号
    int selectCase;//选中的柜格编号
    int storeNum;
    bool waitForCodeScan;
    bool waitForInit;
    QString scanInfo;
    CaseAddress casePos;

    void showEvent(QShowEvent*);
};

#endif // CABINETWIDGET_H
