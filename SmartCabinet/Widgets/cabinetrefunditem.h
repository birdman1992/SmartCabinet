#ifndef CABINETREFUNDITEM_H
#define CABINETREFUNDITEM_H

#include <QWidget>
#include <QPaintEvent>
#include "Structs/cabinetinfo.h"

namespace Ui {
class CabinetRefundItem;
}

class CabinetRefundItem : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetRefundItem(Goods* info, QWidget *parent = 0);//物品名，包id
    ~CabinetRefundItem();
    int itemNum();
    QString itemId();
    bool itemAdd();

private:
    Ui::CabinetRefundItem *ui;
    void paintEvent(QPaintEvent *);

    int num;
    int maxNum;
    QString package_id;
    QString name;
};

#endif // CABINETREFUNDITEM_H
