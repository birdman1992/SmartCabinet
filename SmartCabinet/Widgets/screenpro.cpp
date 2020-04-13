#include "screenpro.h"
#include "ui_screenpro.h"

ScreenPro::ScreenPro(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScreenPro)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->label_2->hide();
}

ScreenPro::~ScreenPro()
{
    delete ui;
}

void ScreenPro::updateProState(bool open)
{
    if(open)
        this->showFullScreen();
    else
        this->hide();
}
