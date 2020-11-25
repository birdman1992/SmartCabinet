#include "sqlmanager.h"
#include <QDebug>
#include <QStringList>
#include <QTimer>
#include <QVariant>
#include <QtDebug>

QSqlDatabase SqlManager::db_cabinet = QSqlDatabase();
SqlManager* SqlManager::m = new SqlManager;
QSqlQuery* SqlManager::pubQuery = NULL;
int queryNum = 0;

SqlManager::SqlManager(QObject *parent) : QObject(parent)
{
    needSync = false;
    initDatabase();
    createTable();
    createField("GoodsInfo", "pinyin");
    createField("CodeInfo", "operation_list", 20);
}

SqlManager *SqlManager::manager()
{
    return m;
}

SqlManager::~SqlManager()
{
    if(db_cabinet.isOpen())
        db_cabinet.close();
}

QSqlQuery SqlManager::selectAllGoods()
{
    QSqlQuery query(db_cabinet);
    query.exec("select GoodsInfo.abbname,COUNT(*) package_count,GoodsInfo.cab_col,GoodsInfo.cab_row FROM GoodsInfo LEFT JOIN CodeInfo ON CodeInfo.package_id=GoodsInfo.package_id GROUP BY GoodsInfo.package_id ORDER BY GoodsInfo.cab_col,GoodsInfo.cab_row;");
    QPoint last_pos = QPoint(-1,-1);
    QPoint cur_pos;
    QStringList last_show_list;
    last_show_list.clear();

    while(query.next())
    {
        QString abbName = query.value(0).toString();
        int packageCount = query.value(1).toInt();
        cur_pos.setX(query.value(2).toInt());
        cur_pos.setY(query.value(3).toInt());
        QString showStr = QString("%1x%2").arg(abbName).arg(packageCount);
        if(cur_pos == last_pos)
        {
            last_show_list<<showStr;
        }
        else
        {
            last_pos = QPoint(-1,-1);
            if(!last_show_list.isEmpty())
            {
                qDebug()<<last_show_list;
                last_show_list.clear();
                last_show_list<<showStr;
            }
        }
    }
    return query;
}

void SqlManager::scanFetch(QString code, RepState state, RepMask stateMask)
{
    QSqlQuery query(db_cabinet);
    //更新库存状态
    if(stateMask & 0x01)
    {
        QString cmd = QString("UPDATE CodeInfo SET state_local=%1 where code='%2'").arg(state&0x1).arg(code);
        if(!query.exec(cmd))
        {
            qDebug()<<query.lastQuery()<<query.lastError().text();
//            continue;
        }
    }
    if(stateMask & 0x02)
    {
        QString cmd = QString("UPDATE CodeInfo SET state_remote=%1 where code='%2'").arg((state&0x2)>>1).arg(code);
        query.bindValue(0, QVariant((state&0x2)>>1));
        if(!query.exec(cmd))
        {
            qDebug()<<query.lastQuery()<<query.lastError().text();
//            continue;
        }
    }
}

QPoint SqlManager::searchByPackageId(QString packageId)
{
    QSqlQuery query(db_cabinet);
    QString cmd = QString("select cab_col,cab_row FROM GoodsInfo WHERE package_id='%1';").arg(packageId);
    if(!query.exec(cmd))
    {
        qDebug()<<query.lastQuery()<<query.lastError().text();
        return QPoint(-1,-1);
    }

    int row=-1, col=-1;
    if(query.next())
    {
        col = query.value(0).toInt();
        row = query.value(1).toInt();
    }
    return QPoint(col, row);
}

QPoint SqlManager::searchByCode(QString code)
{
    QSqlQuery query(db_cabinet);
    QString cmd = QString("select GoodsInfo.cab_col,GoodsInfo.cab_row FROM GoodsInfo LEFT JOIN CodeInfo ON CodeInfo.package_id=GoodsInfo.package_id WHERE codeInfo.code='%1';").arg(code);
    if(!queryExec(&query, "searchByCode", cmd))
        return QPoint(-1,-1);

    int row=-1, col=-1;
    if(query.next())
    {
        col = query.value(0).toInt();
        row = query.value(1).toInt();
    }
    return QPoint(col, row);
}

