#include "epcmodel.h"
#include <QDebug>

EpcModel::EpcModel(QObject *parent)
    :QAbstractTableModel(parent)
{
    markCount = 0;
    curOptId = QString();
    colsName.clear();
    colsName<<"物品"<<"条码"<<"规格"<<"生产商"<<"供应商"<<"操作人"<<"时间"<<"标记";
    markNameTab.clear();
    markNameTab<<"未知"<<"存入"<<"还回"<<"取出"<<"登记"<<"柜内"<<"发现";
}

int EpcModel::rowCount(const QModelIndex &) const
{
    return map_rfid.count();
}

int EpcModel::columnCount(const QModelIndex &) const
{
    return 8;
}

#include "funcs/secwatch.h"
//GI.name, EI.epc_code, EI.goods_code, GI.size, GI.pro_name, GI.sup_name, EI.opt_id, EI.time_stamp
//|物品|RFID|条码|规格|生产商|供应商|操作人|时间|
QVariant EpcModel::data(const QModelIndex& index, int role) const
{
//    qDebug()<<"data>>>>>>>>>>>"<<index.row()<<index.column();
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
//        case 1:return info->epcId;
//            break;
        case 1:return info->goodsCode;
            break;
        case 2:return info->size;
            break;
        case 3:return info->pro_name;
            break;
        case 4:return info->sup_name;
            break;
        case 5:return info->lastOpt;
            break;
        case 6:return (info->lastStamp)?(QDateTime::fromMSecsSinceEpoch(info->lastStamp).toString("yyyy-MM-dd hh:mm:ss")):("");
            break;
        case 7:return markNameTab[info->mark];
            break;
        default:return QVariant();
        }
    }
    return QVariant();
}


QVariant EpcModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return QVariant(colsName[section]);

//    if(role == Qt::DisplayRole && orientation == Qt::Vertical)
//        return QVariant(section+1);

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
    return map_rfid.value(code, NULL);
}

EpcInfo *EpcModel::getEpcInfo(QString code)
{
    return map_rfid.value(code, NULL);
}

void EpcModel::clearEpcMark()
{
    memset(countTab, 0, sizeof(countTab));
    countTab[0] = map_rfid.count();
    foreach(EpcInfo* info, map_rfid)
    {
        info->mark = mark_no;
        info->markLock = false;
    }
    markCount = 0;
    lockCount = 0;
    emit updateLockCount(lockCount);
    refrushModel();
}

void EpcModel::setEpcMark(QString epcId, EpcMark mark)
{
    EpcInfo* info = map_rfid.value(epcId, NULL);
    if(info == NULL)
        return;
    if(info->markLock)
        return;
    if(info->mark == mark)
        return;

    if((!info->mark) && mark)
    {
        markCount++;
        emit scanProgress(markCount, map_rfid.count());
    }

    countTab[info->mark]--;
    countTab[mark]++;
    emit updateCount(mark, countTab[mark]);
    emit updateCount(info->mark, countTab[info->mark]);
    qDebug()<<"[setEpcMark]"<<epcId<<info->mark<<"->"<<mark<<"count:"<<markCount<<"countTab:"<<countTab[mark];

    info->mark = mark;
    info->lastStamp = QDateTime::currentMSecsSinceEpoch();
    info->lastOpt = curOptId;
    QModelIndex topLeft,bottomRight;
    topLeft = createIndex(map_rfid.keys().indexOf(epcId), 6);
    bottomRight = createIndex(map_rfid.keys().indexOf(epcId), 8);

//    reset();
    emit dataChanged(topLeft, bottomRight);
//    qDebug()<<"[setover]"<<countTab[info->mark];
}

void EpcModel::lockEpcMark(QString epcId)
{
    if(!map_rfid.contains(epcId))
    {
        return;
    }
    if(map_rfid[epcId]->markLock == true)
        return;

    map_rfid[epcId]->markLock = true;
    lockCount++;
    emit updateLockCount(lockCount);
    qDebug()<<"[EpcModel] lock count:"<<lockCount;
}

void EpcModel::setEpcState(QString epcId, EpcState state)
{
    EpcInfo* info = map_rfid.value(epcId, NULL);
    if(info == NULL)
        return;

    info->state = state;
}

