#include "cabinetctrlconfig.h"
#include "ui_cabinetctrlconfig.h"
#include <QSettings>
#include <QDebug>
#include "defines.h"

CabinetCtrlConfig::CabinetCtrlConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetCtrlConfig)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
}

void CabinetCtrlConfig::configStart(int seq, int index)
{
    qDebug()<<"[configStart]"<<seq<<index;
    curIndex = index;
    curSeq = seq;
    ui->tips->setText(QString("正在配置柜子%1柜格%2").arg(seq).arg(index));
    ui->indexnum->setText(QString::number(ui->indexnum->text().toInt()+1));
    this->show();
}

CabinetCtrlConfig::~CabinetCtrlConfig()
{
    delete ui;
}

bool CabinetCtrlConfig::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    return true;
}

void CabinetCtrlConfig::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.fillRect(this->rect(), QColor(22, 52, 73, 200));
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CabinetCtrlConfig::on_clear_clicked()
{
    ui->seqnum->clear();
    ui->indexnum->clear();
}

void CabinetCtrlConfig::on_test_clicked()
{
    int seq = ui->seqnum->text().toInt();
    int index = ui->indexnum->text().toInt();
    emit lockCtrl(seq, index);
}

void CabinetCtrlConfig::on_ok_clicked()
{
    QSettings settings(CONF_CABINET,QSettings::IniFormat);
    settings.beginGroup(QString("Cabinet%1").arg(curSeq));
    QByteArray ctrlSeq = settings.value("ctrlSeq", QByteArray::fromHex("00000000000000000000000000000000")).toByteArray();
    QByteArray ctrlIndex = settings.value("ctrlIndex", QByteArray::fromHex("00000000000000000000000000000000")).toByteArray();
    int seq = ui->seqnum->text().toInt();
    int index = ui->indexnum->text().toInt();

    seq = (seq>0xff)?0xff:seq;
    index = (index>0xff)?0xff:index;
    config->list_cabinet[curSeq]->list_case[curIndex]->ctrlSeq = seq;
    config->list_cabinet[curSeq]->list_case[curIndex]->ctrlIndex = index;

    ctrlSeq[curIndex] = seq;
    ctrlIndex[curIndex] = index;

    emit updateBtn();

    settings.setValue("ctrlSeq", ctrlSeq);
    settings.setValue("ctrlIndex", ctrlIndex);
    settings.sync();
    ui->tips->setText("配置成功");
}

void CabinetCtrlConfig::on_cancel_clicked()
{
    this->close();
}
