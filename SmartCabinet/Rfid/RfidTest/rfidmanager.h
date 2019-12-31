#ifndef RFIDMANAGER_H
#define RFIDMANAGER_H

#include <QObject>
#include <QMap>
#include <QTableWidget>
#include "rfidreader.h"
#include "sql/sqlmanager.h"


enum EpcState{
    epc_no,//标签未被发现
    epc_in,//标签在柜内
    epc_out,//标签被取出
    epc_consume,//标签被消耗
};

enum TableMark
{
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
    quint32 lastStamp;//上次更新的时间戳
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
    void initEpc();//程序启动初始化EPC标签
    void startScan();//开始扫描
    void stopScan();//结束扫描
    void epcCheck(quint32 cutOffStamp, int row=0, int col=0);//盘点标签
    void epcSync();//同步标签信息
    void initTableViews(QTableWidget* in=NULL, QTableWidget* out=NULL, QTableWidget* back=NULL, QTableWidget* con=NULL);

public slots:
    void newRfidMark(QString epc, QString goodsCode);

signals:
    void updateEpcInfo(EpcInfo*);
    void epcStateChanged(TableMark changedTableMark);

private:
    RfidReader* testReader;
    QTableWidget* table_out;
    QTableWidget* table_in;
    QTableWidget* table_back;
    QTableWidget* table_con;
    QMap<QString, EpcInfo*> map_rfid;
    QList<RfidReader*> list_device;
    QStringList list_epc;
    QStringList list_new;//新发现的标签
    QStringList list_out;//取出的标签
    QStringList list_back;//还回的标签
    QStringList list_con;//登记消耗的标签
    int clsTime;//结算延迟

private slots:
    void updateEpc(QString epc, int colPos);
    void testUpdateEpc(QString, int colPos);
};

#endif // RFIDMANAGER_H
