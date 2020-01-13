#include "cabinetrefund.h"
#include "ui_cabinetrefund.h"
#include <QDebug>
#include <QScrollBar>

CabinetRefund::CabinetRefund(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cabinetRefund)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    tableClear();
    QFile qssScrollbar(":/stylesheet/styleSheet/ScrollBar.qss");
    qssScrollbar.open(QIODevice::ReadOnly);
    QString style = QString(qssScrollbar.readAll());
    ui->checktable->verticalScrollBar()->setStyleSheet(style);
    qssScrollbar.close();
}

CabinetRefund::~CabinetRefund()
{
    delete ui;
}

bool CabinetRefund::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    return true;
}

void CabinetRefund::show()
{
    this->showFullScreen();
}

void CabinetRefund::refundRst(QString msg)
{
    ui->msg->setText(msg);
}

void CabinetRefund::refundScan(QString scanId, QString fullId)
{
    int i = 0;

    if(!codeAppend(fullId))
    {
        qDebug()<<"[checkScan]"<<"id is repeat";
        refundRst("条码重复，请勿重复扫描");
        return;
    }

    for(i=0; i<list_item.count(); i++)
    {
        qDebug()<<scanId<<list_item.at(i)->itemId();
        if(scanId == list_item.at(i)->itemId())
        {
            if(!list_item.at(i)->itemAdd())
            {
                refundRst("退货数量无法超过当前库存");
                codeAppendCancel();
            }
            else
                refundRst("添加成功");
            return;
        }
    }
    codeAppendCancel();
    refundRst("退货失败,该物品不属于此柜格");
}

void CabinetRefund::refundStart(CaseAddress addr)
{qDebug("[refundStart]");
    ui->msg->setText("");
    ui->ok->setText("提交");

    curAddr.cabinetSeqNum = addr.cabinetSeqNum;
    curAddr.caseIndex = addr.caseIndex;
    QList<Goods*> list_goods = SqlManager::getGoodsList(addr.cabinetSeqNum, addr.caseIndex);

    int i = 0;
    ui->checktable->setRowCount(list_goods.count());
    ui->checktable->setColumnCount(1);

    for(i=0; i<list_goods.count(); i++)
    {
        CabinetRefundItem* item = new CabinetRefundItem(list_goods.at(i));
        ui->checktable->setCellWidget(i, 0, item);
        list_item<<item;
    }

    this->show();
}

void CabinetRefund::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.fillRect(this->rect(), QColor(22, 52, 73, 200));
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CabinetRefund::tableClear()
{
    if(!list_item.isEmpty())
    {
        qDeleteAll(list_item.begin(), list_item.end());
        list_item.clear();
    }
    list_code.clear();
    curRefundCab = NULL;
    ui->checktable->clear();
    ui->checktable->setRowCount(0);
    ui->checktable->setColumnCount(0);
}

bool CabinetRefund::codeAppend(QString code)
{
    qDebug()<<code<<list_code.indexOf(code);
    if((!list_code.isEmpty()) && list_code.indexOf(code) != -1)
        return false;

    list_code<<code;
    return true;
}

void CabinetRefund::codeAppendCancel()
{
    if(!list_code.isEmpty())
        list_code.removeLast();
}

void CabinetRefund::on_pushButton_clicked()
{
    tableClear();
    this->close();
}

void CabinetRefund::on_ok_clicked()
{
    if(list_code.isEmpty())
    {
        ui->msg->setText("请扫码添加退货物品");
        return;
    }
    ui->msg->setText("正在提交");
    emit refundCase(list_code, 3);
}
