#ifndef CABINETSTORELIST_H
#define CABINETSTORELIST_H

#include <QWidget>
#include <QList>
#include <qtimer.h>
#include <QStringList>
#include <QMap>
#include "manager/goodsmanager.h"
#include "manager/storelistmanager.h"
#include "Structs/goodslist.h"
#include "cabinetstorelistitem.h"
#include "cabinetconfig.h"

namespace Ui {
class CabinetStoreList;
}

class CabinetStoreList : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetStoreList(QWidget *parent = 0);
    ~CabinetStoreList();
    void storeStart(GoodsList* l);
    void listError(QString msg);
    void storeFinish();
    void recvScanCode(QString scanCode);
    void storeScan(QString scanCode);
    void bindRst(CaseAddress addr);
    void bindMsg(QString msg);
    void storeRst(QString msg, bool success);
    void setLoginState(bool login);

    bool installGlobalConfig(CabinetConfig *globalConfig);

public slots:
    void show();
    void recvStoreTraceRst(bool success, QString msg, QString goodsCode);

private:
    Ui::CabinetStoreList *ui;
    enum GOODS_STATE{
        STATE_NULL,
        STATE_WAIT,
        STATE_PASS,
        STATE_ERROR,
    };

    void paintEvent(QPaintEvent *);
    void clearList();
    void closeClear();
    void saveList();
    void storeContinue(GoodsList* l);
    void newMsg(QString msg);
    bool checkStoreList();//检查存货单是否都扫描了
    void updateStoreList(QList<CabinetStoreListItem*> l);
    void updateScanGoods(Goods* goods);
    void updateScanPanel(QString goodsId);
    void addItem(CabinetStoreListItem* item);
    void setGoodsState();
    void setStateMsg(GOODS_STATE state, QString msg);
    QString scanDataTrans(QString code);
    QString stateStyleSheet(GOODS_STATE state);
    void storeSuccess(QString goodsId);
    void recoverStoreCache();
    void showListPart(GoodsList* l);//显示存货单详情
    void showStoreCacheCode(QStringList codes);//显示已存入条码

    QTimer time_test;
    bool loginState;
    bool needScanAll;
    GOODS_STATE curState;
    GoodsManager* manager;
    StoreListManager* storeManager;
    CabinetConfig* config;
    GoodsList* list_store;
    QStringList list_store_cache;
    QStringList list_part;
    QStringList list_msg;
    CabinetStoreListItem* bindItem;
    QList<CabinetStoreListItem*> list_item;
    QMap<QString, CabinetStoreListItem*> map_item;
private slots:
//    void timeOut();
    void itemBind(Goods*, CabinetStoreListItem*);
    void goodsTraceTimeout();

    void on_ok_clicked();

    void on_back_clicked();

    void on_listCache_cellClicked(int row, int column);

    void on_back_2_clicked();

    void on_stackedWidget_currentChanged(int arg1);

    void on_list_part_cellClicked(int row, int column);

signals:
    void requireBind(Goods*);
    void requireOpenCase(int seq, int index);
    void storeList(QList<CabinetStoreListItem*>);
    void requireScanState(bool);
    void requireGoodsListCheck(QString listCode);
    void newStoreBarCode(QString);
    void reportTraceId(QString);
//    void storeList(QStringList);
};

#endif // CABINETSTORELIST_H
