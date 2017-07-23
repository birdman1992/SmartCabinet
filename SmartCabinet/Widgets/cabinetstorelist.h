#ifndef CABINETSTORELIST_H
#define CABINETSTORELIST_H

#include <QWidget>
#include <QList>
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
    void storeFinish();
    void bindRst(CaseAddress addr);
    void storeRst(QString msg, bool success);

    bool installGlobalConfig(CabinetConfig *globalConfig);
private:
    Ui::CabinetStoreList *ui;
    void paintEvent(QPaintEvent *);
    void clearList();

    CabinetConfig* config;
    GoodsList* list_store;
    CabinetStoreListItem* bindItem;
    QList<CabinetStoreListItem*> list_item;
private slots:
    void itemBind(Goods*, CabinetStoreListItem*);

    void on_ok_clicked();

    void on_back_clicked();

signals:
    void requireBind(Goods*);
    void requireOpenCase(int seq, int index);
    void storeList(QList<CabinetStoreListItem*>);
};

#endif // CABINETSTORELIST_H