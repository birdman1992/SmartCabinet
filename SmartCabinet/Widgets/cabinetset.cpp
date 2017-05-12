#include "cabinetset.h"
#include "ui_cabinetset.h"
#include "defines.h"
#include <QDebug>

CabinetSet::CabinetSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetSet)
{
    ui->setupUi(this);
    cabinet_pos.clear();
    cabinet_pos[0] = 0;
    list_cabinet<< ui->label_0 << ui->label_1<< ui->label_2 << ui->label_3 << ui->label_4;

    ui->label_1->hide();
    ui->label_2->hide();
    ui->label_3->hide();
    ui->label_4->hide();
}

CabinetSet::~CabinetSet()
{
    delete ui;
}

bool CabinetSet::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    return true;
}

void CabinetSet::on_add_left_clicked()
{
    int i;
    int j;

    for(i=1 ;i<list_cabinet.count(); i+=2)
    {
        if(list_cabinet.at(i)->isHidden())
        {
            j = 1;
            while(cabinet_pos[j]) j++;
            cabinet_pos[j] = i;
            list_cabinet.at(i)->show();
            break;
        }
    }
    if((i+2)>=list_cabinet.count())
        ui->add_left->setEnabled(false);
}

void CabinetSet::on_add_right_clicked()
{
    int i;
    int j;

    for(i=2 ;i<list_cabinet.count(); i+=2)
    {
        if(list_cabinet.at(i)->isHidden())
        {
            j = 1;
            while(cabinet_pos[j]) j++;
            cabinet_pos[j] = i;
            list_cabinet.at(i)->show();
            break;
        }
    }
    if((i+2)>=list_cabinet.count())
        ui->add_right->setEnabled(false);
}

void CabinetSet::on_clear_clicked()
{
    cabinet_pos.clear();
    cabinet_pos[0] = 0;
    ui->label_1->hide();
    ui->label_2->hide();
    ui->label_3->hide();
    ui->label_4->hide();
    ui->add_left->setEnabled(true);
    ui->add_right->setEnabled(true);
}

void CabinetSet::on_save_clicked()
{
    qDebug()<<cabinet_pos.toHex();
    config->creatCabinetConfig(cabinet_pos);
    qDebug()<<"creat over";
//    emit setCabinet(cabinet_pos);
    emit cabinetCreated();
    emit winSwitch(INDEX_STANDBY);
}

void CabinetSet::on_cancel_clicked()
{
    cabinet_pos.clear();
    ui->label_1->hide();
    ui->label_2->hide();
    ui->label_3->hide();
    ui->label_4->hide();
    ui->add_left->setEnabled(true);
    ui->add_right->setEnabled(true);

    emit winSwitch(INDEX_STANDBY);
}
