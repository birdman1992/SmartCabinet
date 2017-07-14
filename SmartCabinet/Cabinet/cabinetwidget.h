#ifndef CABINETWIDGET_H
#define CABINETWIDGET_H

#include <QWidget>
#include <QEvent>
#include <QMessageBox>
#include <QTimer>
#include <QPaintEvent>
#include <QPainter>
#include <QButtonGroup>
#include "Cabinet/cabinet.h"
#include "Widgets/cabinetaccess.h"
#include "Widgets/cabinetlistview.h"
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
    void recvUserCheckRst(UserInfo);//接收用户校验结果
    void recvUserInfo(QByteArray qba);//接收用户信息
    void recvListInfo(GoodsList*);
    void recvBindRst(bool);
    void recvGoodsNumInfo(QString goodsId, int num);
    void updateTime();

signals:
    void winSwitch(int);
    void goodsAccess(CaseAddress, QString, int, int);//柜格坐标，完整条码,数量，操作码(1取货2存货3退货)
    void requireUserCheck(QString);//请求身份验证
    void requireGoodsListCheck(QString);//请求送货单验证
    void requireOpenCase(int seqNum, int index);
    void requireCaseBind(int  seqNum, int index, QString goodsId);
    void goodsNumChanged(int);//报告物品数量的变化
    void checkLockState();//检查锁状态

private slots:
    void on_store_clicked();
//    void on_fetch_clicked();
    void wait_timeout();
    void saveStore(Goods* goods, int num);
    void saveFetch(QString name, int num);
//    void on_fetch_toggled(bool checked);
    void on_store_toggled(bool checked);
    void pinyinSearch(int);

    void on_service_toggled(bool checked);

    void on_refund_toggled(bool checked);

    void on_cut_clicked();

private:
    Ui::CabinetWidget *ui;
    CabinetConfig* config;
    QButtonGroup groupBtn;
    bool waitForCardReader;
    bool waitForGoodsListCode;
    bool clickLock;//点击锁,如果为true，点击无效
    int selectCab;//选中的柜子顺序编号
    int selectCase;//选中的柜格编号
    int storeNum;
    bool waitForCodeScan;
    bool waitForInit;
    bool waitForServer;
    QString scanInfo;
    QString fullScanInfo;
    UserInfo optUser;//操作者id
    CaseAddress casePos;
    QMessageBox* msgBox;
    GoodsList* curStoreList;
    Goods* curGoods;
    CabinetAccess* win_access;//存取窗口
    CabinetListView* win_cab_list_view;//柜子列表视图窗口

    void showEvent(QShowEvent*);
    void warningMsgBox(QString title, QString msg);
    void msgClear();
    void msgShow(QString title, QString msg, bool setmodal);
    void setPowerState(int power);//设置权限状态
    void paintEvent(QPaintEvent *);
    void cabLock();
    void cabInfoBind(int seq, int index, GoodsInfo info);
    void initAccessState();
    void initSearchBtns();
    bool needWaitForServer();
    void showCurrentTime(QString curTime);
    QByteArray scanDataTrans(QByteArray code);//扫描条码转换
};

#endif // CABINETWIDGET_H
