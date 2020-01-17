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
#include "rfidreader.h"
#include "sql/sqlmanager.h"
#include "cabinetconfig.h"


enum EpcState{
    epc_no,//标签未被发现
    epc_in,//标签在柜内
    epc_out,//标签被取出
    epc_consume,//标签被消耗
};

enum TableMark
{
    tab_no = 0,
    tab_in = 1,//放入表
    tab_out = 2,//取出表
    tab_back = 4,//还回表
    tab_con = 8,//消耗表
};

class EpcInfo{
public:
    EpcInfo(QString id, QString _goodsCode=QString());
    QString epcId;
    QString goodsCode;//对应物品id
    quint64 lastStamp;//上次更新的时间戳
    EpcState state;//当前状态
    QString lastOpt;//上次操作人
    int colPos;//物品列坐标
    int rowPos;//物品行坐标
};


class RfidManager : public QObject
{
    Q_OBJECT
public:
    explicit RfidManager(QObject *parent = 0);
    void initColName();
    void startScan();//开始扫描
    void stopScan();//结束扫描
    void epcCheck(int row=0, int col=0);//盘点标签
    void epcSync();//同步标签信息
    void initTableViews(QTableWidget* in=NULL, QTableWidget* out=NULL, QTableWidget* back=NULL, QTableWidget* con=NULL, QTableWidget *check=NULL);

public slots:
    void newRfidMark(QString epc, QString goodsCode, QString goodsId);
    void clsFinish();//结束结算
    void doorStateChanged(int id, bool isOpen);

signals:
    void updateTimer(int);
    void updateCount(int);
    void updateEpcInfo(EpcInfo*);
    void epcStateChanged(TableMark changedTableMark);
    void epcAccess(QStringList epcs, int optType);

private:
    RfidReader* testReader;
    RfidReader* testReader2;
    QTableWidget* table_out;
    QTableWidget* table_in;
    QTableWidget* table_back;
    QTableWidget* table_con;
    QTableWidget* table_check;
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
    bool flagCorct;//数据矫正标志
    bool flagInit;//初始化标志
    bool flagScan;//扫描状态
    int clsTime;//结算延迟
    int tabMark;
    void listShow(QStringList epcs, QTableWidget* table, TableMark mark);
    void queryShow(QSqlQuery query, QTableWidget* table);
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
