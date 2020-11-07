#include "epcmodel.h"
#include <QDebug>

EpcInfo::EpcInfo(QString id, QString _goodsCode)
{
    epcId = id;
    goodsCode = _goodsCode;
    lastStamp = 0;
    state = epc_no;
    mark = mark_no;
    markLock = false;
    rowPos = 0;
    colPos = 0;
    scanedTimes = 0;
    signalIntensity = 0;

}

bool EpcInfo::epcScaned(qint64 scanMs)
{
    scanedTimes++;
    float oldSigInts = signalIntensity;
    if(scanMs)
    {
        //被扫描次数/扫描时间（s）
        signalIntensity = qRound((float)scanedTimes*1000/scanMs*100)/100.0;
        qDebug()<<scanedTimes<<"/"<<(scanMs)<<signalIntensity;
    }
    return oldSigInts == signalIntensity;
}


EpcModel::EpcModel(QObject *parent)
    :QAbstractTableModel(parent)
{
    config = CabinetConfig::config();
    markCount = 0;
    eSumModel = new EpcSumModel;
    curOptId = QString();
    colsName.clear();
//    colsName<<"物品"<<"条码"<<"规格"<<"生产商"<<"供应商"<<"操作人"<<"时间"<<"标记"<<"操作";
    colsName<<"ID"<<"HIS编码"<<"物品"<<"规格"<<"单价"<<"生产商"<<"供应商"<<"操作人"<<"时间"<<"标记"<<"操作";
    markNameTab.clear();
    markNameTab<<"未发现"<<"存入"<<"还回"<<"取出"<<"登记"<<"实时库存"<<"取出未还"<<"总览"<<"离柜"<<"发现";
    optList    <<"--"   <<"--"  <<"--" <<"移除"<<"--" <<"--"     <<"--"    <<"--"  <<"--" <<"--";
}

EpcModel::~EpcModel()
{
    delete eSumModel;
}

int EpcModel::rowCount(const QModelIndex &) const
{
    return map_rfid.count();
}

int EpcModel::columnCount(const QModelIndex &) const
{
    return colsName.count();
}

#include "funcs/secwatch.h"
//GI.name, EI.epc_code, EI.goods_code, GI.size, GI.pro_name, GI.sup_name, EI.opt_id, EI.time_stamp
//|ID|HIS编码|物品|规格|单价|生产商|供应商|操作人|时间|标记|操作
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
        case 0:return info->package_id;
            break;
        case 1:return info->goodsCode;
            break;
        case 2:return info->name;
            break;
        case 3:return info->size;
            break;
        case 4:return info->price;
            break;
        case 5:return info->pro_name;
            break;
        case 6:return info->sup_name;
            break;
        case 7:return info->lastOpt;
            break;
        case 8:return (info->lastStamp)?(QDateTime::fromMSecsSinceEpoch(info->lastStamp).toString("yyyy-MM-dd hh:mm:ss")):("");
            break;
        case 9:return markNameTab[info->mark];
            break;
//        case 10:return info->signalIntensity;
//            break;
        case 10:return optList[info->mark];
            break;

        default:return QVariant();
        }
    }
    if(role == Qt::TextAlignmentRole)//居中
        return Qt::AlignCenter;

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
    map_code.insert(info->goodsCode, info);
}

EpcInfo *EpcModel::operator[](QString code)
{
    return map_rfid.value(code, NULL);
}

EpcInfo *EpcModel::getEpcInfo(QString code)
{
    return map_rfid.value(code, NULL);
}

EpcSumModel *EpcModel::getSumModel()
{
    return eSumModel;
}

//void EpcModel::updateColumn(int col)
//{
//    emit dataChanged(createIndex(0, col), createIndex(rowCount()-1, col));
//}

void EpcModel::clearEpcMark()
{
    QStringList consumCheckList;
    memset(countTab, 0, sizeof(countTab));
    countTab[0] = map_rfid.count();
    QDateTime outOverTime = QDateTime::currentDateTime().addSecs(-3600);//这个时间点之前取出的都标记为取出未归还
    foreach(EpcInfo* info, map_rfid)
    {
        info->mark = mark_no;
        info->markLock = false;
        info->signalIntensity = 0;
        info->scanedTimes = 0;

        if(config->getCabinetType().at(BIT_LOW_HIGH))//高值柜
        {
            if(info->state == epc_out)//取出
            {
                qDebug()<<"[out mark]"<<info->goodsCode;
                consumCheckList<<info->epcId;
                if(QDateTime::fromMSecsSinceEpoch(info->lastStamp) < outOverTime)//未归还
                    setEpcMark(info->epcId, mark_wait_back);
                else
                {
                    qDebug()<<"mark_away";
                    setEpcMark(info->epcId, mark_away);
                }
            }
        }
    }
    //清除计数
    eSumModel->clear();
    markCount = 0;
    lockCount = 0;
    activeStamp = 0;
    clearStamp = QDateTime::currentMSecsSinceEpoch();

    clearUnknowEpcs();

    emit epcConsumeCheck(consumCheckList);
    emit updateLockCount(lockCount);
    emit scanProgress(markCount, map_rfid.count());
    refrushModel();
}

