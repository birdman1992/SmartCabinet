#include "standbywidget.h"
#include "ui_standbywidget.h"

StandbyWidget::StandbyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StandbyWidget)
{
    ui->setupUi(this);
}

StandbyWidget::~StandbyWidget()
{
    delete ui;
}
