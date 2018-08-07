#ifndef CABINETSERVER_H
#define CABINETSERVER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDateTime>
#include <QProcess>
#include <QTimer>
#include <QStringList>

#include "cabinetconfig.h"
#include "Structs/userinfo.h"
#include "Json/cJSON.h"
#include "Structs/goodslist.h"
#include "Structs/goodscar.h"
#include "Structs/goodscheckinfo.h"
#include "Structs/dayreportinfo.h"
#include "Widgets/cabinetcheckitem.h"
#include "Widgets/cabinetstorelistitem.h"


class CabinetServer : public QObject
{
    Q_OBJECT
public:
    explicit CabinetServer(QObject *parent = 0);

    bool installGlobalConfig(CabinetConfig *globalConfig);
    void waitForListTimeout();//等待送货车超时

private:
    QNetworkAccessManager* manager;
    CabinetConfig* config;
    QNetworkReply* reply_register;
    QNetworkReply* reply_login;
    QNetworkReply* reply_list_check;
    QNetworkReply* reply_cabinet_bind;
    QNetworkReply* reply_goods_access;
    QNetworkReply* reply_goods_back;
    QNetworkReply* reply_goods_check;
    QNetworkReply* reply_check_tables;
    QNetworkReply* reply_check_table_info;
    QNetworkReply* reply_datetime;
    QNetworkReply* reply_list_state;
    QNetworkReply* reply_cabinet_info;
    QNetworkReply* reply_cabinet_clone;
    QNetworkReply* reply_update_col;
    QNetworkReply* reply_search_spell;
    QNetworkReply* reply_goods_reply;
    QNetworkReply* reply_day_report;
    CheckList* checkList;
    QString regId;
    QString logId;
    QString barCode;
    QString ApiAddress;
    bool timeIsChecked;
    bool needReqCar;
    bool needSaveAddress;
    bool needClearBeforeClone;
    bool networkState;
    bool netFlag;
    QTimer sysClock;
    QTimer watdogClock;
    QStringList list_goodsList;
    UserInfo* cur_user;
    UserInfo* cur_manager;
    int apiState;
    int fWatchdog;
    int checkId;//盘点返回id
    QList<QByteArray> list_access_cache;


    void checkTime();
    void checkSysTime(QDateTime _time);
    void requireListState();//查询是否有送货单在途中
    void replyCheck(QNetworkReply* reply);
    void netTimeStart();
    void localCacheAccess();//提交本地缓存存取
    void accessLoop();
    QString getAbbName(QString fullName);
    void watchdogStart();

signals:
    void loginRst(UserInfo*);
    void listRst(GoodsList*);
    void bindRst(bool);
    void goodsNumChanged(QString goodsId, int goodsNum);
    void updateGoodsPrice(float single, float total);
    void accessFailed(QString msg);
    void accessSuccess(QString msg);
    void timeUpdate();
    void idUpdate();
    void regResult(bool);
    void cloneResult(bool, QString);
    void cabSyncResult(bool);
    void checkCreatRst(bool);
    void goodsCheckRst(QString msg);
    void newGoodsList(QString listCode, QString rfidCode);
    void newGoodsCar(GoodsCar);
    void netState(bool);//true:连接  false:断开
    void sysLock();//锁定系统
    void insertRst(bool success);
    void curCheckList(CheckList* l);
    void curSearchList(CheckList* l);
    void checkFinish(bool success);
    void checkTables(QList<CheckTableInfo*>);
    void goodsReplyRst(bool success, QString msg);
    void dayReportRst(QList<DayReportInfo*>, QString msg);

public slots:
    void cabRegister();
    void getServerAddr(QString addr);
    void userLogin(QString);
    void listCheck(QString);//送货单信息校验
    void cabInfoUpload();//柜子信息上传
    void cabInfoReq();//柜子信息查询
    void cabCloneReq(QString oldCabinetId);//柜子克隆请求
    void cabInfoSync();//柜子数据同步
    void cabColInsert(int pos, int num);
    void cabinetBind(int, int, QString);
    void goodsAccess(CaseAddress, QString, int, int optType);
    void listAccess(QStringList list, int optType);
    void goodsCheckReq();
    void goodsCheckFinish();
    void goodsBack(QString);//退货
    void goodsCheck(QList<CabinetCheckItem*> l, CaseAddress addr);
    void goodsCheck(QStringList l, CaseAddress);
    void goodsListStore(QList<CabinetStoreListItem*> l);
    void goodsCarScan();
    void sysTimeout();
    void updateAddress();
    void requireCheckTables(QDate start, QDate finish);
    void searchSpell(QString);
    void replyRequire(QList<GoodsCheckInfo *> l);
    void requireCheckTableInfo(QString id);
    void requireListInfo(QDate sDate, QDate eDate);
private slots:
    void recvCabRegister();
    void recvUserLogin();
    void recvListCheck();
    void recvCabBind();
    void recvGoodsAccess();
    void recvListAccess();
    void recvGoodsCheck();
    void recvCheckCreat();
    void recvCheckFinish();
    void recvGoodsBack();
    void recvDateTime();
    void recvDateTimeError(QNetworkReply::NetworkError code);
    void recvListState();
    void recvInfoUploadResult();
    void recvCabClone();
    void recvCabSync();
    void recvColInsert();
    void recvCheckTables();
    void recvCheckTableInfo();
    void recvSearchSpell();
    void recvGoodsReply();
    void recvDayReportInfo();
    void netTimeout();
    int watchdogTimeout();
};

#endif // CABINETSERVER_H
