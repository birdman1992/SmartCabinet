#include "setmenu.h"
#include "ui_setmenu.h"

SetMenu::SetMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetMenu)
{
    ui->setupUi(this);
}

SetMenu::~SetMenu()
{
    delete ui;
}
