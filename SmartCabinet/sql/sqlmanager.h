#ifndef SQLMANAGER_H
#define SQLMANAGER_H

#include <QObject>
#include <QVariantMap>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQueryModel>

#include "Structs/goodslist.h"
#include "Structs/cabinetinfo.h"

#define DB_CABINET "/home/config/Cabinet.sql"

class SqlManager : public QObject
{
    Q_OBJECT

public:
    static SqlManager* manager();
    ~SqlManager();
    QSqlQuery selectAllGoods();
    /*goodsInfo  codesInfo*/
    static QPoint searchByCode(QString code);
    static Goods* searchGoodsByCode(QString code);
    static QList<Goods*> getGoodsList();
    static QList<Goods*> getGoodsList(int col, int row);
    static QString getGoodsId(QString code);//goodsCode->goodsId
    static QStringList getCaseText(int col, int row);
    static int getGoodsCount(QString packageId);
    static QPoint getGoodsPos(QString packageId);
    static void newApiLog(QString url, QByteArray data, quint64 time_stamp, bool need_resend=true);
    static QByteArray apiComplete(quint64 timeStamp);
    static QByteArray apiData(quint64 timeStamp);
    bool waitForSync();
    static void bindGoodsId(int col, int row, QString goodsId);
    static int getShowCountByCase(int col, int row);//获取柜格显示的包类型数量

    /*epcInfo*/
    //EpcInfo:RFID标签表 [epc_code|goods_code|time_stamp|opt_id|state]
    static void insertRfidMark(QString epc, QString goodsCode, QString goodsId=QString());
    static void updateRfid(QString epc, quint32 stamp, QString optId, int state, int row, int col);
    static void begin();
    static QSqlQuery* getPubQuery();
//    static void prepareSingle(QString prepareCmd);
    static void updateRfidsSingle(QString epc, quint32 stamp, QString optId, int state, int row, int col);
    static void querySingle(QString cmd, QString msg=QString());
    static void replace(QString table, QList<QVariantMap> rows);
    static void insert(QString table, QList<QVariantMap> bindings);
    static void commit();
    static QSqlQuery checkRfid(quint32 cutOffStamp, int row=0, int col=0);
    static QSqlQuery checkRfid(QString epcCode);
    static QSqlQuery checkRfid(QStringList epcCodes);
    static QSqlQuery getRfidTable();

    enum RepState{
        no_rep=0,//没有库存
        local_rep=1,//本地库存
        remote_rep=2,//远程库存
        all_rep=3,//本地远程都有库存
    };
    enum RepMask{
        mask_no = 0,
        mask_local = 1,
        mask_remote = 2,
        mask_all = 3,
    };

    static void scanFetch(QString code, RepState state, RepMask stateMask);//
    void replaceGoodsInfo(Goods *info, RepState state=no_rep, RepMask stateMask=mask_no);
    void replaceGoodsInfo(Goods* info, QString listCode, RepState state=no_rep, RepMask stateMask=mask_no);
    static void listStoreAffirm(QString listCode, RepState state);//送货单确认存入
    static QPoint searchByPackageId(QString packageId);
    static QSqlQuery query(QString cmd, QString msg);

    static QSqlDatabase db_cabinet;

public slots:

signals:
    void reqSyncGoods();//同步物品信息请求

private:
    static SqlManager* m;
    explicit SqlManager(QObject *parent = 0);
    static QSqlQuery* pubQuery;
    bool needSync;

    void initDatabase();
    void createTable();
    static bool queryExec(QSqlQuery *q, QString msg=QString(), QString cmd=QString());
};

#endif // SQLMANAGER_H
