#include "cabinetpanel.h"
#include "ui_cabinetpanel.h"

CabinetPanel::CabinetPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetPanel)
{
    ui->setupUi(this);
}

CabinetPanel::~CabinetPanel()
{
    delete ui;
}