//CodeInfo:条码信息表 [code|package_id|batch_number|pro_name|sup_name|state_local|state_remote|store_list]
//GoodsInfo:物品信息表 [package_id|goods_id|package_type|name|abbname|size|unit|cab_col|cab_row|single_price]
Goods *SqlManager::searchGoodsByCode(QString code)
{
    Goods* info = NULL;
    QSqlQuery query(db_cabinet);
    QString cmd = QString("select GoodsInfo.package_id,GoodsInfo.goods_id,GoodsInfo.package_type,GoodsInfo.name,GoodsInfo.abbname,GoodsInfo.size,GoodsInfo.unit,GoodsInfo.cab_col,GoodsInfo.cab_row,GoodsInfo.single_price,CodeInfo.code,CodeInfo.pro_name,CodeInfo.sup_name,CodeInfo.store_list FROM GoodsInfo LEFT JOIN CodeInfo ON CodeInfo.package_id=GoodsInfo.package_id WHERE codeInfo.code='%1';").arg(code);
    if(!queryExec(&query, "searchGoodsByCode", cmd))
        return info;

    if(!query.next())
        return NULL;

    info = new Goods;
    info->packageId = query.value(0).toString();
    info->goodsId = query.value(1).toString();
    info->packageType = query.value(2).toInt();
    info->name = query.value(3).toString();
    info->abbName = query.value(4).toString();
    info->size = query.value(5).toString();
    info->unit = query.value(6).toString();
    info->pos.setX(query.value(7).toInt());
    info->pos.setY(query.value(8).toInt());
    info->singlePrice = query.value(9).toInt();
    info->traceId = query.value(10).toString();
    info->proName = query.value(11).toString();
    info->supName = query.value(12).toString();
    info->listCode = query.value(13).toString();
    return info;
}

QList<Goods *> SqlManager::getGoodsList()
{
    Goods* info = NULL;
    QList<Goods*> ret;
    ret.clear();
    QSqlQuery query(db_cabinet);
    QString cmd = QString("select GoodsInfo.package_id,GoodsInfo.goods_id,GoodsInfo.package_type,GoodsInfo.name,GoodsInfo.abbname,GoodsInfo.size,GoodsInfo.unit,GoodsInfo.cab_col,GoodsInfo.cab_row,GoodsInfo.single_price,CodeInfo.code,CodeInfo.pro_name,CodeInfo.sup_name,CodeInfo.store_list FROM GoodsInfo LEFT JOIN CodeInfo ON CodeInfo.package_id=GoodsInfo.package_id;");
    if(!queryExec(&query, "getGoodsList", cmd))
        return ret;

    while(query.next())
    {
        info = new Goods;
        info->packageId = query.value(0).toString();
        info->goodsId = query.value(1).toString();
        info->packageType = query.value(2).toInt();
        info->name = query.value(3).toString();
        info->abbName = query.value(4).toString();
        info->size = query.value(5).toString();
        info->unit = query.value(6).toString();
        info->pos.setX(query.value(7).toInt());
        info->pos.setY(query.value(8).toInt());
        info->singlePrice = query.value(9).toInt();
        info->traceId = query.value(10).toString();
        info->proName = query.value(11).toString();
        info->supName = query.value(12).toString();
        info->listCode = query.value(13).toString();
        ret<<info;
    }
    return ret;
}

QList<Goods *> SqlManager::getGoodsList(int col, int row)
{
    Goods* info = NULL;
    QList<Goods*> ret;
    ret.clear();
    QSqlQuery query(db_cabinet);
    QString cmd = QString("select GoodsInfo.package_id,GoodsInfo.goods_id,GoodsInfo.package_type,GoodsInfo.name,GoodsInfo.abbname,GoodsInfo.size,GoodsInfo.unit,GoodsInfo.cab_col,GoodsInfo.cab_row,GoodsInfo.single_price,COUNT(code) package_count,CodeInfo.pro_name,CodeInfo.sup_name,CodeInfo.store_list FROM GoodsInfo LEFT JOIN CodeInfo ON CodeInfo.package_id=GoodsInfo.package_id WHERE CodeInfo.state_remote>0 AND GoodsInfo.cab_col=%1 AND GoodsInfo.cab_row=%2 group by GoodsInfo.package_id;").arg(col).arg(row);
    if(!queryExec(&query, "getGoodsList", cmd))
        return ret;

    while(query.next())
    {
        info = new Goods;
        info->packageId = query.value(0).toString();
        info->goodsId = query.value(1).toString();
        info->packageType = query.value(2).toInt();
        info->name = query.value(3).toString();
        info->abbName = query.value(4).toString();
        info->size = query.value(5).toString();
        info->unit = query.value(6).toString();
        info->pos.setX(query.value(7).toInt());
        info->pos.setY(query.value(8).toInt());
        info->singlePrice = query.value(9).toInt();
        info->num = query.value(10).toInt();
        info->proName = query.value(11).toString();
        info->supName = query.value(12).toString();
        info->listCode = query.value(13).toString();
        ret<<info;
    }
    return ret;
}

QString SqlManager::getPackageId(QString code)
{
    QString ret = QString();
    QString cmd = QString("select package_id FROM codeInfo WHERE code='%1'").arg(code);
    QSqlQuery query(db_cabinet);
    if(!queryExec(&query, "getPackageId", cmd))
    {
        return QString();
    }
    qDebug()<<query.isActive()<<query.isSelect();

    if(query.next())
        ret = query.value(0).toString();
    else
        qDebug("getPackageId failed");

    return ret;
}

QString SqlManager::getEpcCode(QString code)
{
    QString ret = QString();
    QString cmd = QString("SELECT EI.epc_code FROM CodeInfo AS CI LEFT JOIN EpcInfo AS EI ON EI.goods_code=CI.code WHERE CI.code='%1'").arg(code);
    QSqlQuery query(db_cabinet);
    if(!queryExec(&query, "getPackageId", cmd))
    {
        return QString();
    }
    qDebug()<<query.isActive()<<query.isSelect();

    if(query.next())
        ret = query.value(0).toString();
    else
        qDebug("getPackageId failed");

    return ret;
}

