#include "devicesimulate.h"
#include "ui_devicesimulate.h"
#include <QStringList>
#include <QSize>
#include <QDebug>

DeviceSimulate::DeviceSimulate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceSimulate)
{
    ui->setupUi(this);
    this->setWindowTitle("模拟控制台");

    ui->listWidget->addItems(QStringList()<<"读卡器"<<"扫码枪"<<"锁控");
    ui->listWidget->item(0)->setSizeHint(QSize(210, 70));
    ui->listWidget->item(0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->listWidget->item(1)->setSizeHint(QSize(210, 70));
    ui->listWidget->item(1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->listWidget->item(2)->setSizeHint(QSize(210, 70));
    ui->listWidget->item(2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

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
}
