#ifndef CABINETWIDGET_H
#define CABINETWIDGET_H

#include <QWidget>
#include <QEvent>
#include <QMessageBox>
#include <QTimer>
#include "Cabinet/cabinet.h"
#include "cabinetconfig.h"
#include "Structs/caseaddress.h"
#include "Structs/goodslist.h"

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
    void recvUserCheckRst(bool);//接收用户校验结果
    void recvUserInfo(QByteArray qba);
signals:
    void winSwitch(int);
    void requireUserCheck(QString);//请求身份验证
    void requireGoodsListCheck(QString);//请求送货单验证

private slots:
    void on_store_clicked();

    void on_fetch_clicked();

    void wait_timeout();
private:
    Ui::CabinetWidget *ui;
    CabinetConfig* config;
    bool waitForCardReader;
    bool waitForGoodsListCode;
    bool clickLock;//点击锁,如果为true，点击无效
    int selectCab;//选中的柜子顺序编号
    int selectCase;//选中的柜格编号
    int storeNum;
    bool waitForCodeScan;
    bool waitForInit;
    QString scanInfo;
    QString optUser;//操作者id
    CaseAddress casePos;
    QMessageBox* msgBox;

    void showEvent(QShowEvent*);
    void warningMsgBox(QString title, QString msg);
    void msgClear();
    void msgShow(QString title, QString msg, bool setmodal);
};

#endif // CABINETWIDGET_H