QStringList SqlManager::getCaseText(int col, int row)
{
    QSqlQuery query(db_cabinet);
    QString cmd = QString("SELECT "
                          "GoodsInfo.abbname,"
                          "GoodsInfo.cab_col,"
                          "GoodsInfo.cab_row,"
                          "GoodsInfo.size,"
                          "IFNULL(CodeInfo.package_count,0) package_count "
                      "FROM "
                          "GoodsInfo "
                          "LEFT JOIN(SELECT COUNT( * ) package_count, package_id FROM CodeInfo WHERE CodeInfo.state_remote = 1 GROUP BY package_id) "
                          "CodeInfo ON CodeInfo.package_id = GoodsInfo.package_id "
                      "WHERE "
                          "GoodsInfo.cab_col = %1 "
                          "AND GoodsInfo.cab_row = %2")
            .arg(col).arg(row);

    if(!queryExec(&query, "getCaseText", cmd))
        return QStringList();

    QStringList last_show_list;
    last_show_list.clear();

    while(query.next())
    {
        QString abbName = query.value(0).toString();
        QString size = query.value(3).toString();
        int packageCount = query.value(4).toInt();
        QString showStr = QString("%1[%3]x%2").arg(abbName).arg(packageCount).arg(size);
        last_show_list<<showStr;
    }
    return last_show_list;
}

int SqlManager::getGoodsCount(QString packageId)
{
    QSqlQuery query(db_cabinet);
    QString cmd = QString("select COUNT(*) package_count FROM CodeInfo WHERE package_id='%1' AND state_local=1;").arg(packageId);
    if(!queryExec(&query, "getGoodsCount", cmd))
        return 0;

    if(query.next())
        return query.value(0).toInt();
    else
        return 0;
}

QPoint SqlManager::getGoodsPos(QString packageId)
{
    QString cmd = QString("select cab_col,cab_row from GoodsInfo WHERE package_id='%1';").arg(packageId);
    QSqlQuery query(db_cabinet);
    if(!queryExec(&query, "getGoodsPos", cmd))
        return QPoint(-1,-1);

    QPoint ret = QPoint(-1, -1);
    if(query.next())
    {
        ret.setX(query.value(0).toInt());
        ret.setY(query.value(1).toInt());
    }
    return ret;
}

//ApiLog:接口日志表 [time_stamp|url|data]
void SqlManager::newApiLog(QString url, QByteArray data, quint64 time_stamp, bool need_resend)
{
    QString cmd;
    if(!need_resend)
        cmd = QString("INSERT INTO ApiLog(time_stamp,url,data,need_resend) VALUES(%1,'%2','%3',%4);").arg(time_stamp).arg(url).arg(QString(data)).arg(0);
    else
        cmd = QString("INSERT INTO ApiLog(time_stamp,url,data,need_resend) VALUES(%1,'%2','%3',%4);").arg(time_stamp).arg(url).arg(QString(data)).arg(1);
    QSqlQuery query(db_cabinet);
    queryExec(&query, "newApiLog", cmd);
}

QByteArray SqlManager::apiComplete(quint64 timeStamp)
{
    QString cmd = QString("UPDATE ApiLog SET state='complete' where time_stamp=%1").arg(timeStamp);
    QSqlQuery query(db_cabinet);
    queryExec(&query, "apiComplete", cmd);
    return apiData(timeStamp);
}

QByteArray SqlManager::apiData(quint64 timeStamp)
{
    QString cmd = QString("SELECT data FROM ApiLog where time_stamp=%1").arg(timeStamp);
    QSqlQuery query(db_cabinet);
    queryExec(&query, "apiComplete", cmd);
    if(query.next())
        return query.value(0).toByteArray();
    return QByteArray();
}

bool SqlManager::waitForSync()
{
    return needSync;
}

void SqlManager::bindGoodsId(int col, int row, QString goodsId)
{
    QString cmd = QString("UPDATE GoodsInfo SET cab_col=%1,cab_row=%2 WHERE package_id='%3'").arg(col).arg(row).arg(goodsId);
    QSqlQuery query(db_cabinet);
    queryExec(&query, "bindGoodsId", cmd);
}

int SqlManager::getShowCountByCase(int col, int row)
{
    int ret = 0;
    QString cmd = QString("select COUNT(*) show_count FROM GoodsInfo WHERE cab_col=%1 AND cab_row=%2 ").arg(col).arg(row);
    QSqlQuery query(db_cabinet);
    queryExec(&query, "getShowCountByCase", cmd);

    if(query.next())
        ret = query.value(0).toInt();
    return ret;
}

