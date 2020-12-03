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
#include <QMap>
#include "Cabinet/cabinet.h"
#include "Widgets/cabinetaccess.h"
#include "Widgets/cabinetlistview.h"
#include "Widgets/cabinetcheck.h"
#include "Widgets/cabinetstorelist.h"
#include "Widgets/cabinetrefund.h"
#include "Widgets/checkwarning.h"
#include "Widgets/dayreport.h"
#include "Device/Network/networkset.h"
#include "cabinetconfig.h"
#include "Structs/caseaddress.h"
#include "Structs/goodslist.h"
#include "Structs/goodscar.h"
#include "manager/goodsmanager.h"
#include "manager/signalmanager.h"
#include "sql/sqlmanager.h"

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
    void updateCase(int col, int row);
    void updateCase(QPoint pos);
    void updateOptStamp();//更新操作时间戳

public slots:
    void switchCabinetState(CabState state);
    void caseClicked(int caseIndex, int cabSeqNum);//柜格下标,柜子顺序编号
    void recvScanData(QByteArray);
    void updateScanState(bool);
    void logoClicked();//logo被点击
    void cabinetInit();
    void updateDelay(int delay);
    void recvUserCheckRst(UserInfo *);//接收用户校验结果
    void recvUserInfo(QByteArray qba);//接收用户信息
    void recvListInfo(GoodsList*);
    void recvBindRst(bool);
    void recvGoodsCheckRst(QString msg);
    void recvGoodsNumInfo(QString goodsId, int num);
    void accessFailedMsg(QString msg);
    void accessSuccessMsg(QString msg);
    void updateFetchPrice(float, float);
    void updateTime();
    void updateId();
    void newGoodsList(QString listCode, QString rfidCode);
    void readyGoodsList(QString listCode);
    void sysLock();
    void recvCabSyncResult(bool);
    void recvCheckRst(bool, QString msg);//master
    void recvCheckFinish(bool);
    void recvCheckCreatRst(bool, QString msg);//new_api
    void recvCheckFinishRst(bool, QString msg);
    void recvGoodsTraceRst(bool, QString msg, QString goodsCode);
    void on_check_clicked(bool checked);

signals:
//    void updateLoginState(int id, bool isLogin);//登录状态更新,登入亮灯，登出灭灯
    void updateLoginState(bool isLogin);
    void winSwitch(int);
    void screenPro(bool);//屏保状态
    void goodsAccess(QPoint, QString, int, int);//柜格坐标，完整条码,数量，操作码(1取货2存货3退货)
    void requireUserCheck(QString);//请求身份验证
    void requireGoodsListCheck(QString);//请求送货单验证
    void requireOpenCase(int seqNum, int index);
    void requireCaseBind(int  seqNum, int index, QString goodsId);
    void requireCaseRebind(int  seqNum, int index, QString goodsId);
    void reportTraceId(QString traceId);
    void goodsNumChanged(int);//报告物品数量的变化
    void checkLockState();//检查锁状态
    void scanData(QByteArray qba);
    void requireFetchList();
    void requireAccessList(QStringList list, int optType);
    void checkCase(QList<CabinetCheckItem*> l, CaseAddress addr);
    void checkCase(QStringList, CaseAddress);
    void storeList(QString barcode ,QList<CabinetStoreListItem*>);
    void newStoreBarCode(QString);
    void requireTimeout();
    void requireCabSync();
    void requireGoodsCheck();
    void goodsCheckFinish();
    void requireCheckShow();
    void requireSearchShow();
    void requireDayReportShow();
    void requireApplyShow();
    void tsCalReq();
    void setSpecialCase(QPoint);
    void reqCheckVersion(bool);
    void cpuFanOn(bool);
    void stack_switch(int index);
    void loginStateChanged(bool);

