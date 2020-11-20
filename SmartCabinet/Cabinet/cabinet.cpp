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
    posType = 1;
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
    ui->tableWidget->clear();

    if(!isMainCabinet)
    {
//        cabType = 0;
        caseNum = CAB_CASE_0_NUM;
        ui->tableWidget->setRowCount(caseNum);
        return;
    }
    else
    {
//        cabType = 1;
        caseNum = CAB_CASE_1_NUM;
        ui->tableWidget->setRowCount(caseNum);
        logo = new QLabel(this);
        logo->setWordWrap(true);
        logo->setStyleSheet("background-color: rgb(85, 170, 255);font: 18pt \"WenQuanYi Micro Hei Mono\";");
        ui->tableWidget->setCellWidget(1,0,logo);
    }
}

void Cabinet::CabinetInit(QString cLayout, int seq, bool doubleCol, int sPos)
{
    seqNum = seq;
    caseNum = cLayout.length();
    screenPos = sPos;
    cabLayout = cLayout;
    isMainCabinet = (screenPos>=0);
    qDebug()<<"isMainCabinet"<<isMainCabinet<<screenPos;

    if(!isMainCabinet)
    {
        cabSplit(cLayout, ui->tableWidget);
    }
    else
    {
        cabSplit(cLayout, ui->tableWidget);
        logo = new QLabel(this);
        logo->setWordWrap(true);
        logo->setStyleSheet("background-color: rgb(85, 170, 255);font: 18pt \"WenQuanYi Micro Hei Mono\";");
        ui->tableWidget->setCellWidget(sPos,0,logo);
    }

    for(int i=0; i<ui->tableWidget->rowCount(); i++)
    {
        CasePanel* lab = new CasePanel(doubleCol);
        ui->tableWidget->setCellWidget(i, 0, lab);
        setCaseState(i, 0);
        lab->setText(SqlManager::getCaseText(seqNum, i));
    }
}

void Cabinet::cabSplit(QString scale, QTableWidget *table)
{
    if(scale.isEmpty()||(table == NULL))
    {
        qDebug()<<scale;
        return;
    }
    int rowCount = scale.length();
    int baseCount = getBaseCount(scale);
    int baseHeight = table->geometry().height()/baseCount;
    int i = 0;
    checkFlag = QByteArray(rowCount, 0);
    table->setRowCount(rowCount);
    table->setColumnCount(1);

    table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setVisible(false);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

//    qDebug()<<"cabSplit"<<table->geometry().height()<<baseCount<<baseHeight;
    for(i=0; i<rowCount; i++)
    {
        table->setRowHeight(i,baseHeight*(scale.mid(i,1).toInt()));
    }
}

int Cabinet::getBaseCount(QString scale)
{
    int i = 0;
    int ret = 0;
    if(scale.isEmpty())
        return ret;

    for(i=0; i<scale.length(); i++)
    {
        ret += scale.mid(i,1).toInt();
    }
    return ret;
}

void Cabinet::setCabPos(int pos)
{
    posNum = pos;
}

void Cabinet::setSpecCase(int index, bool spec)
{
    CasePanel* lab = (CasePanel*)ui->tableWidget->cellWidget(index,0);
    lab->setSpec(spec);
    if(!spec)
    {
        lab->setText(SqlManager::getCaseText(seqNum, index));// setText(list_case[index]->list_goods);
    }
}

int Cabinet::getUnCheckNum()
{
    int ret = 0;
    for(int i=0; i<checkFlag.size(); i++)
    {
        if(!checkFlag[i])
            ret++;
    }
    if(screenPos>=0)
        ret--;
    return ret;
}

int Cabinet::getCabPos()
{
    if(posType)//新布局规则
        return posNum;
    else
        return realPos(posNum);
}

int Cabinet::getSeqNum()
{
    return seqNum;
}

QString Cabinet::getLayout()
{
    return cabLayout;
}

//4,2,0,1,3  -->  -2,-1,0,1,2
int Cabinet::realPos(int pos)
{
    if(pos%2)
        return -(pos+1)/2;
    else
        return pos/2;
}

void Cabinet::setPosType(bool _postype)
{
    posType = _postype;
}

void Cabinet::setScreenPos(int pos)
{
    screenPos = pos;
    isMainCabinet = (screenPos>=0);
    if(isMainCabinet)
    {
        logo = new QLabel(this);
        logo->setWordWrap(true);
        logo->setStyleSheet("background-color: rgb(13,96,150);font: 18pt \"WenQuanYi Micro Hei Mono\";");
        ui->tableWidget->setCellWidget(pos,0,logo);
    }
}

