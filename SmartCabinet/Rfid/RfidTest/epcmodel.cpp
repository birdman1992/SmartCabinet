#include "epcmodel.h"

EpcModel::EpcModel(QObject *parent)
    :QAbstractTableModel(parent)
{
    colsName.clear();
    colsName<<"物品"<<"RFID"<<"条码"<<"规格"<<"生产商"<<"供应商"<<"操作人"<<"时间";
}

int EpcModel::rowCount(const QModelIndex &) const
{
    return map_rfid.count();
}

int EpcModel::columnCount(const QModelIndex &) const
{
    return 8;
}

//GI.name, EI.epc_code, EI.goods_code, GI.size, GI.pro_name, GI.sup_name, EI.opt_id, EI.time_stamp
//|物品|RFID|条码|规格|生产商|供应商|操作人|时间|
QVariant EpcModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
    {
        return QVariant();
    }
    EpcInfo* info = map_rfid.values().at(index.row());
    if(role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case 0:return info->name;
            break;
        case 1:return info->epcId;
            break;
        case 2:return info->goodsCode;
            break;
        case 3:return info->size;
            break;
        case 4:return info->pro_name;
            break;
        case 5:return info->sup_name;
            break;
        case 6:return info->lastOpt;
            break;
        case 7:return info->lastStamp;
            break;
        default:return QVariant();
        }
    }
    return QVariant();
}


QVariant EpcModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(section >= colsName.count())
        return QVariant();

    if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return QVariant(colsName[section]);

    return QAbstractTableModel::headerData(section, orientation, role);
}

void EpcModel::operator<<(EpcInfo *info)
{
    if(map_rfid.contains(info->epcId))
    {
        EpcInfo* oldInfo = map_rfid[info->epcId];
        if(info == oldInfo)
            return;
        map_rfid.remove(oldInfo->epcId);
        delete oldInfo;
    }
    map_rfid.insert(info->epcId, info);
}

EpcInfo *EpcModel::operator[](QString code)
{
    return map_rfid[code];
}

void EpcModel::clear()
{
    if(map_rfid.isEmpty())
        return;

    qDeleteAll(map_rfid.begin(), map_rfid.end());
    map_rfid.clear();
}

void EpcModel::syncUpload()
{
    SqlManager::updateRfidsStart();
    foreach (EpcInfo* info, map_rfid)
    {
        SqlManager::updateRfidsSingle(info->epcId, info->lastStamp, info->lastOpt, info->state, info->rowPos, info->colPos);
    }
    SqlManager::updateRfidsFinish();
}

void EpcModel::syncDownload()
{
    clear();
    QString cmd = QString("SELECT GI.name, EI.epc_code, EI.goods_code, GI.size, GI.pro_name, GI.sup_name, EI.opt_id, EI.time_stamp, EI.state FROM EpcInfo AS EI LEFT JOIN GoodsInfo AS GI ON EI.goods_id=GI.goods_id");
    QSqlQuery query = SqlManager::query(cmd, QString("[RFID sync download]"));
    while(query.next())
    {
        EpcInfo* info = new EpcInfo(query.value(1).toString(), query.value(2).toString());
        info->name = query.value(0).toString();
        info->size = query.value(3).toString();
        info->pro_name = query.value(4).toString();
        info->sup_name = query.value(5).toString();
        info->lastOpt = query.value(6).toString();
        info->lastStamp = query.value(7).toLongLong();
        info->state = (EpcState)query.value(8).toInt();
        map_rfid.insert(info->epcId, info);
//        qDebug()<<info->state;
//        emit updateEpcInfo(info);
    }
}

void EpcModel::initColName()
{
    map_col_name.insert("epc_code", "RFID");
    map_col_name.insert("goods_code", "条码");
    map_col_name.insert("name", "物品");
    map_col_name.insert("size", "规格");
    map_col_name.insert("opt_id", "操作人");
    map_col_name.insert("time_stamp", "时间");
    map_col_name.insert("pro_name", "生产商");
    map_col_name.insert("sup_name", "供应商");
}
