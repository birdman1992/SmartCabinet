#ifndef CABINETACCESS_H
#define CABINETACCESS_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include "Structs/goodslist.h"

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
    void scanOpen(QString goodsId);

private:
    Ui::CabinetAccess *ui;
    bool isStore;
    GoodsList* storeList;
    void paintEvent(QPaintEvent *);
};

#endif // CABINETACCESS_H
