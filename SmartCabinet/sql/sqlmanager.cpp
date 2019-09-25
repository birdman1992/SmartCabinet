#include "sqlmanager.h"
#include <QDebug>
#include <QStringList>

SqlManager::SqlManager(QObject *parent) : QObject(parent)
{
    initDatabase();
    createTable();
}

SqlManager::~SqlManager()
{
    if(db_cabinet.isOpen())
        db_cabinet.close();
}

void SqlManager::initDatabase()
{
    if(QSqlDatabase::contains(DB_CABINET))
    {
        db_cabinet = QSqlDatabase::database(DB_CABINET);
    }
    else
    {
        db_cabinet = QSqlDatabase::addDatabase("QSQLITE", DB_CABINET);
        db_cabinet.setDatabaseName(DB_CABINET);
    }
    if(!db_cabinet.open())
    {
        qDebug()<<"[database]"<<DB_CABINET<<"open failed.";
    }
}

/*
CodeInfo:条码物品信息表 [code|goods_id|batch_number|name|abbname|count|size|unit
|pro_name|sup_name|cab_col|cab_row|package_type|single_price|]

*/
void SqlManager::createTable()
{
    QStringList tables = db_cabinet.tables();
    if(tables.indexOf("CodeInfo") == -1)
    {
        QSqlQuery query(db_cabinet);
        QString cmd = QString("create table CodeInfo(\
                              code CHAR(50) PRIMARY NOT NULL,\
                              goods_id CHAR(15) NOT NULL,\
                              batch_number CHAR(50) NOT NULL,\
                              name CHAR(50) NOT NULL,\
                              abbname CHAR(50) DEFAULT('NULL'),\
                              count INT(4) DEFAULT(0),\
                              size CHAR(20) DEFAULT('NULL'),\
                              unit CHAR(10) DEFAULT('NULL'),\
                              pro_name CHAR(50) DEFAULT('NULL'),\
                              sup_name CHAR(50) DEFAULT('NULL'),\
                              cab_col INT(2) DEFAULT(0),\
                              cab_row INT(2) DEFAULT(0),\
                              package_type INT(3) DEFAULT(1),\
                              single_price INT(8) DEFAULT(0),\
                              )");
        qDebug()<<cmd;
        if(query.exec(cmd))
        {
            qDebug()<<"[create table]"<<"CodeInfo"<<"success";
        }
        else
        {
            qDebug()<<"[create table]"<<"CodeInfo"<<"failed";
        }

    }

}
