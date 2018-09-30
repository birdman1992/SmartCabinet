#ifndef CABINETWIDGET_H
#define CABINETWIDGET_H

#include <QWidget>
#include <QEvent>
#include <QMessageBox>
#include <QTimer>
#include <QPaintEvent>
#include <QPainter>
#include <QButtonGroup>
#include <QSlider>
#include "Cabinet/cabinet.h"
#include "Widgets/cabinetaccess.h"
#include "Widgets/cabinetlistview.h"
#include "Widgets/cabinetcheck.h"
#include "Widgets/cabinetstorelist.h"
#include "Widgets/cabinetrefund.h"
#include "Device/Network/networkset.h"
#include "cabinetconfig.h"
#include "Structs/caseaddress.h"
#include "Structs/goodslist.h"
#include "Structs/goodscar.h"

bool posSort(Cabinet* A, Cabinet* B);

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
    void recvUserCheckRst(UserInfo *);//接收用户校验结果
    void recvUserInfo(QByteArray qba);//接收用户信息
    void recvListInfo(GoodsList*);
    void recvBindRst(bool);
    void recvGoodsCheckRst(QString msg);
    void recvGoodsNumInfo(QString goodsId, int num);
    void accessFailedMsg(QString msg);
    void updateTime();
    void updateId();
    void newGoodsList(QString listCode, QString rfidCode);
    void readyGoodsList(QString listCode);
    void sysLock();
    void recvCabSyncResult(bool);
    void recvCheckRst(bool);

signals:
    void winSwitch(int);
    void goodsAccess(CaseAddress, QString, int, int);//柜格坐标，完整条码,数量，操作码(1取货2存货3退货)
    void requireUserCheck(QString);//请求身份验证
    void requireGoodsListCheck(QString);//请求送货单验证
    void requireOpenCase(int seqNum, int index);
    void requireCaseBind(int  seqNum, int index, QString goodsId);
    void goodsNumChanged(int);//报告物品数量的变化
    void checkLockState();//检查锁状态
    void scanData(QByteArray qba);
    void requireFetchList();
    void requireAccessList(QStringList list, int optType);
    void checkCase(QList<CabinetCheckItem*> l, CaseAddress addr);
    void checkCase(QStringList, CaseAddress);
    void storeList(QList<CabinetStoreListItem*>);
    void requireTimeout();
    void requireCabSync();
    void requireGoodsCheck();
    void goodsCheckFinish();
    void tsCalReq();

private slots:
    void setMenuHide(bool ishide);
    void cabinetBind(Goods* goods);
    void checkOneCase(QList<CabinetCheckItem*> l, CaseAddress addr);
    void checkOneCase(QStringList l, CaseAddress addr);
    //    void on_fetch_clicked();
    void wait_timeout();
    void saveStore(Goods* goods, int num);
    void saveFetch(QString name, int num);
//    void on_fetch_toggled(bool checked);
    void on_store_clicked(bool checked);
    void pinyinSearch(int);
    void updateNetState(bool);
    void on_service_clicked(bool checked);
    void on_refund_clicked(bool checked);
    void on_cut_clicked();
    void on_check_clicked(bool checked);
    void on_search_clicked();
    void on_search_back_clicked();
    void on_searchClear_clicked();
    void on_netState_clicked();
    void on_volCtrl_clicked();
    void vol_changed(int);
    void vol_released();
    void vol_pressed();
    void syncMsgTimeout();

    void on_quit_clicked();

protected:
    bool eventFilter(QObject *, QEvent *);

private:
    Ui::CabinetWidget *ui;
    CabinetConfig* config;
    QButtonGroup groupBtn;
    QSlider* volume;//音量控件
    QString curCard;
    int tsCalFlag;
    bool volPressed;
    bool waitForCardReader;
    bool waitForGoodsListCode;
    bool loginState;
    bool netCheckState;
    bool clickLock;//点击锁,如果为true，点击无效
    int selectCab;//选中的柜子顺序编号
    int selectCase;//选中的柜格编号
    int bindCab;
    int bindCase;
    GoodsInfo bindInfo;

    int storeNum;
    bool waitForCodeScan;
    bool waitForInit;
    bool waitForServer;
    bool checkMask;
    QString scanInfo;
    QString fullScanInfo;
    UserInfo* optUser;//操作者id
    CaseAddress casePos;
    CaseAddress rebind_old_addr;
    CaseAddress rebind_new_addr;
    QMessageBox* msgBox;
    GoodsList* curStoreList;
    Goods* curGoods;
    GoodsInfo* rebindGoods;
    CabinetAccess* win_access;//存取窗口
    CabinetListView* win_cab_list_view;//柜子列表视图窗口
    CabinetCheck* win_check;//盘点窗口
    CabinetStoreList* win_store_list;
    CabinetRefund* win_refund;//退货窗口
    NetworkSet* win_net_set;//网络配置

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
    void initVolum();
    bool needWaitForServer();
    void showCurrentTime(QString curTime);
    void rebindRecover();//重绑定恢复
    void rebindOver();//重绑定完成
    void clearCheckState();
    void clearMenuState();
    void volumTest();
    QByteArray scanDataTrans(QByteArray code);//扫描条码转换
    void calCheck(QString card);
};

#endif // CABINETWIDGET_H
