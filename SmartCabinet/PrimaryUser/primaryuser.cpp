#include "primaryuser.h"
#include "ui_primaryuser.h"
#include <QMessageBox>

PrimaryUser::PrimaryUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrimaryUser)
{
    ui->setupUi(this);
    connect(ui->quit,SIGNAL(clicked(bool)),this,SLOT(btn_quit()));
    connect(ui->sure,SIGNAL(clicked(bool)),this,SLOT(btn_sure()));
}

void PrimaryUser::btn_sure()
{
    UserInf user;
    user.name = ui->name->text();
    user.authority = ui->authority->text().toInt();
    emit new_pri_user(user);
    QMessageBox msgBox;
    msgBox.setText("保存成功.");
    msgBox.exec();
}

void PrimaryUser::btn_quit()
{
    this->close();
}

void PrimaryUser::scan_user_inf(QByteArray qb)
{
    QString name = qb;
    ui->name->setText(name);
    ui->authority->setText("0");
}

PrimaryUser::~PrimaryUser()
{
    delete ui;
}
