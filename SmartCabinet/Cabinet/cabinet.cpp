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

}

Cabinet::~Cabinet()
{
    delete ui;
}

void Cabinet::CabinetInit(int seq, int pos, int num, bool mainCab)
{
    seqNum = seq;
    posNum = pos;
//    caseNum = num;
    isMainCabinet = mainCab;

    if(!isMainCabinet)
    {
        cabType = 0;
        caseNum = CAB_CASE_0_NUM;
        ui->logo->hide();
        ui->tableWidget->setRowCount(caseNum);
        return;
    }
    else
    {
        cabType = 1;
        caseNum = CAB_CASE_1_NUM;
        ui->tableWidget->setRowCount(caseNum);
        logo = new QLabel(this);
        logo->installEventFilter(this);
        ui->logo->hide();
        ui->tableWidget->setCellWidget(1,0,logo);
    }
}

void Cabinet::setCabPos(int pos)
{
    posNum = pos;
}

void Cabinet::setCabType(int _type)
{
    cabType = _type;
}

void Cabinet::addCase(CabinetInfo *info)
{
    if(list_case.count()>=caseNum)
    {
        qDebug()<<"[addCase]"<<"case is full"<<seqNum<<caseNum;
        return;
    }

    int index = list_case.count();
    list_case<<info;
    if(info->name.isEmpty())
    {
        ui->tableWidget->setItem(index,0,new QTableWidgetItem(info->name));
    }
    else
    {
        ui->tableWidget->setItem(index,0,new QTableWidgetItem(info->name+QString("×%1").arg(info->num)));
    }

//    if(info->name.isEmpty())
//        setCaseState(index,3);
//    else
//    {
        if(info->num == 0)
            setCaseState(index,2);
//    }
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

void Cabinet::consumableIn(int index, int num)
{
    if(index >= list_case.count())
        return;

    if(list_case.at(index)->num == 0)
        setCaseState(index, 0);

    list_case.at(index)->num+=num;

    ui->tableWidget->item(index,0)->setText(list_case.at(index)->name+QString("×%1").arg(list_case.at(index)->num));

    QSettings settings(CONF_CABINET, QSettings::IniFormat);
    settings.beginWriteArray(QString("Cabinet%1").arg(seqNum));
    settings.setArrayIndex(index);
    settings.setValue("num",list_case.at(index)->num);
    settings.endArray();
}

void Cabinet::consumableOut(int index,int num)
{
    if(index >= list_case.count())
        return;

    if(list_case.at(index)->num == 0)
        return;

    list_case.at(index)->num = ((list_case.at(index)->num-num)<0)?0:list_case.at(index)->num-num;

    ui->tableWidget->item(index,0)->setText(list_case.at(index)->name+QString("×%1").arg(list_case.at(index)->num));

    QSettings settings(CONF_CABINET, QSettings::IniFormat);
    settings.beginWriteArray(QString("Cabinet%1").arg(seqNum));
    settings.setArrayIndex(index);
    settings.setValue("num",list_case.at(index)->num);
    settings.endArray();
    if(list_case.at(index)->num == 0)
        setCaseState(index, 2);
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
    logo->setText(msg);
    if(showBigCharacter)
        logo->setStyleSheet("background-color: rgb(85, 170, 255);font: 18pt \"Sans Serif\";");
    else
        logo->setStyleSheet("background-color: rgb(85, 170, 255);font: 9pt \"Sans Serif\";");
}

void Cabinet::setCaseName(CabinetInfo info, int index)
{
    QSettings settings(CONF_CABINET, QSettings::IniFormat);
    settings.beginWriteArray(QString("Cabinet%1").arg(seqNum));
    settings.setArrayIndex(index);
    settings.setValue("name",info.name);
    settings.setValue("id",info.id);
    settings.setValue("unit",info.unit);

    list_case.at(index)->name = info.name;
    list_case.at(index)->id = info.id;
    list_case.at(index)->unit = info.unit;
    ui->tableWidget->item(index,0)->setText(info.name+QString("×%1").arg(list_case.at(index)->num));
    ui->tableWidget->item(index,0)->setBackgroundColor(QColor(0, 170, 127));
}

bool Cabinet::isInLeft()
{
    return posNum%2;
}

void Cabinet::on_tableWidget_cellClicked(int row, int)
{
    emit caseSelect(row, seqNum);
}

void Cabinet::caseDraw(int _type)
{
    int baseHeight = this->geometry().height()/10;
    if(_type == 0)//副柜
    {
        if(ui->tableWidget->rowCount() != CAB_CASE_0_NUM)
            ui->tableWidget->setRowCount(CAB_CASE_0_NUM);

        ui->tableWidget->setColumnWidth(0,this->geometry().width());
        ui->tableWidget->setRowHeight(0,baseHeight*3);
        ui->tableWidget->setRowHeight(1,baseHeight*1);
        ui->tableWidget->setRowHeight(2,baseHeight*1);
        ui->tableWidget->setRowHeight(3,baseHeight*1);
        ui->tableWidget->setRowHeight(4,baseHeight*1);
        ui->tableWidget->setRowHeight(5,baseHeight*1);
        ui->tableWidget->setRowHeight(6,baseHeight*1);
        ui->tableWidget->setRowHeight(7,baseHeight*1);
    }
    else if(_type == 1)//单列主柜
    {
        if(ui->tableWidget->rowCount() != CAB_CASE_1_NUM)
            ui->tableWidget->setRowCount(CAB_CASE_1_NUM);

        ui->tableWidget->setColumnWidth(0,this->geometry().width());
        ui->tableWidget->setRowHeight(0,baseHeight*3);
        ui->tableWidget->setRowHeight(1,baseHeight*3);
        ui->tableWidget->setRowHeight(2,baseHeight*1);
        ui->tableWidget->setRowHeight(3,baseHeight*1);
        ui->tableWidget->setRowHeight(4,baseHeight*1);
        ui->tableWidget->setRowHeight(5,baseHeight*1);
        logo->resize(ui->tableWidget->columnWidth(0), ui->tableWidget->rowHeight(1));
    }
}

void Cabinet::setCaseState(int index, int numState)
{
    if(index > list_case.count())
        return;

    if(numState == 0)//库存充足
    {
        ui->tableWidget->item(index,0)->setBackgroundColor(QColor(0, 170, 127));
    }
    else if(numState == 1)//库存不足
    {
        ui->tableWidget->item(index,0)->setBackgroundColor(QColor(255, 170, 0));
    }
    else if(numState == 2)
    {
        ui->tableWidget->item(index,0)->setBackgroundColor(QColor(238, 128, 61));
    }
    else if(numState == 3)//空柜格
    {
        ui->tableWidget->item(index,0)->setBackgroundColor(QColor(255, 255, 255));
    }
}

bool Cabinet::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == logo)
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

void Cabinet::resizeEvent(QResizeEvent*)
{
    caseDraw(cabType);
}

void Cabinet::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
