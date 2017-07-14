#ifndef CABINETLISTVIEW_H
#define CABINETLISTVIEW_H

#include <QWidget>
#include <QPaintEvent>
#include <QFrame>
#include <QList>
#include <QShowEvent>
#include "cabinetconfig.h"
#include "Structs/cabinetinfo.h"
#include "Structs/caseaddress.h"

namespace Ui {
class CabinetListView;
}

class CabinetListView : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetListView(QWidget *parent = 0);
    ~CabinetListView();
    void setCabView(QFrame* cab);

    bool installGlobalConfig(CabinetConfig *globalConfig);
private slots:

    void on_back_clicked();

private:
    Ui::CabinetListView *ui;
    QFrame* cabFrame;
    CabinetConfig* config;
    QList<GoodsInfo*> list_goods;
    QList<GoodsInfo*> list_filted;//过滤后物品列表

    void paintEvent(QPaintEvent *);
    void showEvent(QShowEvent* );
    void showCabView();
    void clearList();
    void getCabList();
    void updateCabList(QChar filter=' ');
};

#endif // CABINETLISTVIEW_H
