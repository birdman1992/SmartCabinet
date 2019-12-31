#ifndef FRMRFID_H
#define FRMRFID_H

#include <QWidget>
#include "rfidmanager.h"

namespace Ui {
class FrmRfid;
}

class FrmRfid : public QWidget
{
    Q_OBJECT

public:
    explicit FrmRfid(QWidget *parent = 0);
    ~FrmRfid();

private slots:
    void updateEpcInfo(EpcInfo*);
    void on_scan_clicked();
    void on_stop_clicked();

private:
    Ui::FrmRfid *ui;
    RfidManager* rfManager;
    void updateTableRow(int rowIndex, EpcInfo*);
};

#endif // FRMRFID_H
