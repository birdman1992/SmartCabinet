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
#include "epcsummodel.h"
#include "Rfid/RfidTest/epcsummodel.h"

class EpcModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    EpcModel(QObject *parent = NULL);
    ~EpcModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void operator<<(EpcInfo* info);
    EpcInfo* operator[](QString code);
    EpcInfo* getEpcInfo(QString code);
//    void updateColumn(int col);
    EpcSumModel* getSumModel();
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
    EpcSumModel* eSumModel;
    int countTab[20];
    int markCount;//count be marked
    int outCount;//count for fetch out
    int lockCount;

    void initColName();
    void storeEpcs(QStringList epcs);
//    void refrushCount();
};

#endif // EPCMODEL_H
