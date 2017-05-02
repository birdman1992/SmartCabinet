#include "showinf.h"
#include "ui_showinf.h"

ShowInf::ShowInf(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowInf)
{
    ui->setupUi(this);
}

ShowInf::~ShowInf()
{
    delete ui;
}
