#ifndef EPCMODEL_H
#define EPCMODEL_H
#include <QAbstractTableModel>
#include <QVariant>
#include <QMap>
#include <QStringList>
#include <QDateTime>
#include <QModelIndex>
#include "sql/sqlmanager.h"
#include "defines.h"
#include "cabinetconfig.h"

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
    EpcInfo* getEpcInfo(QString code);
//    void updateColumn(int col);
    void clearEpcMark();
    void setEpcMark(QString epcId, EpcMark mark);
    void lockEpcMark(QString epcId);
    void unLockEpcMark(QString epcId);
    void setEpcState(QString epcId, EpcState state);
    void updateStamp(QString epcId);
    void transEpcMark(EpcMark mark_before, EpcMark mark_after);
    void clear();
    void setCurOpt(QString optId);
    int getMarkCount();
    bool markInfoCompleted();
    void refrushModel();
    bool epcCheckActive(quint64 msecs = 10000);//标签盘点活跃
    QStringList markTab();
    void operation(QString goodsCode);
    void operation(QString goodsCode, EpcMark mark);
    void unknowEpc(QString epc);
    void clearUnknowEpcs();
    int checkOptTime(int downCount);//无操作倒计时

public slots:
    void syncUpload();
    void syncDownload();
    void epcConsume(QStringList epcs);
    void curOptNoChanged(QString optNo);

signals:
    void scanProgress(int scanCount, int totalCount);
    void updateCount(EpcMark mark, int count);
    void updateLockCount(int count);
    void updateUnknowCount(int count);
    void clearCount();
    void epcAccess(QStringList epcs, UserOpt optType);
    void epcAccess(QStringList fetchEpcs, QStringList backEpcs,QString optNo);
    void epcStore(QVariantMap);
    void epcConsumeCheck(QStringList);

private:
    QMap<QString, EpcInfo*> map_rfid;
    QMap<QString, EpcInfo*> map_code;
    QMap<QString, QString> map_col_name;
    QStringList colsName;
    QStringList markNameTab;
    QStringList optList;
    QStringList unknowList;
    QString curOptId;
    QString operationNo;
    quint64 activeStamp;//标签活跃时间戳:最后一次标签状态发生变化的时间
    quint64 clearStamp;//初始化时间戳:用于计算扫描强度
    CabinetConfig* config;
    int countTab[20];
    int markCount;//count be marked
    int outCount;//count for fetch out
    int lockCount;

    void initColName();
    void storeEpcs(QStringList epcs);
//    void refrushCount();
};

#endif // EPCMODEL_H