void EpcModel::setEpcMark(QString epcId, EpcMark mark)
{
    EpcInfo* info = map_rfid.value(epcId, NULL);
    if(info == NULL)
        return;
    if(info->markLock)//锁定的标签
        return;
    if(info->mark == mark)//无变化标签
        return;
    if((!info->mark) && mark)//新标记的标签
    {
        markCount++;
        emit scanProgress(markCount, map_rfid.count());
    }
    qint64 curStamp = QDateTime::currentMSecsSinceEpoch();
    EpcMark oldMark = info->mark;

    countTab[oldMark]--;
    countTab[mark]++;
    qDebug()<<"[setEpcMark]"<<epcId<<info->mark<<"->"<<mark<<"count:"<<markCount<<"countTab:"<<countTab[mark];
    eSumModel->setEpcMark(info->package_id, mark, info->mark);

    info->mark = mark;
    if(info->mark != mark_wait_back)
    {
        info->lastStamp = curStamp;
        info->lastOpt = curOptId;
        //更新活跃时间戳
        activeStamp = info->lastStamp;
    }

    QModelIndex topLeft, bottomRight;
    topLeft = createIndex(map_rfid.keys().indexOf(epcId), colsName.indexOf("操作人"));
    bottomRight = createIndex(map_rfid.keys().indexOf(epcId), colsName.indexOf("操作"));

//    reset();
    emit dataChanged(topLeft, bottomRight);
    emit updateCount(oldMark, countTab[oldMark]);//更新旧的标记数量
    emit updateCount(info->mark, countTab[info->mark]);//更新新的标记数量
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
    qDebug()<<"locked:"<<epcId;
    qDebug()<<"[EpcModel] lock count:"<<lockCount;
}

void EpcModel::unLockEpcMark(QString epcId)
{
    if(!map_rfid.contains(epcId))
    {
        return;
    }
    if(map_rfid[epcId]->markLock == false)
        return;

    map_rfid[epcId]->markLock = false;
    lockCount--;
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
    QStringList list_fetch;
    QStringList list_back;
    QStringList list_store;
//    QStringList list_consume;
    SqlManager::begin();
    QString cmd;
    foreach (EpcInfo* info, map_rfid)
    {
        if(info->mark != eSumModel->scene()){
//            qDebug()<<"[syncUpload]:ignore"<<info->epcId<<info->mark<<eSumModel->scene();
            continue;
        }

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
            list_back<<info->epcId;
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
            map_rfid.remove(info->epcId);
            delete info;
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
            list_store<<info->epcId;
            break;
        case mark_out://取出标记
            if(info->state == epc_no)
            {
                list_store<<info->epcId;
            }
            info->state = epc_out;
            cmd = QString("UPDATE EpcInfo SET time_stamp=%1, opt_id='%2', state=%3 WHERE epc_code='%4'")
                    .arg(info->lastStamp)
                    .arg(info->lastOpt)
                    .arg(info->state)
                    .arg(info->epcId);
            list_fetch<<info->epcId;
            break;
        default:
            break;
        }
        if(!cmd.isEmpty())
            SqlManager::querySingle(cmd, "[syncUpload]");
    }
    qDebug()<<"存入:"<<list_store.count()<<"取出:"<<list_fetch.count()<<"还回:"<<list_back.count();

    //存入物品
    if(!list_store.isEmpty())
        storeEpcs(list_store);

    emit epcAccess(list_fetch, list_back, operationNo);

    //删除已登记消耗物品
    SqlManager::querySingle(QString("DELETE FROM EpcInfo WHERE state=%1").arg(epc_consume));
    SqlManager::commit();
}

