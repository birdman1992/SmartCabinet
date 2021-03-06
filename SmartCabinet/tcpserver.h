#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QDateTime>
#include <QProcess>
#include <QString>
#include <QVariant>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "cabinetconfig.h"
#include "Crypto/qaes.h"

#include "cabinetconfig.h"
#include "Structs/userinfo.h"
#include "Json/cJSON.h"
#include "Structs/goodslist.h"
#include "Structs/goodscar.h"
#include "Structs/goodscheckinfo.h"
#include "Widgets/cabinetcheckitem.h"
#include "Widgets/cabinetstorelistitem.h"
#include "manager/cabinetmanager.h"
#include "manager/usermanager.h"
#include "manager/goodsmanager.h"


class tcpServer : public QObject
{
    Q_OBJECT
public:
    explicit tcpServer(QObject *parent = NULL);
    ~tcpServer();
    void setServer(QHostAddress _address, quint16 _port);
    bool installGlobalConfig(CabinetConfig *globalConfig);
    enum TcpReqState{
        noState,
        regState,
        logState,
        cloneState,
        checkTimeState
    };

private:
    CabinetConfig* config;
    CabinetManager* cabManager;
    UserManager* userManager;
    GoodsManager* goodsManager;
    QList<CabinetStoreListItem *> storeList;
    QString regId;
    QString userId;
    QString storeListCode;
    bool needReg;
    bool needSaveAddress;
    bool needClone;
    bool waitTimeRst;
    bool syncFLag;
    bool undoFlag;
    TcpReqState tcpState;
    QStringList accessList;

    int pushTcpReq(QByteArray qba);
    QString getBarCode(QString str);
    //TCP
    QTcpSocket* socket;
    QAes* aesCodec;
    QTimer* beatTimer;
    QHostAddress address;
    quint16 port;
    QString app_id;
    QString app_secret;
    QByteArray tcpCache;

    bool packageIsComplete(QByteArray qba);
    void parCabInfo(cJSON* json);//cabinet info
    void parUserInfo(cJSON* json);//user info
    void parGoodsInfo(cJSON* json);//goods info
    Goods* parGoods(cJSON* json);//Goods
    CheckTableInfo* parCheckTableInfo(cJSON* json);
    void parApp(cJSON* json);//app info
    GoodsCheckInfo *parGoodsCheckInfo(cJSON *json);
    QString getPackageId(QString goodsId, int goodsType);
    GoodsCheckInfo *parGoodsApplyInfo(cJSON *json);
    NUserInfo* parOneUser(cJSON* json);
    UserInfo* nUserToUser(NUserInfo* nInfo);//NUserInfo->UserInfo

//    bool needReg;

    //HTTP
    QString ApiAddress;

    QNetworkAccessManager* manager;
    QNetworkReply* reply_login;
    QNetworkReply* reply_check_store_list;
    QNetworkReply* reply_goods_access;
    QNetworkReply* reply_bind_case;
    QNetworkReply* reply_refund;
    QNetworkReply* reply_check;
    QNetworkReply* reply_apply;
    QNetworkReply* reply_spell;
    CheckList* checkList;

    void getTimeStamp();
    void regist();
    void login();

    QByteArray jLogin(QString id, QString aesId, int jType);//登录(1)和注册(2)  json
    QByteArray jUpdate(QString layout, QString col_map, QPoint scrPos);
    QByteArray jRegist(QString id, QString aesId);
    QByteArray apiJson(QStringList params, QString secret);
    QString apiString(QStringList params, QString secret);
    QString apiSign(QStringList params, QString secret);
    void apiPost(QString uil, QNetworkReply **reply, QByteArray data, QObject *receiver, const char *slot);
    void apiGet(QString uil, QNetworkReply **reply, QString data, QObject *receiver, const char *slot);
    void apiPut(QString uil, QNetworkReply **reply, QByteArray data, QObject *receiver, const char *slot);
    void apiDelete(QString uil, QNetworkReply **reply, QString data, QObject *receiver, const char *slot);
    QStringList paramsBase();
    QString nonceString(int len=16);
    qint64 timeStamp();

    void checkSysTime(QDateTime _time);
    void replyCheck(QNetworkReply *reply);
    void goodsFetch(QString goodsCode);
    void goodsRefund(QString goodsCode);

    void reLogin();
private slots:
    void readData();
    void connectChanged(QAbstractSocket::SocketState);
    void heartBeat();
    void reconnect();

    void recvDateTimeError();
    void recvUserLogin();
    void recvListCheck();
    void recvGoodsAccess();
    void recvGoodsStoreList();
    void recvRebindCase();
    void recvBindCase();
    void recvGoodsRefund();
    void recvCheckCreate();
    void recvCheckFinish();
    void recvCheckHistory();
    void recvCheckTable();
    void recvGoodsCheck();
    void recvApplyRst();

    void tcpReqTimeout();//tcp requst timeout

    void recvSpellReq();
signals:
    void loginRst(UserInfo*);
    void listRst(GoodsList*);
    void bindRst(bool);
    void goodsNumChanged(QString goodsId, int goodsNum);
    void accessFailed(QString msg);
    void timeUpdate();
    void idUpdate();
    void regResult(bool);
    void cloneResult(bool, QString);
    void cabSyncResult(bool);//物品信息同步信号
    void checkCreatRst(bool, QString msg=QString());
    void checkFinishRst(bool, QString msg=QString());
    void goodsCheckRst(QString msg);
    void newGoodsList(QString listCode, QString rfidCode);
    void newGoodsCar(GoodsCar);
    void netState(bool);//true:连接  false:断开
    void sysLock();//锁定系统
    void insertRst(bool success);
    void insertUndoRst(bool success);
    void curCheckList(CheckList* l);
    void checkTables(QList<CheckTableInfo*>);
    void curSearchList(CheckList*);
    void goodsReplyRst(bool,QString);
    void cabPanelChanged();

public slots:
    void cabRegister();
    void getServerAddr(QString addr);
    void userLogin(QString id);
    void listCheck(QString);//送货单信息校验
    void cabInfoUpload();//柜子信息上传
    void cabInfoReq();//柜子信息查询
    void cabCloneReq(QString oldCabinetId);//柜子克隆请求
    void cabInfoSync();//柜子数据同步
    void cabColInsert(int pos, QString layout);
    void cabinetBind(int col, int row, QString goodsId);
    void cabinetRebind(int col, int row, QString goodsId);
    void goodsAccess(CaseAddress, QString, int, int optType);//1取货2存货3退货
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
    void searchSpell(QString spell);
    void replyRequire(QList<GoodsCheckInfo*> replyList);
    void requireCheckTableInfo(QString id);

    void cabInsertUndo();
};

#endif // TCPSERVER_H
