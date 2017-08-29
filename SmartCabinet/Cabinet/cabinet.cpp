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

void Cabinet::CabinetInit(int _width, int seq, int pos, int, bool mainCab)
{
    caseWidth = _width;
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
        logo->setWordWrap(true);
        logo->setStyleSheet("background-color: rgb(85, 170, 255);font: 18pt \"Sans Serif\";");
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

void Cabinet::checkCase(int index)
{
    setCaseState(index, 2);
}

void Cabinet::addCase(GoodsInfo *info, int caseIndex)
{
    if(list_case.count()>=caseNum)
    {
        qDebug()<<"[addCase]"<<"case is full"<<seqNum<<caseNum;
        return;
    }
//    qDebug()<<"[addCase]"<<info->name<<info->num;
    info->outNum = 0;

    if(caseIndex<list_case.count())
    {
        list_case.at(caseIndex)->list_goods<<info;
        CasePanel* lab = (CasePanel*)ui->tableWidget->cellWidget(caseIndex,0);
//        QLabel* lab = (QLabel*)ui->tableWidget->cellWidget(caseIndex,0);
//        lab->setWordWrap(true);
        lab->setText(list_case.at(caseIndex)->list_goods);//->caseShowStr(lab->caseFont(), caseWidth/2));
//        ui->tableWidget->item(caseIndex,0)->setText(list_case.at(caseIndex)->caseShowStr());
    }
    else
    {
        CabinetInfo* cabInfo = new CabinetInfo();
        if(!info->name.isEmpty())
            cabInfo->list_goods<<info;
        list_case<<cabInfo;
        if(isMainCabinet && (caseIndex == 1))
            return;
        CasePanel* lab = new CasePanel();
        lab->setText(list_case.at(caseIndex)->list_goods);//->caseShowStr(lab->caseFont(), caseWidth/2));
//        QLabel* lab = new QLabel(cabInfo->caseShowStr());
//        lab->setWordWrap(true);
        ui->tableWidget->setCellWidget(caseIndex, 0, lab);
        setCaseState(caseIndex, 0);
//        ui->tableWidget->setItem(caseIndex,0,new QTableWidgetItem(cabInfo->caseShowStr()));
//        ui->tableWidget->item(caseIndex,0)->setBackground(QBrush(QColor(36, 0, 149)));
    }
}

void Cabinet::setCtrlWord(int caseIndex, QByteArray seq, QByteArray index)
{
//    qDebug()<<"setCtrlWord"<<caseIndex<<seq.toHex()<<index.toHex();
    if(caseIndex >= list_case.count())
        return;

    if(seq.size() <= caseIndex)
        list_case[caseIndex]->ctrlSeq = seqNum;
    else
        list_case[caseIndex]->ctrlSeq = ((int)seq[caseIndex]==0)?seqNum:seq[caseIndex];

    if(index.size() <= caseIndex)
        list_case[caseIndex]->ctrlIndex = caseIndex;
    else
        list_case[caseIndex]->ctrlIndex = ((int)index[caseIndex]==0)?caseIndex:index[caseIndex];
}

int Cabinet::getIndexByName(QString findName)
{
    int i = 0;

    for(i=0; i<list_case.count(); i++)
    {
//        if(findName == list_case.at(i)->name)
        if(list_case.at(i)->caseSearch(findName) != -1)
            return i;
    }

    return -1;
}

void Cabinet::consumableIn(CaseAddress addr, int num)
{
    if((addr.caseIndex >= list_case.count()) || (addr.cabinetSeqNum!=seqNum) || (addr.goodsIndex>=list_case.at(addr.caseIndex)->list_goods.count()))
        return;

//    if(list_case.at(index)->num == 0)
//    if(list_case.at(i)->caseSearch(findName) != -1)
//        setCaseState(index, 0);

    list_case.at(addr.caseIndex)->list_goods.at(addr.goodsIndex)->num+=num;
    CasePanel* lab = (CasePanel*)ui->tableWidget->cellWidget(addr.caseIndex,0);
    lab->setText(list_case.at(addr.caseIndex)->list_goods);
//    ui->tableWidget->item(addr.caseIndex,0)->setText(list_case.at(addr.caseIndex)->caseShowStr());

    QSettings settings(CONF_CABINET, QSettings::IniFormat);
    settings.beginGroup(QString("Cabinet%1").arg(seqNum));
    settings.beginWriteArray(QString("case%1").arg(addr.caseIndex));
    settings.setArrayIndex(addr.goodsIndex);qDebug()<<"[123]"<<addr.goodsIndex;
    settings.setValue("num",list_case.at(addr.caseIndex)->list_goods.at(addr.goodsIndex)->num);
    settings.endArray();
    settings.endGroup();
}

void Cabinet::consumableOut(CaseAddress addr,int num)
{qDebug("[consumableOut]");
    if((addr.caseIndex >= list_case.count()) || (addr.cabinetSeqNum!=seqNum) || (addr.goodsIndex>=list_case.at(addr.caseIndex)->list_goods.count()))
        return;

    if(list_case.at(addr.caseIndex)->list_goods.at(addr.goodsIndex)->num == 0)
        return;

    list_case.at(addr.caseIndex)->list_goods.at(addr.goodsIndex)->num = ((list_case.at(addr.caseIndex)->list_goods.at(addr.goodsIndex)->num-num)<0)?0:list_case.at(addr.caseIndex)->list_goods.at(addr.goodsIndex)->num-num;
    CasePanel* lab = (CasePanel*)ui->tableWidget->cellWidget(addr.caseIndex,0);
    lab->setText(list_case.at(addr.caseIndex)->list_goods);
    //    ui->tableWidget->item(addr.caseIndex,0)->setText(list_case.at(addr.caseIndex)->caseShowStr());

    QSettings settings(CONF_CABINET, QSettings::IniFormat);
    settings.beginGroup(QString("Cabinet%1").arg(seqNum));
    settings.beginWriteArray(QString("case%1").arg(addr.caseIndex));
    settings.setArrayIndex(addr.caseIndex);
    settings.setValue("num",list_case.at(addr.caseIndex)->list_goods.at(addr.goodsIndex)->num);
    settings.endArray();
    settings.endGroup();
//    if(list_case.at(addr.caseIndex)->num == 0)
    //        setCaseState(addr.caseIndex, 2);
}

void Cabinet::updateGoodsNum(CaseAddress addr, int num)
{
    if((addr.cabinetSeqNum == -1) || num<0)
        return;

    qDebug()<<"[updateGoodsNum]"<<addr.cabinetSeqNum<<addr.caseIndex<<addr.goodsIndex<<num;
    list_case.at(addr.caseIndex)->list_goods.at(addr.goodsIndex)->num = num;
    CasePanel* lab = (CasePanel*)ui->tableWidget->cellWidget(addr.caseIndex,0);
    lab->setText(list_case.at(addr.caseIndex)->list_goods);
//    ui->tableWidget->item(addr.caseIndex,0)->setText(list_case.at(addr.caseIndex)->caseShowStr());
    QSettings settings(CONF_CABINET, QSettings::IniFormat);
    settings.beginGroup(QString("Cabinet%1").arg(seqNum));
    settings.beginWriteArray(QString("case%1").arg(addr.caseIndex));
    settings.setArrayIndex(addr.goodsIndex);
    settings.setValue("num",num);
    settings.endArray();
    settings.endGroup();
}

void Cabinet::updateCabinetCase(CaseAddress addr)
{
    if(addr.cabinetSeqNum == -1)
        return;

    qDebug()<<"[updateCabinetCase]"<<addr.cabinetSeqNum<<addr.caseIndex;
    CasePanel* lab = (CasePanel*)ui->tableWidget->cellWidget(addr.caseIndex,0);
    lab->setText(list_case.at(addr.caseIndex)->list_goods);
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
        logo->setStyleSheet("background-color: rgb(85, 170, 255);font: 20px \"Sans Serif\";");
}

void Cabinet::setCaseName(GoodsInfo info, int index)
{
    QSettings settings(CONF_CABINET, QSettings::IniFormat);
    settings.beginGroup(QString("Cabinet%1").arg(seqNum));
    int arr_size = settings.beginReadArray(QString("case%1").arg(index));
    settings.setArrayIndex(0);
    bool isEmpty = settings.value("name").toString().isEmpty();
    settings.endArray();
    settings.beginWriteArray(QString("case%1").arg(index));
//    qDebug()<<isEmpty<<arr_size;
    if(isEmpty)
        settings.setArrayIndex(0);
    else
        settings.setArrayIndex(arr_size);

    settings.setValue("name",info.name);
    settings.setValue("abbName",info.abbName);
    settings.setValue("id",info.id);
    settings.setValue("unit",info.unit);
    settings.setValue("packageId",info.packageId);
    settings.setValue("num", info.num);
    settings.endArray();
    settings.endGroup();
    settings.sync();

    GoodsInfo* gInfo = new GoodsInfo(info);
    list_case.at(index)->list_goods<<gInfo;
    CasePanel* lab = (CasePanel*)ui->tableWidget->cellWidget(index,0);
    lab->setText(list_case.at(index)->list_goods);
//    ui->tableWidget->item(index,0)->setText(list_case.at(index)->caseShowStr());
//    list_case.at(index)->name = info.name;
//    list_case.at(index)->id = info.id;
//    list_case.at(index)->unit = info.unit;
//    list_case.at(index)->packageId = info.packageId;
//    ui->tableWidget->item(index,0)->setText(info.name+QString("×%1").arg(list_case.at(index)->num));
//    ui->tableWidget->item(index,0)->setBackgroundColor(QColor(0, 170, 127));
}

bool Cabinet::isInLeft()
{
    return posNum%2;
}

void Cabinet::searchByPinyin(QChar ch)
{
    int i = 0;
    int j = 0;
    qDebug()<<"searchByPinyin";

    for(i=0; i<list_case.count(); i++)
    {
        CabinetInfo* info = list_case[i];

        for(j=0; j<info->list_goods.count(); j++)
        {
            if(info->list_goods[j]->Py.at(0) == ch)
            {
                setCaseState(i, 1);
                break;
            }
            if(j == (info->list_goods.count()-1))
            {
                setCaseState(i, 0);
            }
        }
    }
}

void Cabinet::clearSearch()
{
    int i = 0;

    for(i=0; i<list_case.count(); i++)
    {
        if((i==1)&&isMainCabinet)
            continue;
        setCaseState(i,0);
    }
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
    qDebug()<<"setCaseState"<<index<<numState;
    state = numState;

//    if(isMainCabinet && index)
//        index++;

    if(numState == 0)//正常状态
    {
        CasePanel* lab = (CasePanel*)ui->tableWidget->cellWidget(index,0);
        lab->setCheckState(false);
        lab->setStyleSheet(cellStyle(QColor(36, 221, 149)));
    }
    else if(numState == 1)//被搜索状态
    {
        CasePanel* lab = (CasePanel*)ui->tableWidget->cellWidget(index,0);
        lab->setCheckState(false);
        lab->setStyleSheet(cellStyle(QColor(6, 161, 101)));
    }
    else if(numState == 2)//盘点完毕状态
    {
        CasePanel* lab = (CasePanel*)ui->tableWidget->cellWidget(index,0);
        lab->setCheckState(true);
//        lab->setStyleSheet(cellStyle(QColor(36, 221, 149))+checkStyle());
    }
}

int Cabinet::getCaseState()
{
    return state;
}

QString Cabinet::cellStyle(QColor rgb)
{
    QString ret = QString("color:rgb(255,255,255);\
                          background-color: rgb(%1, %2, %3);\
            margin-top:5px;\
            margin-bottom:5px;").arg(rgb.red()).arg(rgb.green()).arg(rgb.blue());
            return ret;
}

QString Cabinet::checkStyle()
{
    return QString("image: url(:/image/image/icon_check.png);\
            image-position:top right");
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