void EpcModel::syncDownload()
{
    clear();
    QString cmd = QString("SELECT GI.name, EI.epc_code, EI.goods_code, GI.size, GI.pro_name, GI.sup_name, EI.opt_id, EI.time_stamp, EI.state, GI.package_id, GI.single_price FROM EpcInfo AS EI LEFT JOIN CodeInfo AS CI ON EI.goods_code=CI.code LEFT JOIN GoodsInfo AS GI ON CI.package_id=GI.package_id");
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
        info->package_id = query.value(9).toString();
        info->price = query.value(10).toFloat();
        map_rfid.insert(info->epcId, info);
        map_code.insert(info->goodsCode, info);
        countTab[info->mark]++;
//        qDebug()<<info->state;
//        emit updateEpcInfo(info);
    }
    //注册汇总信息
    eSumModel->regSumInfo(map_rfid);
    refrushModel();
    countTab[0] = map_rfid.count();
    qDebug()<<"[model row]"<<map_rfid.count();
}

void EpcModel::epcConsume(QStringList epcs)
{
    qDebug()<<"consumed epsc"<<epcs;
    return;
    foreach (QString epc, epcs)
    {
        setEpcMark(epc, mark_con);
    }
}

void EpcModel::curOptNoChanged(QString optNo)
{
//    qDebug();
    operationNo = optNo;
}

void EpcModel::setCurOpt(QString optId)
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

/**
 * @brief EpcModel::epcCheckActive
 * @param msecs 标签活跃阈值,距离上一次活跃时间戳超过此阈值就认为标签盘点稳定(非活跃)了
 * @return  标签盘点是否活跃  true:活跃 false:非活跃
 */
bool EpcModel::epcCheckActive(quint64 msecs)
{
    return QDateTime::currentMSecsSinceEpoch() - activeStamp < msecs;
}

QStringList EpcModel::markTab()
{
    return markNameTab;
}

void EpcModel::operation(QString goodsCode)
{
    EpcInfo* info = map_code.value(goodsCode, NULL);
    if(info == NULL)
        return;

    qDebug()<<"[EpcModel] operation:"<<goodsCode;
    unLockEpcMark(info->epcId);

    if(info->mark == mark_out)
        setEpcMark(info->epcId, mark_in);

    lockEpcMark(info->epcId);
}

void EpcModel::operation(QString goodsCode, EpcMark mark)
{
    EpcInfo* info = map_code.value(goodsCode, NULL);
    if(info == NULL)
        return;

    qDebug()<<"[EpcModel] operation:"<<goodsCode;
    unLockEpcMark(info->epcId);

    if(info->mark == mark_out)
        setEpcMark(info->epcId, mark);

    lockEpcMark(info->epcId);
}

void EpcModel::unknowEpc(QString epc)
{
    if(unknowList.contains(epc))
        return;

    unknowList<<epc;
    emit updateUnknowCount(unknowList.count());
}

void EpcModel::clearUnknowEpcs()
{
    unknowList.clear();
    emit updateUnknowCount(unknowList.count());
}

/**
 * @brief EpcModel::checkOptTime 完成一个倒计时
 * @param downCount 总倒数计数
 * @return 剩余倒数计数
 */
int EpcModel::checkOptTime(int downCount)
{
    if(activeStamp == 0){
        activeStamp = QDateTime::currentMSecsSinceEpoch();
        return downCount;
    }

     int ret = downCount - (QDateTime::currentMSecsSinceEpoch() - activeStamp)/1000;
     if(ret < 0)
         return 0;
     return ret;
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

//packageBarcode barcode rfidCodes
/**
 * @brief EpcModel::storeEpcs
 * @param epcs
 * store_list/package_id/code
 * store_list:packageMap[]
 *          package_id:map
 *                     code:string
 */
void EpcModel::storeEpcs(QStringList epcs)
{
    QString cmd = QString("SELECT EI.epc_code, CI.package_id, CI.store_list FROM EpcInfo AS EI LEFT JOIN CodeInfo AS CI ON EI.goods_code=CI.code WHERE epc_code IN ('%1')").arg(epcs.join("','"));
    QSqlQuery query = SqlManager::query(cmd, "[storeEpcs]");
//    QMap<QString ,QVariantMap> codeMapList;
    QVariantMap reportMap;//store_list/package_id/code

    while(query.next())
    {
        QString package_id = query.value(1).toString();
        QString store_list = query.value(2).toString();

        QVariantMap epcMap;
        epcMap.insert("code", query.value(0));
        QVariantMap packageMap = reportMap.value(store_list).toMap();
        packageMap.insertMulti(package_id, epcMap);
        reportMap.insert(store_list, packageMap);
//        packageMap.insertMulti(package_id,epcMap);
//        reportMap.insertMulti(store_list, packageMap);
//        static_cast<QVariantMap>(reportMap[store_list].toMap());
    }
    qDebug()<<"store count"<<reportMap.count();
//    qDebug()<<reportMap;
    emit epcStore(reportMap);
}
