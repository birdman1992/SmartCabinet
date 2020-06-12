#ifndef FRMRFID_H
#define FRMRFID_H

#include <QWidget>
#include <QTableWidget>
#include <QTabWidget>
#include <QList>
#include <QStringList>
#include <QTimer>
#include <QPaintEvent>
#include <QHeaderView>
#include <QTableView>
#include <QToolButton>
#include <QBitArray>
#include <QSortFilterProxyModel>
#include "Rfid/RfidTest/epcmodel.h"
#include "rfidmanager.h"

namespace Ui {
class FrmRfid;
}

class FrmRfid : public QWidget
{
    Q_OBJECT

public:
    explicit FrmRfid(QWidget *parent = 0);
    void setLoginState(bool login);
    void setPow(int pow);
    ~FrmRfid();

public slots:
    void updateScanTimer(int ms);
    void updateCount(EpcMark mark, int count);
    void updateCurUser(QString optId);
    void scanProgress(int curCount, int totalCount);
    void updateLockCount(int lockCount);

private slots:
    void testSlot();
    void updateAntInCount(int count);
    void updateEpcInfo(EpcInfo*);
    void showTabs(TableMark tabMark);
    void accessSuccess(QString msg);
    void accessFailed(QString msg);
    void clearCountText();
    void on_scan_clicked();
    void on_stop_clicked();
    void on_OK_clicked();
    void on_fresh_clicked();
    void on_pushButton_clicked();
    void on_tab_filter_all_toggled(bool checked);
    void on_tab_filter_out_toggled(bool checked);
    void on_tab_filter_new_toggled(bool checked);
    void on_tab_filter_back_toggled(bool checked);
    void on_tab_filter_consume_toggled(bool checked);
    void on_tab_filter_in_toggled(bool checked);
    void on_tab_filter_unknow_toggled(bool checked);

    void on_tab_filter_wait_back_toggled(bool checked);

    void on_tab_view_clicked(const QModelIndex &index);

private:
    Ui::FrmRfid *ui;
    RfidManager* rfManager;
    QSortFilterProxyModel* filterModel;
    QTabWidget* win_tabs;
    EpcModel* eModel;
    bool isLogin;
    QBitArray visibleFlag;//控制各个按钮是否可见,下标为EpcMark
    QStringList list_win_name;
    QList<QTableView*> tabs;
//    QList<QToolButton*> btnTable;
    QMap<EpcMark, QToolButton*> btnTable;
    void updateTableRow(int rowIndex, EpcInfo*);
    void initTabs();
    void setDefaultSel();//设置默认选项
    void showEvent(QShowEvent *);
    void paintEvent(QPaintEvent*);
};


#endif // FRMRFID_H
