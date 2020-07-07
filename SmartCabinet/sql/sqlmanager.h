#ifndef SQLMANAGER_H
#define SQLMANAGER_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>

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
    static void sqlDelete();

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
public slots:

signals:
    void reqSyncGoods();//同步物品信息请求

private:
    static SqlManager* m;
    explicit SqlManager(QObject *parent = 0);
    static QSqlDatabase db_cabinet;
    bool needSync;

    void initDatabase();
    void createTable();
    void createField(QString tabName, QString fieldName);//创建字段
    static bool queryExec(QSqlQuery *q, QString cmd, QString msg=QString());
};

#endif // SQLMANAGER_H
