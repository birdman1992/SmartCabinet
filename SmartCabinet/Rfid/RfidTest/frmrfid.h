#ifndef FRMRFID_H
#define FRMRFID_H

#include <QWidget>

namespace Ui {
class FrmRfid;
}

class FrmRfid : public QWidget
{
    Q_OBJECT

public:
    explicit FrmRfid(QWidget *parent = 0);
    ~FrmRfid();

private:
    Ui::FrmRfid *ui;
};

#endif // FRMRFID_H
