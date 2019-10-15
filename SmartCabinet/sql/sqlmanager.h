#ifndef SQLMANAGER_H
#define SQLMANAGER_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>

#include "Structs/goodslist.h"
#include "Structs/cabinetinfo.h"

#define DB_CABINET "Cabinet"

class SqlManager : public QObject
{
    Q_OBJECT
public:
    static SqlManager* manager();
    ~SqlManager();
    QSqlQuery selectAllGoods();
    static QStringList getCaseText(int col, int row);
    static QPoint getGoodsPos(QString packageId);
    static void newApiLog(QString url, QByteArray data, quint64 time_stamp, bool need_resend=true);
    static void apiComplete(quint64 timeStamp);
    bool waitForSync();

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

    void replaceGoodsInfo(GoodsInfo *info, RepState state=no_rep, RepMask stateMask=mask_no);
    void replaceGoodsInfo(Goods* info, RepState state=no_rep, RepMask stateMask=mask_no, QString listCode=QString());
    void bindGoodsPackage(QString packageId, int col, int row);
    static void listStoreAffirm(QString listCode, RepState state);//送货单确认存入

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
    static bool queryExec(QSqlQuery q, QString cmd, QString msg=QString());
};

#endif // SQLMANAGER_H