//EpcInfo:RFID标签表 [epc_code|goods_code|goods_id|time_stamp|opt_id|state|row|col]
void SqlManager::insertRfidMark(QString epc, QString goodsCode, QString goodsId)
{
    QSqlQuery query(db_cabinet);
    query.prepare("INSERT INTO EpcInfo(epc_code, goods_code, goods_id) VALUES(:epc_code, :goods_code, :goods_id)");
    query.bindValue(0, epc);
    query.bindValue(1, goodsCode);
    query.bindValue(2, goodsId);
    queryExec(&query, "insertRfidMark");
    qDebug()<<query.lastQuery();
}

void SqlManager::updateRfid(QString epc, quint32 stamp, QString optId, int state, int row, int col)
{
    QString cmd = QString("UPDATE EpcInfo SET time_stamp=%1, opt_id='%2', state=%3, row=%4, col=%5 WHERE epc_code='%6'").arg(stamp).arg(optId).arg(state).arg(row).arg(col).arg(epc);
    QSqlQuery query(db_cabinet);
    queryExec(&query, "updateRfid", cmd);
}

void SqlManager::begin()
{
    if(pubQuery == NULL)//
    {
        pubQuery = new QSqlQuery(db_cabinet);
        pubQuery->exec("BEGIN;");
    }
    queryNum++;//同时执行的事务数
}

QSqlQuery *SqlManager::getPubQuery()
{
    return pubQuery;
}

//void SqlManager::prepareSingle(QString prepareCmd)
//{
//    pubQuery->prepare(prepareCmd);
//}

void SqlManager::updateRfidsSingle(QString epc, quint32 stamp, QString optId, int state, int row, int col)
{
    QString cmd = QString("UPDATE EpcInfo SET time_stamp=%1, opt_id='%2', state=%3, row=%4, col=%5 WHERE epc_code='%6'").arg(stamp).arg(optId).arg(state).arg(row).arg(col).arg(epc);
    queryExec(pubQuery, "updateRfid", cmd);
}

void SqlManager::querySingle(QString cmd, QString msg)
{
    queryExec(pubQuery, msg, cmd);
}

void SqlManager::replace(QString table, QList<QVariantMap> bindings)
{
    if(bindings.isEmpty())
        return;

    foreach (QVariantMap binding, bindings)
    {
//        qDebug()<<"prepare:"<<QStringList(binding.keys()).join(",")<<QStringList(binding.keys()).join(",:");
        QString preStr = QString("REPLACE INTO %1 (%2) VALUES(:%3)")
                  .arg(table)
                  .arg(QStringList(binding.keys()).join(","))
                  .arg(QStringList(binding.keys()).join(",:"));

        pubQuery->prepare(preStr);

        foreach (QString key, binding.keys())
        {
//            qDebug()<<key<<binding[key];
            pubQuery->bindValue(QString(":%1").arg(key), binding[key]);
        }
        queryExec(pubQuery, QString("[replace] %1").arg(table));
    }
}

void SqlManager::insert(QString table, QList<QVariantMap> bindings)
{
    foreach (QVariantMap binding, bindings)
    {
        pubQuery->prepare(QString("INSERT INTO %1 (%2) VALUES(:%3)")
                          .arg(table)
                          .arg(QStringList(binding.keys()).join(","))
                          .arg(QStringList(binding.keys()).join(",:")));

        foreach (QString key, binding.keys())
        {
//            qDebug()<<key<<binding[key];
            pubQuery->bindValue(QString(":%1").arg(key), binding.value(key));
        }
        queryExec(pubQuery, QString("[insert] %1").arg(table));
    }
}

void SqlManager::commit()
{
    queryNum--;
    if(queryNum == 0)//没有别的正在执行的事务
    {
        pubQuery->exec("COMMIT;");
        delete pubQuery;
        pubQuery = NULL;
    }
}

QSqlQuery SqlManager::query(QString cmd, QString msg)
{
    QSqlQuery query(db_cabinet);
    queryExec(&query, msg, cmd);
    return query;
}

//GoodsInfo:物品信息表 [package_id|goods_id|package_type|name|abbname|size|unit|cab_col|cab_row|single_price]
//EpcInfo:RFID标签表 [epc_code|goods_code|goods_id|time_stamp|opt_id|state|row|col]
//[物品|条码|RFID|规格|操作人|时间]
//获取取出物品列表
QSqlQuery SqlManager::checkRfid(quint32 cutOffStamp, int row, int col)
{
    QString cmd = QString("SELECT GI.name, EI.epc_code, GI.size, GI.pro_name, GI.sup_name, EI.opt_id, EI.time_stamp FROM EpcInfo AS EI LEFT JOIN GoodsInfo AS GI ON EI.goods_id=GI.goods_id WHERE EI.row=%1 AND EI.col=%2 AND EI.time_stamp<%3").arg(row).arg(col).arg(cutOffStamp);
    QSqlQuery query(db_cabinet);
    queryExec(&query, "checkRfid", cmd);
    return query;
}

//根据EPC获取物品信息
QSqlQuery SqlManager::checkRfid(QString epcCode)
{
    QString cmd = QString("SELECT GI.name, EI.epc_code, GI.size, GI.pro_name, GI.sup_name, EI.opt_id, EI.time_stamp FROM EpcInfo AS EI LEFT JOIN GoodsInfo AS GI ON EI.goods_id=GI.goods_id WHERE EI.epc_code IN %1").arg(epcCode);
    QSqlQuery query(db_cabinet);
    queryExec(&query, "checkRfid", cmd);
    return query;
}

