#include "cabinetlistitem.h"
#include "ui_cabinetlistitem.h"

CabinetListItem::CabinetListItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetListItem)
{
    ui->setupUi(this);
}

CabinetListItem::~CabinetListItem()
{
    delete ui;
}
