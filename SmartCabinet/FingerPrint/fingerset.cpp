#include "fingerset.h"
#include "ui_fingerset.h"

FingerSet::FingerSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FingerSet)
{
    ui->setupUi(this);
}

FingerSet::~FingerSet()
{
    delete ui;
}