int Cabinet::getScreenPos()
{
    return screenPos;
}

int Cabinet::getCaseNum()
{
    return caseNum;
}

//void Cabinet::setCabType(int _type)
//{
//    cabType = _type;
//}

void Cabinet::checkCase(int index)
{
    setCaseState(index, 2);
}

void Cabinet::searchCase(int index)
{
    setCaseState(index, 1);
}

void Cabinet::initCase(int index)
{
    setCaseState(index, 0);
}

void Cabinet::updateCase(int caseIndex)
{
    if(isMainCabinet && (caseIndex == screenPos))
        return;

    if(caseIndex >= ui->tableWidget->rowCount())
        return;

    CasePanel* lab = (CasePanel*)ui->tableWidget->cellWidget(caseIndex,0);
    lab->setText(SqlManager::getCaseText(seqNum, caseIndex));
}

int Cabinet::rowCount()
{
    return ui->tableWidget->rowCount();
}

void Cabinet::updateCabinet()
{
    int i=0;
    for(i=0; i<rowCount(); i++)
    {
        if(!(isMainCabinet && (i == screenPos)))
            updateCase(i);
    }
}

void Cabinet::setCtrlWord(QByteArray seq, QByteArray index)
{
    _ctrlSeq = seq;
    _ctrlIndex = index;

    if(_ctrlSeq.size()<16)
        _ctrlSeq.resize(16);

    if(_ctrlIndex.size()<16)
        _ctrlIndex.resize(16);
//    for(int i=0; i<list_case.count(); i++)
//    {
//        setCtrlWord(i, seq, index);
    //    }
}

void Cabinet::setCtrlSeq(int caseIndex, int seq)
{
//    qDebug()<<"setCtrlSeq"<<caseIndex<<_ctrlSeq.size()<<_ctrlSeq.toHex();
    if(caseIndex >= _ctrlSeq.size())
        return;

    _ctrlSeq[caseIndex] = seq;
//    qDebug()<<_ctrlSeq[caseIndex];
}

void Cabinet::setctrlIndex(int caseIndex, int index)
{
    if(caseIndex >= _ctrlIndex.size())
        return;

    _ctrlIndex[caseIndex] = index;
}

int Cabinet::ctrlSeq(int caseIndex)
{
    return caseIndex > _ctrlSeq.size() ? 0:_ctrlSeq[caseIndex];
}

int Cabinet::ctrlIndex(int caseIndex)
{
    return caseIndex > _ctrlIndex.size() ? 0:_ctrlIndex[caseIndex];
}

int Cabinet::getMaxshowNum(int caseIndex)
{
    CasePanel* lab = (CasePanel*)ui->tableWidget->cellWidget(caseIndex,0);
    return lab->maxShowNum();
}

bool Cabinet::haveEmptyPos(int caseIndex)
{
    qDebug()<<"haveEmptyPos"<<SqlManager::getShowCountByCase(seqNum, caseIndex) << getMaxshowNum(caseIndex);
    if(SqlManager::getShowCountByCase(seqNum, caseIndex) < getMaxshowNum(caseIndex))
        return true;
    else
        return false;
}

void Cabinet::updateGoodsNum(CaseAddress addr, int num)
{
    if((addr.cabinetSeqNum == -1) || num<0)
        return;

    updateCase(addr.caseIndex);
//    qDebug()<<"[updateGoodsNum]"<<addr.cabinetSeqNum<<addr.caseIndex<<addr.goodsIndex<<num;
//    list_case.at(addr.caseIndex)->list_goods.at(addr.goodsIndex)->num = num;
//    CasePanel* lab = (CasePanel*)ui->tableWidget->cellWidget(addr.caseIndex,0);
//    lab->setText(list_case.at(addr.caseIndex)->list_goods);
//    QString setKey = QString("Cabinet%1/case%2/%3/num").arg(seqNum).arg(addr.caseIndex).arg(addr.goodsIndex+1);
//    qDebug()<<"setkey"<<setKey;
//    QSettings settings(CONF_CABINET, QSettings::IniFormat);
//    settings.setValue(setKey, num);
//    settings.sync();

//    settings.beginGroup(QString("Cabinet%1").arg(seqNum));
//    settings.beginWriteArray(QString("case%1").arg(addr.caseIndex));
//    settings.setArrayIndex(addr.goodsIndex);
//    settings.setValue("num",num);
//    settings.endArray();
//    settings.endGroup();
}

