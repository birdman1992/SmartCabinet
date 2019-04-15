#ifndef STROEBYITEM_H
#define STROEBYITEM_H

#include <QWidget>
#include "Structs/goodslist.h"
#include "cabinetconfig.h"

namespace Ui {
class StroeByItem;
}

class StroeByItem : public QWidget
{
    Q_OBJECT

public:
    explicit StroeByItem(QWidget *parent = 0);
    ~StroeByItem();

public slots:
    void newStoreList(GoodsList*);

signals:

private:
    Ui::StroeByItem *ui;
    GoodsList* curList;
    CabinetConfig* config;

    void updateStoreList();

private slots:

};

#endif // STROEBYITEM_H
