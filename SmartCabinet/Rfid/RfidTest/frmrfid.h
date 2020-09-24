#ifndef FRMRFID_H
#define FRMRFID_H

#include <QWidget>
#include <QCloseEvent>
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
    void clearCurOperation();
    ~FrmRfid();

public slots:
    void updateScanTimer(int ms);
    void updateCount(EpcMark mark, int count);
    void updateCurUser(QString optId);
    void scanProgress(int curCount, int totalCount);
    void updateLockCount(int lockCount);
    void showConfigDevice();
    void showEpcInfo();

signals:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

private slots:
    void showOperation();
    void updateAntInCount(int count);
    void updateOperationStr(QString optStr);
//    void updateEpcInfo(EpcInfo*);
    void accessSuccess(QString msg);
    void accessFailed(QString msg);
    void clearCountText();
    void on_scan_clicked();
    void on_stop_clicked();
    void on_OK_clicked();
    void on_fresh_clicked();
    void on_close_clicked();
    void on_tab_filter_all_toggled(bool checked);
    void on_tab_filter_new_toggled(bool checked);
    void on_tab_filter_back_toggled(bool checked);
    void on_tab_filter_consume_toggled(bool checked);
    void on_tab_filter_in_toggled(bool checked);
    void on_tab_filter_unknow_toggled(bool checked);
    void on_tab_filter_wait_back_toggled(bool checked);
    void on_tab_view_clicked(const QModelIndex &index);
    void on_tab_filter_out_toggled(bool checked);
    void on_stop_scan_clicked();
    void on_close_2_clicked();
    void on_add_device_clicked();
    void on_rfidDevView_clicked(const QModelIndex &index);
    void on_sig_add_clicked();
    void on_sig_minus_clicked();
    void on_ant_pow_valueChanged(int value);
    void on_conf_minus_clicked();
    void on_conf_add_clicked();
    void on_conf_int_valueChanged(int value);
    void on_grad_minus_clicked();
    void on_grad_add_clicked();
    void on_grad_thre_valueChanged(int value);
    void on_dev_type_toggled(bool checked);
    void on_operation_clicked(bool checked);

private:
    Ui::FrmRfid *ui;
    RfidManager* rfManager;
    QSortFilterProxyModel* filterModel;
    QTabWidget* win_tabs;
    EpcModel* eModel;
    bool isLogin;
    QBitArray visibleFlag;//控制各个按钮是否可见,下标为EpcMark
    QStringList list_win_name;
    QString curSelRfidReader;
    QString curOperation;
    QMap<EpcMark, QToolButton*> btnTable;
    void initTabs();
    void setDefaultSel();//设置默认选项
    void updateSelReader(QString devIp);

protected:
    void closeEvent(QCloseEvent*);
    void showEvent(QShowEvent *);
    void paintEvent(QPaintEvent*);
};


#endif // FRMRFID_H