void Cabinet::updateCabinetCase(CaseAddress addr)
{
    if(addr.cabinetSeqNum == -1)
        return;

    qDebug()<<"[updateCabinetCase]"<<addr.cabinetSeqNum<<addr.caseIndex;
    updateCase(addr.caseIndex);
//    CasePanel* lab = (CasePanel*)ui->tableWidget->cellWidget(addr.caseIndex,0);
//    lab->setText(list_case.at(addr.caseIndex)->list_goods);
}

int Cabinet::cabinetPosNum()
{
    return posNum;
}

void Cabinet::clearSelectState(int row)
{
    ui->tableWidget->setItemSelected(ui->tableWidget->item(row,0), false);
}

void Cabinet::showMsg(QString msg, bool iswarnning)
{
    if(logo == NULL)
    {
        qDebug("show NULL>>>>>>>>>>>>>>");
        return;
    }
    logo->setText(msg);
    logo->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    if(iswarnning)
        logo->setStyleSheet("color:#fff; background-color: rgb(13,96,150);font: 18px \"WenQuanYi Micro Hei Mono\";image:url(:/image/image/warning.png);image-position:top left");
    else
        logo->setStyleSheet("color:#fff;background-color: rgb(13,96,150);font: 18px \"WenQuanYi Micro Hei Mono\";");
}

void Cabinet::setCaseName(Goods info, int index)
{
    Q_UNUSED(info);
    Q_UNUSED(index);
//    if(index>=list_case.count())
//        return;
//    QSettings settings(CONF_CABINET, QSettings::IniFormat);
//    settings.beginGroup(QString("Cabinet%1").arg(seqNum));
//    int arr_size = settings.beginReadArray(QString("case%1").arg(index));
//    settings.setArrayIndex(0);
//    bool isEmpty = settings.value("name").toString().isEmpty();
//    settings.endArray();
//    settings.beginWriteArray(QString("case%1").arg(index));
//    qDebug()<<"[setCaseName]"<<"seq"<<seqNum<<"caseindex:"<<index<<"goodsindex"<<arr_size;
//    qDebug()<<"is empty"<<isEmpty;
//    if(isEmpty)
//        settings.setArrayIndex(0);
//    else
//        settings.setArrayIndex(arr_size);

//    settings.setValue("name",info.name);
//    settings.setValue("abbName",info.abbName);
//    settings.setValue("id",info.id);
//    settings.setValue("unit",info.unit);
//    settings.setValue("packageId",info.packageId);
//    settings.setValue("num", info.num);
//    settings.endArray();
//    settings.endGroup();
//    settings.sync();

//    GoodsInfo* gInfo = new GoodsInfo(info);
//    list_case.at(index)->list_goods<<gInfo;
//    CasePanel* lab = (CasePanel*)ui->tableWidget->cellWidget(index,0);//qDebug()<<lab->labWidth();
//    lab->setText(list_case.at(index)->list_goods);
}

bool Cabinet::isInLeft()
{
    return posNum%2;
}

void Cabinet::clearSearch()
{
//    int i = 0;

//    for(i=0; i<list_case.count(); i++)
//    {
//        if((i==screenPos)&&isMainCabinet)
//            continue;
//        setCaseState(i,0);
//    }
}

void Cabinet::clearGoods()
{
//    foreach(CabinetInfo* info, list_case)
//    {
//        info->clearList();
//    }
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
    if(index >= ui->tableWidget->rowCount())
        return;
    if(index == screenPos)
    {
        qDebug()<<"screen pos";
        return;
    }
//    qDebug()<<"setCaseState"<<index<<numState;
    state = numState;
    checkFlag[index] = numState;

//    if(isMainCabinet && index)
//        index++;
    CasePanel* lab = (CasePanel*)ui->tableWidget->cellWidget(index,0);
    if(lab->isSpecialCase())
        return;

    if(numState == 0)//正常状态
    {
        lab->setCheckState(false);
        lab->setStyleSheet(cellStyle(QColor(36, 221, 149)));
    }
    else if(numState == 1)//被搜索状态
    {
        lab->setCheckState(false);
        lab->setStyleSheet(cellStyle(QColor(238, 128, 61)));
    }
    else if(numState == 2)//盘点完毕状态
    {
        lab->setCheckState(true);
        lab->setStyleSheet(cellStyle(QColor(36, 221, 149))+checkStyle());
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
            margin-top:2px;\
            margin-bottom:2px;").arg(rgb.red()).arg(rgb.green()).arg(rgb.blue());
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
//    caseDraw(cabType);
//    qDebug("resize");
    cabSplit(cabLayout,ui->tableWidget);
}

void Cabinet::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
