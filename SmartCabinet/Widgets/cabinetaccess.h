#ifndef CABINETACCESS_H
#define CABINETACCESS_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include "Structs/goodslist.h"
#include "Structs/cabinetinfo.h"
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
    void setAccessModel(bool store);//true:存 false:取
    void setStoreList(GoodsList* list);
    void clickOpen(QString goodsId);
    void clickOpen(CabinetInfo* info);
    void scanOpen(QString goodsId);
    void save();

private slots:
    void on_onekey_clicked();
    void on_cancel_clicked();
    void on_ok_clicked();
    void input(int);
    void backspace();
    void clearAll();

signals:
    void saveStore(Goods* goods,int num);
    void saveFetch(QString,int);

private:
    Ui::CabinetAccess *ui;
    bool isStore;
    bool defaultValue;
    GoodsList* storeList;
    Goods* curGoods;
    CabinetInfo* curCab;
    NumKeyboard* keyBoard;
    QString strInput;
    void paintEvent(QPaintEvent *);
    void showEvent(QShowEvent*);
};

#endif // CABINETACCESS_H
