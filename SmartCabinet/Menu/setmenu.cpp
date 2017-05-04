#include "setmenu.h"
#include "ui_setmenu.h"
#include <QMessageBox>

SetMenu::SetMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetMenu)
{
    ui->setupUi(this);
    count_down = NULL;
    user = NULL;

    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(btn_in()));
    connect(ui->adduser,SIGNAL(clicked(bool)),this,SLOT(btn_add()));
}

void SetMenu::btn_in()
{
    count_down = new CountDown;
    count_down->show();
}

void SetMenu::count_close()
{
    if(count_down != NULL)
    count_down->close();
    if(user != NULL)
    {
        user->close();
    }
}

void SetMenu::btn_add()
{
//    user = new PrimaryUser;
//    user->show();
}

SetMenu::~SetMenu()
{
    delete ui;
    delete count_down;
}

