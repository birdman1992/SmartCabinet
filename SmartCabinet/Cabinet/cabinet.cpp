#include "cabinet.h"
#include "ui_cabinet.h"
#include <QColor>
#include <QDebug>
#include <QSettings>
#include <QTableWidgetItem>
#include "defines.h"

Cabinet::Cabinet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Cabinet)
{
    ui->setupUi(this);
    ui->logo->installEventFilter(this);
}

Cabinet::~Cabinet()
{
    delete ui;
}

void Cabinet::CabinetInit(int seq, int pos, int num, bool mainCab)
{
    seqNum = seq;
    posNum = pos;
    caseNum = num;
    isMainCabinet = mainCab;

    if(!isMainCabinet)
    {
        ui->logo->hide();
        ui->tableWidget->setRowCount(caseNum);
        return;
    }

    ui->tableWidget->setRowCount(caseNum-2);
}

void Cabinet::setCabPos(int pos)
{
    posNum = pos;
}

void Cabinet::addCase(CabinetInfo *info)
{
    if(list_case.count()>=caseNum)
    {
        qDebug()<<"[addCase]"<<"case is full";
        return;
    }

    int index = list_case.count();
    list_case<<info;
    ui->tableWidget->setItem(index,0,new QTableWidgetItem(info->name));

    if(!info->name.isEmpty())
        ui->tableWidget->item(index,0)->setBackgroundColor(QColor(0, 170, 127));
}

int Cabinet::getIndexByName(QString findName)
{
    int i = 0;

    for(i=0; i<list_case.count(); i++)
    {
        if(findName == list_case.at(i)->name)
            return i;
    }

    return -1;
}

void Cabinet::consumableIn(int index)
{
    if(index >= list_case.count())
        return;
    list_case.at(index)->num++;

    QSettings settings(CONF_CABINET, QSettings::IniFormat);
    settings.beginWriteArray(QString("Cabinet%1").arg(seqNum));
    settings.setArrayIndex(index);
    settings.setValue("num",list_case.at(index)->num);
    settings.endArray();
}

void Cabinet::consumableOut(int index)
{
    if(index >= list_case.count())
        return;
    list_case.at(index)->num--;

    QSettings settings(CONF_CABINET, QSettings::IniFormat);
    settings.beginWriteArray(QString("Cabinet%1").arg(seqNum));
    settings.setArrayIndex(index);
    settings.setValue("num",list_case.at(index)->num);
    settings.endArray();
}

int Cabinet::cabinetPosNum()
{
    return posNum;
}

void Cabinet::clearSelectState(int row)
{
    ui->tableWidget->setItemSelected(ui->tableWidget->item(row,0), false);
}

void Cabinet::showMsg(QString msg, bool showBigCharacter)
{
    ui->logo->setText(msg);
    if(showBigCharacter)
        ui->logo->setStyleSheet("background-color: rgb(85, 170, 255);font: 18pt \"Sans Serif\";");
    else
        ui->logo->setStyleSheet("background-color: rgb(85, 170, 255);font: 9pt \"Sans Serif\";");
}

void Cabinet::setCaseName(QString name, int index)
{
    QSettings settings(CONF_CABINET, QSettings::IniFormat);
    settings.beginWriteArray(QString("Cabinet%1").arg(seqNum));
    settings.setArrayIndex(index);
    settings.setValue("name",name);
    settings.endArray();

    list_case.at(index)->name = name;
    ui->tableWidget->item(index,0)->setText(name);
    ui->tableWidget->item(index,0)->setBackgroundColor(QColor(0, 170, 127));
}

void Cabinet::on_tableWidget_cellClicked(int row, int)
{
    emit caseSelect(row, seqNum);
}

bool Cabinet::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->logo)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            emit logoClicked();
        }
        else
        {
            return false;
        }
    }

    // pass the event on to the parent class
    return QWidget::eventFilter(obj, event);
}