//根据EPC获取物品信息
QSqlQuery SqlManager::checkRfid(QStringList epcCodes)
{
    QString epcCode = epcCodes.join("','");
    QString cmd = QString("SELECT GI.name, EI.epc_code, EI.goods_code, GI.size, GI.pro_name, GI.sup_name, EI.opt_id, EI.time_stamp FROM EpcInfo AS EI LEFT JOIN GoodsInfo AS GI ON EI.goods_id=GI.goods_id WHERE EI.epc_code IN ('%1')").arg(epcCode);
    QSqlQuery query(db_cabinet);
    queryExec(&query, "checkRfid", cmd);
    return query;
}

//[epc_code|goods_code|time_stamp|state]
QSqlQuery SqlManager::getRfidTable()
{
    QString cmd = QString("SELECT epc_code, goods_code, time_stamp, state from EpcInfo");
    QSqlQuery query(db_cabinet);
    queryExec(&query, "getRfidTable", cmd);
    return query;
}

void SqlManager::sqlDelete()
{
    QString cmd = QString("DELETE FROM CodeInfo");
    QSqlQuery query(db_cabinet);
    queryExec(&query, "sqlDelete", cmd);

    cmd = QString("DELETE FROM GoodsInfo");
    queryExec(&query, "sqlDelete", cmd);

    cmd = QString("DELETE FROM EpcInfo");
    queryExec(&query, "sqlDelete", cmd);

    cmd = QString("DELETE FROM OperationInfo");
    queryExec(&query, "sqlDelete", cmd);
//    QString cmd = QString("DELETE FROM CodeInfo").arg(col).arg(row).arg(goodsId);
//    QSqlQuery query(db_cabinet);
//    queryExec(&query, cmd, "sqlDelete");
//    QString cmd = QString("DELETE FROM CodeInfo").arg(col).arg(row).arg(goodsId);
//    QSqlQuery query(db_cabinet);
    //    queryExec(&query, cmd, "sqlDelete");
}

QList<QPoint> SqlManager::goodsSearch(QString searchStr)
{
    QList<QPoint> searchRst;
    QString cmd = QString("SELECT cab_row,cab_col FROM GoodsInfo WHERE pinyin LIKE '%%1%'").arg(searchStr);
    QSqlQuery query(db_cabinet);
    queryExec(&query, "goodsSearch", cmd);

    while (query.next()) {
        QPoint pos = QPoint(query.value(1).toInt(),query.value(0).toInt());
        if(pos.x()<0 || pos.y()<0)
            continue;
        searchRst<<pos;
    }
    return searchRst;
}

QSqlQuery SqlManager::goodsInfoList(QString searchStr)
{
    QString cmd = QString("SELECT "
                              "GI.name || ' [' || IFNULL( GI.size, '  ' ) || '] (' || GI.package_type || ')' || '×' || COUNT( code ) AS InfoStr, "
                              "GI.package_id "
                          "FROM "
                              "GoodsInfo AS GI "
                              "LEFT JOIN CodeInfo AS CI ON CI.package_id = GI.package_id "
                          "WHERE "
                              "GI.pinyin LIKE '%%1%' "
                          "GROUP BY "
                              "GI.package_id").arg(searchStr);
    QSqlQuery query(db_cabinet);
    queryExec(&query, "goodsInfoList", cmd);
    return query;
}

