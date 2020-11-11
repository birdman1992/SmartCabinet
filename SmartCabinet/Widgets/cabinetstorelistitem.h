#ifndef CABINETSTORELISTITEM_H
#define CABINETSTORELISTITEM_H

#include <QWidget>
#include <QPaintEvent>
#include "Structs/goodslist.h"
#include "Structs/caseaddress.h"

namespace Ui {
class CabinetStoreListItem;
}

class CabinetStoreListItem : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetStoreListItem(Goods* goods,CaseAddress addr, QWidget *parent = 0);
    explicit CabinetStoreListItem(Goods* goods,QPoint addr, QWidget *parent = 0);
    ~CabinetStoreListItem();
    void bindRst(CaseAddress addr);
    QString itemId();
    int itemNum();
    int waitNum();
    QPoint itemPos();
    bool samePos(QPoint pos);
    void storeOnePac();
    Goods* itemGoods();
    void infoUpdate();

private slots:
    void on_minus_clicked();
    void on_add_clicked();
    void on_opt_clicked();

signals:
    void requireBind(Goods*, CabinetStoreListItem*);
    void requireOpenCase(int seq, int index);

private:
    Ui::CabinetStoreListItem *ui;
    int num;
    QString name;
    int pos_seq;
    int pos_index;
    QString package_id;
    Goods* cabGoods;

    void paintEvent(QPaintEvent *);
    void updateOptState();
    void setStoreState(bool storeOver);//所有物品存放完毕

};

#endif // CABINETSTORELISTITEM_H
