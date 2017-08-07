#include "devicesimulate.h"
#include "ui_devicesimulate.h"
#include <QStringList>
#include <QSize>
#include <QDebug>
#include <QPushButton>

DeviceSimulate::DeviceSimulate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceSimulate)
{
    ui->setupUi(this);
    this->setWindowTitle("模拟控制台");
    this->setWindowFlags(Qt::WindowStaysOnTopHint);

    ui->listWidget->addItems(QStringList()<<"读卡器"<<"送货单"<<"扫码枪"<<"锁控");
    ui->listWidget->item(0)->setSizeHint(QSize(160, 70));
    ui->listWidget->item(0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->listWidget->item(1)->setSizeHint(QSize(160, 70));
    ui->listWidget->item(1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->listWidget->item(2)->setSizeHint(QSize(160, 70));
    ui->listWidget->item(2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->listWidget->item(3)->setSizeHint(QSize(160, 70));
    ui->listWidget->item(3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    initGroup();//初始化按钮组

    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
}

DeviceSimulate::~DeviceSimulate()
{
    delete ui;
}

void DeviceSimulate::recvLockCtrlData(QByteArray qba)
{
    qDebug()<<"[recvLockCtrlData]"<<qba;
}

//点击后会发送按钮文字内容
void DeviceSimulate::group_card_clicked(int id)
{
    QString str = group_card.button(id)->text();
    emit sendCardReaderData(str.toUtf8());
}

//点击后会发送按钮文字内容
void DeviceSimulate::group_drug_clicked(int id)
{
    QString str = group_drug.button(id)->text();
    emit sendCodeScanData(str.toUtf8());
}

//初始化按钮组
void DeviceSimulate::initGroup()
{
    group_card.addButton(ui->pushButton,0);
    group_card.addButton(ui->pushButton_2,1);
    group_card.addButton(ui->pushButton_3,2);
    group_card.addButton(ui->pushButton_4,3);
    connect(&group_card, SIGNAL(buttonClicked(int)), this, SLOT(group_card_clicked(int)));

    group_drug.addButton(ui->pushButton_5,0);
    group_drug.addButton(ui->pushButton_6,1);
    group_drug.addButton(ui->pushButton_7,2);
    group_drug.addButton(ui->pushButton_8,3);
    group_drug.addButton(ui->pushButton_9,4);
    group_drug.addButton(ui->pushButton_10,5);
    group_drug.addButton(ui->pushButton_11,6);
    group_drug.addButton(ui->pushButton_12,7);
    group_drug.addButton(ui->pushButton_13,8);
    group_drug.addButton(ui->pushButton_14,9);
    group_drug.addButton(ui->pushButton_15,10);
    group_drug.addButton(ui->pushButton_16,11);
    group_drug.addButton(ui->pushButton_17,12);
    group_drug.addButton(ui->pushButton_18,13);
    group_drug.addButton(ui->pushButton_19,14);
    group_drug.addButton(ui->pushButton_20,15);
    group_drug.addButton(ui->pushButton_21,16);
    group_drug.addButton(ui->pushButton_22,17);
    group_drug.addButton(ui->pushButton_23,18);
    group_drug.addButton(ui->pushButton_24,19);
    connect(&group_drug, SIGNAL(buttonClicked(int)), this, SLOT(group_drug_clicked(int)));
    initBtnText();
}

void DeviceSimulate::initBtnText()
{
    QPushButton* btn;
    QString baseCode = ((QPushButton*)group_drug.button(0))->text();
    int i=0;

    for(i=0; i<10; i++)
    {
        btn = (QPushButton*)group_drug.button(i);
        btn->setText(baseCode + QString::number(100+i));
    }

    baseCode = ((QPushButton*)group_drug.button(10))->text();
    for(i=10; i<20; i++)
    {
        btn = (QPushButton*)group_drug.button(i);
        btn->setText(baseCode + QString::number(100+i));
    }

}


void DeviceSimulate::on_listCode_clicked()
{
    QString str = ui->listCode->text();
    emit sendCodeScanData(str.toUtf8());
}

void DeviceSimulate::on_listCode_2_clicked()
{
    QString str = ui->listCode_2->text();
    emit sendCodeScanData(str.toUtf8());
}

void DeviceSimulate::on_car1_clicked()
{
    QByteArray qba = "fc0700000000ff";
    qba.replace(4,8, ui->car1->text().toLocal8Bit());
    qba = QByteArray::fromHex(qba);
    emit sendRfidData(qba);
}


void DeviceSimulate::on_car2_clicked()
{
    QByteArray qba = "fc0700000000ff";
    qba.replace(4,8, ui->car2->text().toLocal8Bit());
    qba = QByteArray::fromHex(qba);
    emit sendRfidData(qba);
}
