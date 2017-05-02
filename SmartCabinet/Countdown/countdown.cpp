#include "countdown.h"
#include "ui_countdown.h"

CountDown::CountDown(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CountDown)
{
    ui->setupUi(this);
}

CountDown::~CountDown()
{
    delete ui;
}
