#ifndef CABINETLISTVIEW_H
#define CABINETLISTVIEW_H

#include <QWidget>
#include <QPaintEvent>
#include <QFrame>
#include <QList>
#include <QShowEvent>
#include <QModelIndex>
#include <qmap.h>
#include <QButtonGroup>
#include "Structs/caseaddress.h"
#include "cabinetconfig.h"
#include "Structs/cabinetinfo.h"
#include "Structs/caseaddress.h"
#include "Widgets/cabinetlistitem.h"

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
    void fetchSuccess();
    void fetchFailed(QString msg);
    void setNetState(bool state);

    bool installGlobalConfig(CabinetConfig *globalConfig);
private slots:
    void on_back_clicked();
    void on_list_goods_clicked(const QModelIndex &index);
    void on_fetch_clicked();
    void search(int);

    void on_searchClear_clicked();

public slots:
    void recvScanData(QByteArray qba);

    void show();
signals:
    void requireAccessList(QStringList list, int optType);
    void requireOpenCase(int seqNum, int index);

private:
    Ui::CabinetListView *ui;
    bool networkState;
    QFrame* cabFrame;
    QButtonGroup groupSearch;
    QMap<QString, CabinetListItem*> selectMap;
    CabinetConfig* config;
    QList<Goods*> list_goods;
    QList<Goods*> list_filted;//过滤后物品列表

    void paintEvent(QPaintEvent *);
    void showEvent(QShowEvent* );
    void initButtons();
    void showCabView();
    void clearList();
    void getCabList();
    void updateCabList(QString filter=" ");
    bool packIsSelected(QString packId);
    QString scanDataTrans(QString code);
};

#endif // CABINETLISTVIEW_H
