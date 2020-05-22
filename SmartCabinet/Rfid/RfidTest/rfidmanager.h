#ifndef RFIDMANAGER_H
#define RFIDMANAGER_H

#include <QObject>
#include <QMap>
#include <QTime>
#include <QTimer>
#include <QTableWidget>
#include <QScrollBar>
#include <QDateTime>
#include <QList>
#include <QSortFilterProxyModel>

#include "rfidreader.h"
#include "sql/sqlmanager.h"
#include "manager/signalmanager.h"
#include "cabinetconfig.h"
#include "Rfid/RfidTest/epcmodel.h"
#include "rfiddevhub.h"

class RfidManager : public QObject
{
    Q_OBJECT
public:
    explicit RfidManager(EpcModel *model, QObject *parent = 0);
    void setCurOptId(QString optId);
    void initColName();
    void startScan();//开始扫描
    void doorCloseScan();//结束扫描
    void epcCheck(int row=0, int col=0);//盘点标签
    void epcSync();//同步标签信息
    void timerClear();
public slots:
    void newRfidMark(QString epc, QString goodsCode, QString goodsId);
    void clsFinish();//结束结算
    void clsGiveUp();//放弃结算
    void doorStateChanged(int id, bool isOpen);

signals:
    void updateTimer(int);
//    void updateCount(int);
//    void updateEpcInfo(EpcInfo*);
    void epcStateChanged(TableMark changedTableMark);
    void epcAccess(QStringList epcs, int optType);
    void optFinish();//用户操作全部完成：柜门全部关闭

private:
    RfidReader* testReader;
    RfidReader* testReader2;
//    QTableView* table_out;
//    QTableView* table_in;
//    QTableView* table_back;
//    QTableView* table_con;
//    QTableView* table_check;
//    QSortFilterProxyModel* model_out;
//    QSortFilterProxyModel* model_in;
//    QSortFilterProxyModel* model_back;
//    QSortFilterProxyModel* model_con;
//    QSortFilterProxyModel* model_check;

    EpcModel* eModel;
    RfidDevHub* rfidHub;

    CabinetConfig* config;
    QMap<QString, EpcInfo*> map_rfid;
    QMap<QString, QString> map_col_name;
    QList<RfidReader*> list_device;
    QStringList list_epc;
    QStringList list_new;//新发现的标签
    QStringList list_out;//取出的标签
    QStringList list_back;//还回的标签
    QStringList list_con;//登记消耗的标签
    QStringList list_ign;//忽略表
    QStringList list_check;//盘点表
    quint64 clsStamp;
    quint32 doorState;
    quint16 insideAnt;
    quint16 outsideAnt;
    QTime scanTimer;
    QTimer upTimer;
    bool accessLock;//存取锁定状态
    bool flagInit;//初始化标志
    bool flagScan;//扫描状态
    int clsTime;//结算延迟
    int tabMark;
//    void listShow(QStringList epcs, QTableWidget* table, TableMark mark);
//    void queryShow(QSqlQuery query, QTableWidget* table);
    void recordClear();
    void timerStart();
    void timerStop();


private slots:
    void initEpc();//程序启动初始化EPC标签
    void updateEpc(QString epc, int seq, int ant);
    void testUpdateEpc(QString epc, int seq, int ant);
    void clsTimeOut();
    void timerUpdate();
};

#endif // RFIDMANAGER_H