/*
CodeInfo:条码信息表 [code|package_id|batch_number|pro_name|sup_name|state_local|state_remote|store_list]
GoodsInfo:物品信息表 [package_id|goods_id|package_type|name|abbname|size|unit|cab_col|cab_row|single_price]

stateMask:bit0: =1 update localstate, =0 not update
          bit1: =1 update remote, =0 not update
*/
void SqlManager::replaceGoodsInfo(Goods *info, RepState state, RepMask stateMask)
{
    if(info->goodsId.isEmpty())
        return;

    QSqlQuery query(db_cabinet);
    query.exec("BEGIN;");
    qDebug()<<"[CommitGoodsInfo]:begin";

    //添加物品信息
    query.prepare("REPLACE INTO GoodsInfo(package_id,goods_id,package_type,name,abbname,size,unit,cab_col,cab_row,single_price,pinyin)\
                    VALUES(:package_id,:goods_id,:package_type,:name,:abbname,:size,:unit,:cab_col,:cab_row,:single_price,:pinyin)");
    query.bindValue(0, QVariant(info->packageId));
    query.bindValue(1, QVariant(info->goodsId));
    query.bindValue(2, QVariant(info->packageType));
    query.bindValue(3, QVariant(info->name));
    query.bindValue(4, QVariant(info->abbName));
    query.bindValue(5, QVariant(info->size));
    query.bindValue(6, QVariant(info->unit));
    query.bindValue(7, QVariant(info->col));
    query.bindValue(8, QVariant(info->row));
    query.bindValue(9, QVariant(info->price));
    query.bindValue(10, QVariant(info->Py));
    if(!query.exec())
    {
        qDebug()<<query.lastQuery()<<query.lastError().text();
    }
    else
    {
        qDebug()<<"[replaceGoodsInfo]:"<<info->packageId;
    }

    //添加条码信息 CodeInfo:条码信息表 [code|package_id|batch_number|pro_name|sup_name|state_local|state_remote]
    foreach (QString traceId, info->codes)
    {
        query.prepare("REPLACE INTO CodeInfo(code,package_id)\
                        VALUES(:code,:package_id)");
        query.bindValue(0, QVariant(traceId));
        query.bindValue(1, QVariant(info->packageId));
//        query.prepare("REPLACE INTO CodeInfo(code,package_id,pro_name,sup_name,store_list)\
//                        VALUES(:code,:package_id,:pro_name,:sup_name,:store_list)");
//        query.bindValue(0, QVariant(traceId));
//        query.bindValue(1, QVariant(info->packageId));
//        query.bindValue(2, QVariant(info->proName));
//        query.bindValue(3, QVariant(info->supName));
//        query.bindValue(4, QVariant(listCode));

        if(!query.exec())
        {
            qDebug()<<query.lastQuery()<<query.lastError().text();
            continue;
        }
        else
        {
//            qDebug()<<"[replaceCodeInfo]:"<<traceId;
        }

        //更新库存状态
        if(stateMask & 0x01)
        {
            query.prepare("UPDATE CodeInfo SET state_local=:state_local");
            query.bindValue(0, QVariant(state&0x1));
            if(!query.exec())
            {
                qDebug()<<query.lastQuery()<<query.lastError().text();
                continue;
            }
        }
        if(stateMask & 0x02)
        {
            query.prepare("UPDATE CodeInfo SET state_remote=:state_remote");
            query.bindValue(0, QVariant((state&0x2)>>1));
            if(!query.exec())
            {
                qDebug()<<query.lastQuery()<<query.lastError().text();
                continue;
            }
        }
    }
    if(!query.exec("COMMIT;"))
    {
        qDebug()<<"[commit failed]"<<query.lastError().text();
    }
    else
    {
        qDebug()<<"[CommitGoodsInfo]:end";
    }
}

/*
CodeInfo:条码信息表 [code|package_id|batch_number|pro_name|sup_name|state_local|state_remote|store_list]
GoodsInfo:物品信息表 [package_id|goods_id|package_type|name|abbname|size|unit|cab_col|cab_row|single_price]

stateMask:bit0: =1 update localstate, =0 not update
          bit1: =1 update remote, =0 not update
*/
void SqlManager::replaceGoodsInfo(Goods* info, QString listCode, RepState state, RepMask stateMask)
{
    if(info->codes.isEmpty())
        return;

    QSqlQuery query(db_cabinet);
    query.exec("BEGIN;");
    qDebug()<<"[CommitGoodsInfo]:begin";

    //添加物品信息
    query.prepare("INSERT INTO GoodsInfo(package_id,goods_id,package_type,name,abbname,size,unit,cab_col,cab_row,single_price,pinyin)\
                    VALUES(:package_id,:goods_id,:package_type,:name,:abbname,:size,:unit,:cab_col,:cab_row,:single_price,:pinyin)");
    query.bindValue(0, QVariant(info->packageId));
    query.bindValue(1, QVariant(info->goodsId));
    query.bindValue(2, QVariant(info->packageType));
    query.bindValue(3, QVariant(info->name));
    query.bindValue(4, QVariant(info->abbName));
    query.bindValue(5, QVariant(info->size));
    query.bindValue(6, QVariant(info->unit));
    query.bindValue(7, QVariant(info->pos.x()));
    query.bindValue(8, QVariant(info->pos.y()));
    query.bindValue(9, QVariant(info->singlePrice));
    query.bindValue(10, QVariant(info->Py));
    if(!query.exec())
    {
        qDebug()<<query.lastQuery()<<query.lastError().text()<<" 已存在物品信息，不插入";
    }
    else
    {
        qDebug()<<"[insertGoodsInfo]:"<<info->packageId;
    }

    //添加条码信息 CodeInfo:条码信息表 [code|package_id|batch_number|pro_name|sup_name|state_local|state_remote|store_list]
    foreach (QString traceId, info->codes)
    {
        query.prepare("REPLACE INTO CodeInfo(code,package_id,pro_name,sup_name,store_list)\
                        VALUES(:code,:package_id,:pro_name,:sup_name,:store_list)");
        query.bindValue(0, QVariant(traceId));
        query.bindValue(1, QVariant(info->packageId));
        query.bindValue(2, QVariant(info->proName));
        query.bindValue(3, QVariant(info->supName));
        query.bindValue(4, QVariant(listCode));

        if(!query.exec())
        {
            qDebug()<<query.lastQuery()<<query.lastError().text();
            continue;
        }
        else
        {
            qDebug()<<"[replaceCodeInfo]:"<<traceId;
        }

        //更新库存状态
        if(stateMask & 0x01)
        {
            query.prepare("UPDATE CodeInfo SET state_local=:state_local");
            query.bindValue(0, QVariant(state&0x1));
            if(!query.exec())
            {
                qDebug()<<query.lastQuery()<<query.lastError().text();
                continue;
            }
        }
        if(stateMask & 0x02)
        {
            query.prepare("UPDATE CodeInfo SET state_remote=:state_remote");
            query.bindValue(0, QVariant((state&0x2)>>1));
            if(!query.exec())
            {
                qDebug()<<query.lastQuery()<<query.lastError().text();
                continue;
            }
        }
    }
    if(!query.exec("COMMIT;"))
    {
        qDebug()<<"[commit failed]"<<query.lastError().text();
    }
    else
    {
        qDebug()<<"[CommitGoodsInfo]:end";
    }
}

