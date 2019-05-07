#ifndef STROEBYITEM_H
#define STROEBYITEM_H

#include <QWidget>
#include "Structs/goodslist.h"
#include "cabinetstorelistitem.h"
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
//    void recvScanInfo(QByteArray);
//    void recvMsg(QString);

signals:
    void requireOpenCase(int,int);
    void requireBind(Goods*);

private:
    Ui::StroeByItem *ui;
    GoodsList* curList;
    CabinetConfig* config;
    CabinetStoreListItem* bindItem;
    QList<CabinetStoreListItem*> list_item;

    void updateStoreList();

    void clearList();
private slots:
    void itemBind(Goods *goods, CabinetStoreListItem *item);
};

#endif // STROEBYITEM_H
