#include "epcsummodel.h"
#include <QtDebug>

EpcSumModel::EpcSumModel()
{
    curScene = mark_out;
    colsName.clear();
    colsName<<"ID"<<"物品"<<"规格"<<"数量"<<"生产商"<<"供应商";
    markNameTab.clear();
    markNameTab<<"未发现"<<"存入"<<"还回"<<"取出"<<"登记"<<"实时库存"<<"取出未还"<<"总览"<<"离柜"<<"发现";

}

int EpcSumModel::rowCount(const QModelIndex &) const
{
    return map_sumInfo.count();
}

int EpcSumModel::columnCount(const QModelIndex &) const
{
    return colsName.count();
}

QVariant EpcSumModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
    {
        return QVariant();
    }
    EpcSumInfo* info = map_sumInfo.values().at(index.row());

    if(role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case 0:return info->package_id;
            break;
        case 1:return info->name;
            break;
        case 2:return info->size;
            break;
        case 3:return info->count[curScene];
            break;
        case 4:return info->pro_name;
            break;
        case 5:return info->sup_name;
            break;

        default:return QVariant();
        }
    }
    if(role == Qt::TextAlignmentRole)//居中
        return Qt::AlignCenter;

    return QVariant();
}

QVariant EpcSumModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return QVariant(colsName[section]);

//    if(role == Qt::DisplayRole && orientation == Qt::Vertical)
//        return QVariant(section+1);

    return QAbstractTableModel::headerData(section, orientation, role);
}

EpcMark EpcSumModel::scene()
{
    return curScene;
}

QString EpcSumModel::sceneStr()
{
    return markNameTab.at(curScene);
}

void EpcSumModel::clear()
{
    foreach (EpcSumInfo* info, map_sumInfo)
    {
        info->clearCount();
    }
    refrushModel();
}

/**
 * @brief EpcSumModel::regSumInfo 初始化注册汇总信息
 * @param map_rfid
 */
void EpcSumModel::regSumInfo(QMap<QString, EpcInfo*> map_rfid)
{
    qDeleteAll(map_sumInfo.begin(), map_sumInfo.end());
    map_sumInfo.clear();

    foreach(EpcInfo* info, map_rfid)
    {
        EpcSumInfo* sumInfo;
        if(!map_sumInfo.contains(info->package_id))
        {
            sumInfo = new EpcSumInfo;
            sumInfo->package_id = info->package_id;
            sumInfo->name = info->name;
            sumInfo->pro_name = info->pro_name;
            sumInfo->sup_name = info->sup_name;
            sumInfo->size = info->size;
            map_sumInfo.insert(info->package_id, sumInfo);
        }
        else
        {
            sumInfo = map_sumInfo[info->package_id];
        }

        switch(info->state)
        {
//        case epc_no://
//            setEpcMark(info->package_id,mark_new);break;

        case epc_in://
            setEpcMark(info->package_id,mark_in);break;//柜内标签

        case epc_out://
            setEpcMark(info->package_id,mark_away);break;//离柜标签

//        case epc_consume://
//            setEpcMark(info->package_id,mark_);break;

        default:
            break;
        }
    }
    refrushModel();
}

void EpcSumModel::refrushModel()
{
    beginResetModel();
    endResetModel();
    //    emit updateCount(this->rowCount(QModelIndex()));
}

void EpcSumModel::setEpcMark(QString pacId, EpcMark mark, EpcMark oldMark)
{
    map_sumInfo[pacId]->count[mark]++;
    map_sumInfo[pacId]->count[oldMark]--;
    QModelIndex topLeft, bottomRight;
    topLeft = createIndex(map_sumInfo.keys().indexOf(pacId), 3);
    bottomRight = createIndex(map_sumInfo.keys().indexOf(pacId), 3);
    emit dataChanged(topLeft, bottomRight);
    qDebug()<<"[EpcSumModel]"<<"pac:"<<pacId<<map_sumInfo[pacId]->count[mark];
}

void EpcSumModel::setEpcMark(QString pacId, EpcMark mark)
{
    map_sumInfo[pacId]->count[mark]++;
    QModelIndex topLeft, bottomRight;
    topLeft = createIndex(map_sumInfo.keys().indexOf(pacId), 3);
    bottomRight = createIndex(map_sumInfo.keys().indexOf(pacId), 3);
    emit dataChanged(topLeft, bottomRight);
//    qDebug()<<"[EpcSumModel]"<<"pac:"<<pacId<<mark<<map_sumInfo[pacId]->count[mark];
}

void EpcSumModel::setScene(EpcMark mark)
{
    curScene = mark;
    refrushModel();
}
