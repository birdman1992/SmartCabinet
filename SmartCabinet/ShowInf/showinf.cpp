#include "showinf.h"
#include "ui_showinf.h"
#include <QDebug>
ShowInf::ShowInf(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowInf)
{
    ui->setupUi(this);

//    count_down = new CountDown;
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
    ui->name->setText(med.name);
    qDebug()<<"exist  :"<<Exist;
    Exist = med.exist;
    if(Exist == 1)
    {
        ui->exist->setText("该药品存在药柜");
        ui->cabinet->setValue(med.cab_num);
        ui->lattice->setValue(med.lat_num);
    }
    ui->cabinet->setEnabled(false);
    ui->lattice->setEnabled(false);
//    count_down->close();
}

void ShowInf::check_no_exist(MedInf med)
{
    ui->cabinet->setEnabled(true);
    ui->lattice->setEnabled(true);
    ui->name->setText(med.name);
        qDebug()<<"exist  :"<<med.exist;
        Exist = med.exist;
    if(med.exist == 0)
    {
        ui->exist->setText("该药品不存在药柜");
    }
}

void ShowInf::btn_close()
{
    this->close();
}

void ShowInf::btn_save()
{
    MedInf med;
    med.num = ui->lattice->value();
    med.cab_num = ui->cabinet->value();
    med.lat_num = ui->lattice->value();
    med.exist = Exist;
    med.name = ui->name->text();
    med.application = "application" ;
    med.ShelfLife = "ShelfLife" ;
    med.ProductionDate = "ProductionDate" ;
    med.Features = "Features" ;
    emit cabinet_inf(med);
    this->close();
}