//确认物品存入状态
void SqlManager::listStoreAffirm(QString listCode, RepState state, QStringList rejectList)
{
    QSqlQuery query(db_cabinet);
    if(state & local_rep)//本地存入
    {
        QString rejectCode = rejectList.join("','");
        QString cmd = QString("UPDATE CodeInfo SET state_local=1 WHERE store_list='%1' AND code NOT IN ('%2');").arg(listCode).arg(rejectCode);
//        qDebug()<<"CMD"<<cmd;
        if(!queryExec(&query, "listStoreAffirm", cmd))
        {
            qDebug()<<"[listStoreAffirm failed]"<<query.lastError().text();
        }
    }
    if(state & remote_rep)//远程存入
    {
        QString rejectCode = rejectList.join("','");
        QString cmd = QString("UPDATE CodeInfo SET state_remote=1 WHERE store_list='%1' AND code NOT IN ('%2');").arg(listCode).arg(rejectCode);
        if(!queryExec(&query, "listStoreAffirm", cmd))
        {
            qDebug()<<"[listStoreAffirm failed]"<<query.lastError().text();
        }
    }
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
        qDebug()<<"[database]"<<DB_CABINET<<"open failed."<<db_cabinet.lastError().text();
    }
}

/*
CodeInfo:条码信息表 [code|package_id|batch_number|pro_name|sup_name|state_local|state_remote|store_list|surgery_bill_no]
GoodsInfo:物品信息表 [package_id|goods_id|package_type|name|abbname|size|unit|cab_col|cab_row|single_price]
EpcInfo:RFID标签表 [epc_code|goods_code|goods_id|time_stamp|opt_id|state|row|col]
ApiLog:接口日志表 [time_stamp|url|data|need_resend]
*/
void SqlManager::createTable()
{
    QStringList tables = db_cabinet.tables();
    if(tables.indexOf("CodeInfo") == -1)
    {
        QSqlQuery query(db_cabinet);
        QString cmd = QString("create table CodeInfo("
                              "code CHAR(50) PRIMARY KEY NOT NULL,"
                              "package_id CHAR(15) NOT NULL,"
                              "batch_number CHAR(50) DEFAULT('NULL'),"
                              "pro_name CHAR(50) DEFAULT('NULL'),"
                              "sup_name CHAR(50) DEFAULT('NULL'),"
                              "state_local INT(3) DEFAULT(1),"
                              "state_remote INT(3) DEFAULT(1),"
                              "store_list CHAR(20) DEFAULT('NULL'),"
                              "operation_list CHAR(20) DEFAULT(''),"
                              "surgery_bill_no CHAR(20) DEFAULT('NULL'),"
                              "check_time_stamp INT(15) DEFAULT(0)"
                              ");");
//        qDebug()<<cmd;
        if(query.exec(cmd))
        {
            qDebug()<<"[create table]"<<"CodeInfo"<<"success";
            needSync = true;
        }
        else
        {
            qDebug()<<"[create table]"<<"CodeInfo"<<"failed"<<query.lastError();
        }

    }
    if(tables.indexOf("GoodsInfo") == -1)
    {
        QSqlQuery query(db_cabinet);
        QString cmd = QString("create table GoodsInfo(\
                              package_id CHAR(15) PRIMARY KEY NOT NULL,\
                              goods_id CHAR(15) NOT NULL,\
                              package_type INT(3) DEFAULT(1),\
                              name CHAR(50) NOT NULL,\
                              abbname CHAR(50) DEFAULT('NULL'),\
                              size CHAR(20) DEFAULT('NULL'),\
                              unit CHAR(10) DEFAULT('NULL'),\
                              cab_col INT(2) DEFAULT(-1),\
                              cab_row INT(2) DEFAULT(-1),\
                              single_price REAL(18) DEFAULT(0),\
                              pro_name CHAR(50) DEFAULT(''),\
                              sup_name CHAR(50) DEFAULT(''),\
                              pinyin CHAR(50) DEFAULT('')\
                              );");
        if(query.exec(cmd))
        {
            qDebug()<<"[create table]"<<"GoodsInfo"<<"success";
            needSync = true;
        }
        else
        {
            qDebug()<<"[create table]"<<"GoodsInfo"<<"failed"<<query.lastError();
        }
    }

    if(tables.indexOf("EpcInfo") == -1)
    {
        QSqlQuery query(db_cabinet);
        QString cmd = QString("create table EpcInfo(\
                              epc_code CHAR(15) PRIMARY KEY NOT NULL,\
                              goods_code CHAR(15) NOT NULL,\
                              time_stamp INT(10) DEFAULT(0),\
                              opt_id CHAR(15) DEFAULT('NULL'),\
                              state INT(2) DEFAULT(0),\
                              row INT(2) DEFAULT(0),\
                              col INT(2) DEFAULT(0)\
                              );");
        if(query.exec(cmd))
        {
            qDebug()<<"[create table]"<<"EpcInfo"<<"success";
            needSync = true;
        }
        else
        {
            qDebug()<<"[create table]"<<"EpcInfo"<<"failed"<<query.lastError();
        }
    }

    //ApiLog:接口日志表 [time_stamp|url|data|state|need_resend]
    if(tables.indexOf("ApiLog") == -1)
    {
        QSqlQuery query(db_cabinet);
        QString cmd = QString("create table ApiLog(\
                              time_stamp INT(15) PRIMARY KEY NOT NULL,\
                              url CHAR(150) NOT NULL,\
                              data CHAR(2048) DEFAULT('NULL'),\
                              state CHAR(10) DEFAULT('wait'),\
                              need_resend INT(1) DEFAULT(1)\
                              );");
        if(query.exec(cmd))
        {
            qDebug()<<"[create table]"<<"ApiLog"<<"success";
            needSync = true;
        }
        else
        {
            qDebug()<<"[create table]"<<"ApiLog"<<"failed"<<query.lastError();
        }
    }

    //OperationInfo:手术单信息表 [surgery_bill_no|surgery_bill_name|apply_surgery_date|surgery_order_no|operating_table|exec_surgery_date|apply_depot_name|apply_doctor_name|exec_depot_name|exec_doctor_name|patient_name|patient_gender|patient_age|patient_no]
    if(tables.indexOf("OperationInfo") == -1)
    {
        QSqlQuery query(db_cabinet);
        QString cmd = QString("create table OperationInfo(\
                              ssc_surgery_bill_id CHAR(20) PRIMARY KEY NOT NULL,\
                              surgery_bill_no CHAR(20) DEFAULT(''),\
                              surgery_bill_name CHAR(20) DEFAULT(''),\
                              apply_surgery_date CHAR(12) DEFAULT(''),\
                              surgery_order_no CHAR(20) DEFAULT(''),\
                              operating_table CHAR(20) DEFAULT(''),\
                              exec_surgery_date   CHAR(12) DEFAULT(''),\
                              apply_depot_name  CHAR(20) DEFAULT(''),\
                              apply_doctor_name  CHAR(20) DEFAULT(''),\
                              exec_depot_name  CHAR(20) DEFAULT(''),\
                              exec_doctor_name  CHAR(20) DEFAULT(''),\
                              patient_name  CHAR(20) DEFAULT(''),\
                              patient_gender   CHAR(2) DEFAULT(''),\
                              patient_age  CHAR(3) DEFAULT(''),\
                              patient_no  CHAR(20) DEFAULT('')\
                              );");
        if(query.exec(cmd))
        {
            qDebug()<<"[create table]"<<"OperationInfo"<<"success";
            needSync = true;
        }
        else
        {
            qDebug()<<"[create table]"<<"OperationInfo"<<"failed"<<query.lastError();
        }
    }
}