private slots:
    void msgClear();
    void setMenuHide(bool ishide);
    void cabinetBind(Goods* goods);
    void checkOneCase(QList<CabinetCheckItem*> l, CaseAddress addr);
    void checkOneCase(QStringList l, CaseAddress addr);
    void checkPush();
    void saveStore(Goods* goods, int num);
    void saveFetch(QString name, int num);
    void searchByPinyin(QString str);
//    void on_fetch_toggled(bool checked);
    void on_store_clicked(bool checked);
    void pinyinSearch(int);
    void updateNetState(bool);
    void on_service_clicked(bool checked);
    void on_refund_clicked(bool checked);
    void on_cut_clicked();
//    void on_check_clicked(bool checked);
//    void on_check_toggled(bool checked);
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
    void on_btn_check_table_clicked();
    void on_reply_clicked();
    void on_consume_date_clicked();

    void on_back_clicked(bool checked);

    void on_check_toggled(bool checked);

    void on_rebind_clicked(bool checked);

    void on_store_toggled(bool checked);
protected:
    bool eventFilter(QObject *, QEvent *);
    void resizeEvent(QResizeEvent*);

private:
    Ui::CabinetWidget *ui;
    CabinetConfig* config;
    SqlManager* sqlManager;
    CheckWarning* win_check_warnning;
    QButtonGroup groupBtn;
    QSlider* volume;//音量控件
    QString curCard;
    QTimer* timeUpdater;
    QTime lastOptTime;
    bool screenProState;
    QMap<QWidget*, bool> showMap;
    QList<Cabinet *> list_cabinet;
    QList<QPoint> list_state_case;//搜索状态的柜格
    int tsCalFlag;

    bool volPressed;
    bool waitForCardReader;
    bool waitForGoodsListCode;
    bool loginState;
    bool netCheckState;
    bool waitForCheckFinish;
    bool clickLock;//点击锁,如果为true，点击无效
    int selectCab;//选中的柜子顺序编号
    int selectCase;//选中的柜格编号
    int bindCab;
    int bindCase;
    Goods bindInfo;
    GoodsManager* goodsManager;

    int storeNum;
    bool waitForCodeScan;
    bool waitForInit;
    bool waitForSecondaryCard;
    bool checkMask;
    QString scanInfo;
    QString fullScanInfo;
    QString scanGoodsId;
    UserInfo* optUser;//操作者id
    CaseAddress casePos;
    CaseAddress rebind_old_addr;
    CaseAddress rebind_new_addr;
    QMessageBox* msgBox;
    GoodsList* curStoreList;
    Goods* curGoods;
    QString rebindGoods;
    CabinetAccess* win_access;//存取窗口
    CabinetListView* win_cab_list_view;//柜子列表视图窗口
    CabinetCheck* win_check;//盘点窗口
    CabinetStoreList* win_store_list;
    CabinetRefund* win_refund;//退货窗口
    NetworkSet* win_net_set;//网络配置

    void showEvent(QShowEvent*);
    void updateShowMap();
    void hideAllShowMap();
    void setPowerState(int power);//设置权限状态
    void paintEvent(QPaintEvent *);
    void cabLock();
    void cabInit();
    void cabInfoBind(int seq, int index, QString info);
    void initAccessState();
    void initSearchBtns();
    void initVolum();
    void showCurrentTime(QString curTime);
    void rebindRecover();//重绑定恢复
    void rebindOver();//重绑定完成
    void clearCheckState();
    void clearMenuState();
    void volumTest();
    void checkStart();//盘点开始 master&new_api
    void checkCreat();
    void calCheck(QString card);
    void magicCmd(QString cmd);
    QByteArray scanDataTrans(QByteArray code);//扫描条码转换
    bool isListCode(QByteArray qba);
    void setSearchState(QList<QPoint> l);
    void clearCaseState();

    bool caseHasPos(int seq, int index);
    void checkCabinetCase(int seq, int index);
    void searchCabinetCase(int seq, int index);
};

#endif // CABINETWIDGET_H
