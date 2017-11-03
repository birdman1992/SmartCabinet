#ifndef CABINETCHECKITEM_H
#define CABINETCHECKITEM_H

#include <QWidget>
#include <QPaintEvent>
#include "Structs/cabinetinfo.h"

namespace Ui {
class CabinetCheckItem;
}

class CabinetCheckItem : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetCheckItem(GoodsInfo* info, QWidget *parent = 0);//物品名，包id
    ~CabinetCheckItem();
    int itemNum();
    QString itemId();
    bool itemAdd();
    QStringList list_fullId;

private slots:
//    void on_minus_10_clicked();

    void on_minus_clicked();

    void on_add_clicked();

//    void on_add_10_clicked();

private:
    Ui::CabinetCheckItem *ui;
    void paintEvent(QPaintEvent *);

    int num;
    int maxNum;
    QString package_id;
    QString name;
};

#endif // CABINETCHECKITEM_H