void SqlManager::createField(QString tabName ,QString fieldName, int fSize)
{
    QSqlQuery query(db_cabinet);
    QString cmd = QString("select sql from sqlite_master where type = 'table' and name = '%1'").arg(tabName);
    if(!queryExec(&query, QString("[Check Field] %1").arg(fieldName), cmd))
        return;

    if(query.next())
    {
        QString tabStr = query.value(0).toString();
//        qDebug()<<tabStr;
        if(tabStr.indexOf(fieldName) == -1)//不存在字段
        {
            qDebug()<<"[SqlManager]:createField:"<<fieldName<<"in table"<<tabName;
            cmd = QString("ALTER TABLE %1 ADD COLUMN %2 CHAR(%3) DEFAULT('')").arg(tabName).arg(fieldName).arg(fSize);
            queryExec(&query, QString("[Create Field] %1").arg(fieldName), cmd);
        }
    }
    return;
}

bool SqlManager::queryExec(QSqlQuery* q, QString msg, QString cmd)
{
    if(cmd.isEmpty())
    {
        if(!q->exec())
        {
            qDebug()<<q->lastQuery();
            qDebug()<<"[sqlite]"<<msg<<"failed"<<q->lastError();
            return false;
        }
    }
    else
    {
        if(!q->exec(cmd))
        {
            qDebug()<<q->lastQuery();
            qDebug()<<"[sqlite]"<<msg<<"failed"<<q->lastError();
            return false;
        }
    }
    //qDebug()<<q->lastQuery();
    return true;
}
