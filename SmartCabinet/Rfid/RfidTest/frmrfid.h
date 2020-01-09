#ifndef FRMRFID_H
#define FRMRFID_H

#include <QWidget>
#include <QTableWidget>
#include <QTabWidget>
#include <QList>
#include <QStringList>
#include <QTimer>
#include <QPaintEvent>
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
    ~FrmRfid();

private slots:
    void testSlot();
    void updateAntInCount(int count);
    void updateEpcInfo(EpcInfo*);
    void showTabs(TableMark tabMark);
    void accessSuccess(QString msg);
    void accessFailed(QString msg);
    void on_scan_clicked();
    void on_stop_clicked();
    void on_OK_clicked();
    void on_fresh_clicked();
    void on_pushButton_clicked();

private:
    Ui::FrmRfid *ui;
    RfidManager* rfManager;
    QTabWidget* win_tabs;
    bool isLogin;
    QStringList list_win_name;
    QList<QTableWidget*> tabs;
    void updateTableRow(int rowIndex, EpcInfo*);
    void initTabs();
    void showEvent(QShowEvent *);
    void paintEvent(QPaintEvent*);
};


#endif // FRMRFID_H
