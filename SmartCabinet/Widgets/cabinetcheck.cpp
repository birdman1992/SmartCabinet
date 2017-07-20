#include "cabinetcheck.h"
#include "ui_cabinetcheck.h"
#include <QPainter>

CabinetCheck::CabinetCheck(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetCheck)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
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

void CabinetCheck::checkStart(CaseAddress addr)
{
    tableClear();
    ui->msg->setText("");
    ui->ok->setText("提交");
    curCheckCab = config->list_cabinet[addr.cabinetSeqNUM]->list_case[addr.caseIndex];
    curAddr.cabinetSeqNUM = addr.cabinetSeqNUM;
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
        list_item.clear();
    curCheckCab = NULL;
    ui->checktable->clear();
    ui->checktable->setRowCount(0);
    ui->checktable->setColumnCount(0);
}

void CabinetCheck::on_pushButton_clicked()
{
    this->close();
}

void CabinetCheck::on_ok_clicked()
{
    ui->msg->setText("正在提交");
    emit checkCase(list_item, curAddr);
}
