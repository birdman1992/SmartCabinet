#ifndef CABINETACCESS_H
#define CABINETACCESS_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include "Structs/goodslist.h"
#include "Structs/cabinetinfo.h"
#include "Structs/caseaddress.h"
#include "cabinetconfig.h"
#include <Widgets/numkeyboard.h>

namespace Ui {
class CabinetAccess;
}

class CabinetAccess : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetAccess(QWidget *parent = 0);
    ~CabinetAccess();
//    accessInit(bool store, );
    void setNetworkState(bool state);
    void setAccessModel(bool store);//true:存 false:取
    void setStoreList(GoodsList* list);
    void clickOpen(QString goodsId);
    void clickOpen(CabinetInfo* info);
    void scanOpen(QString goodsId, QString goodsCode=QString());
    void save();
    void storeFailed(QString msg);
    void fetchFailed(QString msg);
    void showTips(QString msg, bool isDangerous);

    bool installGlobalConfig(CabinetConfig *globalConfig);
private slots:
    void on_onekey_clicked();
    void on_cancel_clicked();
    void on_ok_clicked();
    void input(int);
    void backspace();
    void clearAll();

public slots:
    void recvOptGoodsNum(int num);//接收操作物品数量
    void show();
    void setPrice(float single, float total);

signals:
    void saveStore(Goods* goods,int num);
    void saveFetch(QString,int);

private:
    Ui::CabinetAccess *ui;
    bool isStore;
    bool defaultValue;
    bool networkState;
    CabinetConfig* config;
    GoodsList* storeList;
    Goods* storeGoods;
    Goods* curGoods;
    CabinetInfo* curCab;
    NumKeyboard* keyBoard;
    QString strInput;
    CaseAddress addr;
    void paintEvent(QPaintEvent *);
    void showEvent(QShowEvent*);
};

#endif // CABINETACCESS_H
