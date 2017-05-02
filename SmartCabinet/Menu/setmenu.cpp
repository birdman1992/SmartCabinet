#include "setmenu.h"
#include "ui_setmenu.h"
#include <QMessageBox>

SetMenu::SetMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetMenu)
{
    ui->setupUi(this);
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(btn_in()));
}

void SetMenu::btn_in()
{
    count_down = new CountDown;
    count_down->show();
}

SetMenu::~SetMenu()
{
    delete ui;
}

