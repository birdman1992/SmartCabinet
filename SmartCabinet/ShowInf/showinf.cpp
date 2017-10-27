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

    ui->cabinet->setText("无");
    ui->num_2->setText("无");
    ui->num->setText("无");
    ui->name->setText("无");
    ui->exist->setText("请先扫码！！！");
    connect(ui->save,SIGNAL(clicked(bool)),this,SLOT(btn_save()));
    connect((ui->close),SIGNAL(clicked(bool)),this,SLOT(btn_close()));
    connect(ui->out,SIGNAL(clicked(bool)),this,SLOT(btn_out()));
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
        ui->cabinet->setText(QString::number(med.cab_num,10));
        ui->num_2->setText(QString::number(med.lat_num,10));
        ui->num->setText(QString::number(med.num,10));
    }
    else if(Exist == 0)
    {
        ui->exist->setText("该药品不存在药柜，请放入");
        ui->cabinet->setText(QString::number(med.cab_num,10));
        ui->num_2->setText(QString::number(med.lat_num,10));
        ui->num->setText(QString::number(med.num,10));
    }
}

void ShowInf::check_no_exist(MedInf med)
{
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
    med.num = ui->num->text().toInt() + 1;
    med.cab_num = ui->cabinet->text().toInt();
    med.lat_num = ui->num_2->text().toInt();
    med.exist = Exist;
    med.name = ui->name->text();
    med.application = "application" ;
    med.ShelfLife = "ShelfLife" ;
    med.ProductionDate = "ProductionDate" ;
    med.Features = "Features" ;
    emit cabinet_inf(med);
}

void ShowInf::btn_out()
{
    MedInf med;
    med.num = ui->num->text().toInt() - 1;
    med.cab_num = ui->cabinet->text().toInt();
    med.lat_num = ui->num_2->text().toInt();
    med.exist = 1;
    med.name = ui->name->text();
    med.application = "application" ;
    med.ShelfLife = "ShelfLife" ;
    med.ProductionDate = "ProductionDate" ;
    med.Features = "Features" ;
    emit cabinet_inf(med);
}
