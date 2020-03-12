#ifndef EPCMODEL_H
#define EPCMODEL_H
#include <QAbstractTableModel>
#include <QVariant>
#include <QMap>
#include <QStringList>
#include "sql/sqlmanager.h"

enum EpcState{
    epc_no,//标签未被发现
    epc_in,//标签在柜内
    epc_out,//标签被取出
    epc_consume,//标签被消耗
};

//
enum EpcMark{
    mark_no,
    mark_new,//新发现的标签
    mark_back,//还回的标签
    mark_out,//取出的标签
    mark_con,//登记消耗的标签
    mark_check,//盘点标签
};

enum TableMark
{
    tab_no = 0,
    tab_in = 1,//放入表
    tab_out = 2,//取出表
    tab_back = 4,//还回表
    tab_con = 8,//消耗表
};
////GI.name, EI.epc_code, EI.goods_code, GI.size, GI.pro_name, GI.sup_name, EI.opt_id, EI.time_stamp
class EpcInfo{
public:
    EpcInfo(QString id, QString _goodsCode=QString());
    QString epcId;
    QString name;
    QString size;
    QString pro_name;
    QString sup_name;
    QString goodsCode;//对应物品id
    quint64 lastStamp;//上次更新的时间戳
    EpcState state;//当前状态
    EpcMark mark;//
    bool markLock;//lock the mark
    QString lastOpt;//上次操作人
    int colPos;//物品列坐标
    int rowPos;//物品行坐标
};

class EpcModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    EpcModel(QObject *parent = NULL);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void operator<<(EpcInfo* info);
    EpcInfo* operator[](QString code);
    void clearEpcMark();
    void setEpcMark(QString epcId, EpcMark mark);
    void lockEpcMark(QString epcId);
    void clear();
    void syncUpload();
    void syncDownload();

private:
    QMap<QString, EpcInfo*> map_rfid;
    QMap<QString, QString> map_col_name;
    QStringList colsName;
    int markCount;

    void initColName();
};

#endif // EPCMODEL_H
