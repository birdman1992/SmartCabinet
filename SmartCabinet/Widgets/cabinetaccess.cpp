#include "cabinetaccess.h"
#include "ui_cabinetaccess.h"

CabinetAccess::CabinetAccess(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetAccess)
{
    ui->setupUi(this);
    isStore = false;//默认为取模式
}

CabinetAccess::~CabinetAccess()
{
    delete ui;
}

void CabinetAccess::setAccessModel(bool store)
{

}
