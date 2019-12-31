#include "frmrfid.h"
#include "ui_frmrfid.h"

FrmRfid::FrmRfid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmRfid)
{
    ui->setupUi(this);
}

FrmRfid::~FrmRfid()
{
    delete ui;
}
