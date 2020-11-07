#ifndef EPCSUMMODEL_H
#define EPCSUMMODEL_H
#include <QString>
#include <QList>
#include <QMap>
#include <QAbstractTableModel>
#include <QStringList>

enum EpcState{
    epc_no,//标签未被发现
    epc_in,//标签在柜内
    epc_out,//标签被取出
    epc_consume,//标签被消耗
};

//
enum EpcMark{
    mark_no,
    mark_new,//新发现的标签,存入标签
    mark_back,//还回的标签
    mark_out,//取出的标签
    mark_con,//登记消耗的标签
    mark_in,//柜内标签
    mark_wait_back,//取出未归还的标签
    mark_all,//所有标签,不作为标记使用
    mark_away,//离柜标签
    mark_checked,//发现标签
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
    bool epcScaned(qint64 scanMs);//扫描持续时间:ms
    QString epcId;
    QString name;
    QString size;
    QString pro_name;
    QString sup_name;
    QString goodsCode;//对应物品条码
    QString package_id;
    float price;
    qint64 lastStamp;//上次更新的时间戳
    EpcState state;//当前状态
    qint32 scanedTimes;//扫描次数计数
    float signalIntensity;//信号强度
    EpcMark mark;//
    bool markLock;//lock the mark
    QString lastOpt;//上次操作人
    int colPos;//物品列坐标
    int rowPos;//物品行坐标
};

//EPC汇总信息
class EpcSumInfo
{
public:
    EpcSumInfo() {clearCount();}
    void clearCount(){memset(count, 0, sizeof(count));}

    QString package_id;
    QString size;
    int count[20];
    QString pro_name;
    QString sup_name;
    QString name;
};

class EpcSumModel : public QAbstractTableModel
{
public:
    EpcSumModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    EpcMark scene();
    QString sceneStr();
    void clear();


    QMap<QString, EpcSumInfo*> map_sumInfo;
    void regSumInfo(QMap<QString, EpcInfo *> map_rfid);
    void setEpcMark(QString pacId, EpcMark mark, EpcMark oldMark);
    void setEpcMark(QString pacId, EpcMark mark);
    void setScene(EpcMark mark);
private:
    EpcMark curScene;//当前计数场景
    QStringList colsName;
    QStringList markNameTab;
    void refrushModel();
};

#endif // EPCSUMMODEL_H
