#include "cabinetcheck.h"
#include "ui_cabinetcheck.h"
#include <QPainter>
#include <QDebug>
#include <qscrollbar.h>

CabinetCheck::CabinetCheck(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetCheck)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    QFile qssScrollbar(":/stylesheet/styleSheet/ScrollBar.qss");
    qssScrollbar.open(QIODevice::ReadOnly);
    QString style = QString(qssScrollbar.readAll());
    ui->checktable->verticalScrollBar()->setStyleSheet(style);
    qssScrollbar.close();
}

CabinetCheck::~CabinetCheck()
{
    delete ui;
}

bool CabinetCheck::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    return true;
}

void CabinetCheck::checkRst(QString msg)
{
    ui->msg->setText(msg);
}

void CabinetCheck::show()
{
    this->showFullScreen();
}

void CabinetCheck::checkScan(QString scanId, QString fullId)
{
    int i = 0;

    if(!codeAppend(fullId))
    {
        qDebug()<<"[checkScan]"<<"id is repeat";
        checkRst("条码重复，请勿重复盘点");
        return;
    }


    for(i=0; i<list_item.count(); i++)
    {
//        qDebug()<<list_item.at(i)->itemId()<<scanId;
        if(scanId == list_item.at(i)->itemId())
        {
            if(!list_item.at(i)->itemAdd())
            {
                checkRst("盘点数量无法超过当前库存");
                codeAppendCancel();
            }
            else
            {
                list_item.at(i)->list_fullId<<fullId;
                checkRst("添加成功");
            }
            return;
        }
    }
    codeAppendCancel();
    checkRst("添加失败,该物品不属于此柜格");
}

void CabinetCheck::checkStart(CaseAddress addr)
{
    ui->msg->setText("");
    ui->ok->setText("提交");

    curCheckCab = config->list_cabinet[addr.cabinetSeqNum]->list_case[addr.caseIndex];
    curAddr.cabinetSeqNum = addr.cabinetSeqNum;
    curAddr.caseIndex = addr.caseIndex;

    int i = 0;
    ui->checktable->setRowCount(curCheckCab->list_goods.count());
    ui->checktable->setColumnCount(1);

    for(i=0; i<curCheckCab->list_goods.count(); i++)
    {
        CabinetCheckItem* item = new CabinetCheckItem(curCheckCab->list_goods.at(i));
        ui->checktable->setCellWidget(i, 0, item);
        list_item<<item;
    }

    this->show();
}

void CabinetCheck::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.fillRect(this->rect(), QColor(22, 52, 73, 200));
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CabinetCheck::tableClear()
{
    if(!list_item.isEmpty())
    {
        qDeleteAll(list_item.begin(), list_item.end());
        list_item.clear();
    }
    list_code.clear();
    curCheckCab = NULL;
    ui->checktable->clear();
    ui->checktable->setRowCount(0);
    ui->checktable->setColumnCount(0);
}

bool CabinetCheck::codeAppend(QString code)
{
    qDebug()<<code<<list_code.indexOf(code);
    if((!list_code.isEmpty()) && list_code.indexOf(code) != -1)
        return false;

    list_code<<code;
    return true;
}

void CabinetCheck::codeAppendCancel()
{
    if(!list_code.isEmpty())
        list_code.removeLast();
}

void CabinetCheck::on_pushButton_clicked()
{
    tableClear();
    this->close();
}

void CabinetCheck::on_ok_clicked()
{
    ui->msg->setText("正在提交");
//    emit checkCase(list_code, curAddr);
    emit checkCase(list_item, curAddr);
}
