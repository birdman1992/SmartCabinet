#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QDateTime>
#include <QProcess>
#include <QString>
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
        checkTimeState
    };

private:
    CabinetConfig* config;
    QString regId;
    bool needReg;
    bool needSaveAddress;
    bool waitTimeRst;
    TcpReqState tcpState;

    int pushTcpReq(QByteArray qba);
    //TCP
    QTcpSocket* socket;
    QAes* aesCodec;
    QTimer* beatTimer;
    QHostAddress address;
    quint16 port;
//    bool needReg;

    //HTTP
    QString ApiAddress;

    void getTimeStamp();
    void regist();
    void login();

    QByteArray jLogin(QString id, QString aesId, int jType);//登录(1)和注册(2)  json
    QByteArray jRegist(QString id, QString aesId);
    QByteArray apiJson(QStringList params, QString secret);
    QString apiSign(QStringList params, QString secret);
    QString nonceString(int len=16);

    void checkSysTime(QDateTime _time);
private slots:
    void readData();
    void connectChanged(QAbstractSocket::SocketState);
    void heartBeat();
    void reconnect();

    void recvDateTimeError();
    void tcpReqTimeout();//tcp requst timeout
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
    void cabSyncResult(bool);
    void checkCreatRst(bool);
    void goodsCheckRst(QString msg);
    void newGoodsList(QString listCode, QString rfidCode);
    void newGoodsCar(GoodsCar);
    void netState(bool);//true:连接  false:断开
    void sysLock();//锁定系统
    void insertRst(bool success);
    void curCheckList(CheckList* l);
    void checkTables(QList<CheckTableInfo*>);

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

    void requireCheckTableInfo(QString id);
};

#endif // TCPSERVER_H