void EpcModel::updateStamp(QString epcId)
{
    EpcInfo* info = map_rfid.value(epcId, NULL);
    if(info == NULL)
        return;

    info->lastStamp = QDateTime::currentMSecsSinceEpoch();
}

void EpcModel::transEpcMark(EpcMark mark_before, EpcMark mark_after)
{
//    qDebug()<<"[transEpcMark]"<<mark_before<<"->"<<mark_after;
    foreach (EpcInfo* info, map_rfid)
    {
        if(info->mark == mark_before)
        {
            info->mark = mark_after;
            countTab[mark_before]--;
            countTab[mark_after]++;
        }
    }
    emit updateCount(mark_before, countTab[mark_before]);
    emit updateCount(mark_after, countTab[mark_after]);
    refrushModel();
}

void EpcModel::clear()
{
    if(map_rfid.isEmpty())
        return;

    qDeleteAll(map_rfid.begin(), map_rfid.end());
    map_rfid.clear();
}

//UPDATE EpcInfo SET time_stamp=%1, opt_id='%2', state=%3, row=%4, col=%5 WHERE epc_code='%6'
void EpcModel::syncUpload()
{
    SqlManager::updateRfidsStart();
    QString cmd;
    foreach (EpcInfo* info, map_rfid)
    {
        cmd = QString();
        switch(info->mark)
        {
        case mark_no:
            break;
        case mark_back://还回标记
            info->state = epc_in;
            cmd = QString("UPDATE EpcInfo SET time_stamp=%1, opt_id='%2', state=%3 WHERE epc_code='%4'")
                    .arg(info->lastStamp)
                    .arg(info->lastOpt)
                    .arg(info->state)
                    .arg(info->epcId);
            break;
        case mark_checked://盘点标记
            break;
        case mark_con://消耗标记
            info->state = epc_consume;
            cmd = QString("UPDATE EpcInfo SET time_stamp=%1, opt_id='%2', state=%3 WHERE epc_code='%4'")
                    .arg(info->lastStamp)
                    .arg(info->lastOpt)
                    .arg(info->state)
                    .arg(info->epcId);
            break;
        case mark_in://柜内标记
            break;
        case mark_new://放入标记
            info->state = epc_in;
            cmd = QString("UPDATE EpcInfo SET time_stamp=%1, opt_id='%2', state=%3 WHERE epc_code='%4'")
                    .arg(info->lastStamp)
                    .arg(info->lastOpt)
                    .arg(info->state)
                    .arg(info->epcId);
            break;
        case mark_out://取出标记
            info->state = epc_out;
            cmd = QString("UPDATE EpcInfo SET time_stamp=%1, opt_id='%2', state=%3 WHERE epc_code='%4'")
                    .arg(info->lastStamp)
                    .arg(info->lastOpt)
                    .arg(info->state)
                    .arg(info->epcId);
            break;
        default:
            break;
        }
        if(!cmd.isEmpty())
            SqlManager::querySingle(cmd, "[syncUpload]");
    }
    SqlManager::updateRfidsFinish();
}

void EpcModel::syncDownload()
{
    clear();
    QString cmd = QString("SELECT GI.name, EI.epc_code, EI.goods_code, GI.size, GI.pro_name, GI.sup_name, EI.opt_id, EI.time_stamp, EI.state FROM EpcInfo AS EI LEFT JOIN GoodsInfo AS GI ON EI.goods_id=GI.goods_id");
    QSqlQuery query = SqlManager::query(cmd, QString("[RFID sync download]"));
    memset(countTab, 0, sizeof(countTab));
//    beginResetModel();

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
        countTab[info->mark]++;
//        qDebug()<<info->state;
//        emit updateEpcInfo(info);
    }
    refrushModel();
    countTab[0] = map_rfid.count();
    qDebug()<<"[model row]"<<map_rfid.count();
}

void EpcModel::setOptId(QString optId)
{
    curOptId = optId;
}

int EpcModel::getMarkCount()
{
    return markCount;
}

bool EpcModel::markInfoCompleted()
{
    return (markCount+outCount >= map_rfid.count());
}

void EpcModel::refrushModel()
{
    beginResetModel();
    endResetModel();
//    emit updateCount(this->rowCount(QModelIndex()));
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
