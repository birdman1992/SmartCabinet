#include "showinf.h"
#include "ui_showinf.h"

ShowInf::ShowInf(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowInf)
{
    ui->setupUi(this);
    ui->num->setText("");

    ui->add->setValue(0);
    ui->cabinet->setValue(0);
    ui->lattice->setValue(0);

    connect(ui->save,SIGNAL(clicked(bool)),this,SLOT(btn_save()));
    connect((ui->close),SIGNAL(clicked(bool)),this,SLOT(btn_close()));
}

ShowInf::~ShowInf()
{
    delete ui;
}

void ShowInf::check_exist(MedInf med)
{
    ui->cabinet->setEnabled(false);
    ui->lattice->setEnabled(true);
    ui->name->setText(med.name);
}

void ShowInf::check_no_exist(MedInf med)
{
    ui->name->setText(med.name);
}

void ShowInf::btn_close()
{
    this->close();
}

void ShowInf::btn_save()
{
    cabinet_num = ui->cabinet->text().toInt();
    lattice_num = ui->lattice->text().toInt();
    add_num = ui->add->text().toInt();
    emit cabinet_inf(cabinet_num,lattice_num,add_num);
    this->close();
}
