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
    explicit SqlManager(QObject *parent = 0);
    ~SqlManager();
    void insertGoodsInfo(GoodsInfo* info);
    void insertGoodsInfo(Goods* info);

public slots:

signals:
    void reqSyncGoods();//同步物品信息请求

private:
    QSqlDatabase db_cabinet;

    void initDatabase();
    void createTable();

};

#endif // SQLMANAGER_H
