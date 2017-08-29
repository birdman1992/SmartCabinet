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
    QNetworkReply* reply_datetime;
    QNetworkReply* reply_list_state;
    QString regId;
    QString logId;
    QString barCode;
    QString ApiAddress;
    bool timeIsChecked;
    bool needReqCar;
    bool needSaveAddress;
    bool networkState;
    bool netFlag;
    QTimer sysClock;
    QStringList list_goodsList;
    UserInfo* cur_user;
    int apiState;
    QList<QByteArray> list_access_cache;

    void cabRegister();
    void checkTime();
    void checkSysTime(QDateTime _time);
    void requireListState();//查询是否有送货单在途中
    void replyCheck(QNetworkReply* reply);
    void netTimeStart();
    void localCacheAccess();//提交本地缓存存取
    void accessLoop();

signals:
    void loginRst(UserInfo*);
    void listRst(GoodsList*);
    void bindRst(bool);
    void goodsNumChanged(QString goodsId, int goodsNum);
    void accessFailed(QString msg);
    void timeUpdate();
    void idUpdate();
    void goodsCheckRst(QString msg);
    void newGoodsList(QString listCode, QString rfidCode);
    void newGoodsCar(GoodsCar);
    void netState(bool);//true:连接  false:断开
    void sysLock();//锁定系统

public slots:
    void getServerAddr(QString addr);
    void userLogin(QString);
    void listCheck(QString);//送货单信息校验
    void cabInfoUpload();//柜子信息上传
    void cabInfoReq();//柜子信息查询
    void cabCloneReq();//柜子克隆请求
    void cabCloneSync();//柜子克隆数据同步
    void cabinetBind(int, int, QString);
    void goodsAccess(CaseAddress, QString, int, int optType);
    void listAccess(QStringList list, int optType);
    void goodsBack(QString);//退货
    void goodsCheck(QList<CabinetCheckItem*> l, CaseAddress addr);
    void goodsListStore(QList<CabinetStoreListItem*> l);
    void goodsCarScan();
    void sysTimeout();

private slots:
    void recvCabRegister();
    void recvUserLogin();
    void recvListCheck();
    void recvCabBind();
    void recvGoodsAccess();
    void recvListAccess();
    void recvGoodsCheck();
    void recvGoodsBack();
    void recvDateTime();
    void recvDateTimeError(QNetworkReply::NetworkError code);
    void recvListState();
    void netTimeout();
};

#endif // CABINETSERVER_H
