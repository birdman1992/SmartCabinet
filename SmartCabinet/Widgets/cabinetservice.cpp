#include "cabinetservice.h"
#include "ui_cabinetservice.h"
#include "defines.h"
#include <QDebug>
#include <arpa/inet.h>
#include <QPainter>
#include <QListWidgetItem>
#include <QElapsedTimer>
#include <unistd.h>
#include <QDebug>
#include "Device/controldevice.h"

CabinetService::CabinetService(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetService)
{
    ui->setupUi(this);
    ui->addr->installEventFilter(this);
    dev_network = NULL;
    initStack();
    initGroup();
    ui->listWidget->setCurrentRow(0);
    ui->stackedWidget->setCurrentIndex(0);
//    QTimer::singleShot(1000, this, SLOT(initNetwork()));
//    initNetwork();

    setAttribute(Qt::WA_TranslucentBackground, true);
}

CabinetService::~CabinetService()
{
    delete ui;
}

void CabinetService::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    p.fillRect(this->rect(), QColor(255, 255, 255, 80));  //QColor最后一个参数80代表背景的透明度
}

void CabinetService::on_back_clicked()
{
    winSwitch(INDEX_CAB_SHOW);
}

void CabinetService::showEvent(QShowEvent *)
{
    initNetwork();
}

bool CabinetService::eventFilter(QObject *w, QEvent *e)
{
    if(w == ui->addr && e->type() == QEvent::FocusIn)
    {
//        ui->addr->clear();
    }
    return QWidget::eventFilter(w,e);
}

void CabinetService::updateNetInfo()
{
    dev_ip = dev_network->ip();
    dev_gateway = dev_network->gateway();
    dev_netmask = dev_network->netmask();
    qDebug()<<dev_ip<<dev_gateway<<dev_netmask;
    ui->addr->setText(dev_ip);
    ui->netmask->setText(dev_netmask);
    ui->gateway->setText(dev_gateway);
}

void CabinetService::initStack()
{
//    QListWidgetItem* item = new QListWidgetItem("网络配置");
//    item->setSizeHint();

    ui->listWidget->addItem(new QListWidgetItem("网络配置"));
    ui->listWidget->addItem(new QListWidgetItem("锁控测试"));
    ui->listWidget->addItem(new QListWidgetItem("设置"));

    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);

    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
}

void CabinetService::initGroup()
{
    l_board_num<<ui->checkBox<<ui->checkBox_2<<ui->checkBox_3<<ui->checkBox_4<<ui->checkBox_5<<ui->checkBox_6;

    l_lock_num.addButton(ui->btn_0, 0);
    l_lock_num.addButton(ui->btn_1, 1);
    l_lock_num.addButton(ui->btn_2, 2);
    l_lock_num.addButton(ui->btn_3, 3);
    l_lock_num.addButton(ui->btn_4, 4);
    l_lock_num.addButton(ui->btn_5, 5);
    l_lock_num.addButton(ui->btn_6, 6);
    l_lock_num.addButton(ui->btn_7, 7);
    l_lock_num.addButton(ui->btn_8, 8);
    l_lock_num.addButton(ui->btn_9, 9);
    l_lock_num.addButton(ui->btn_10, 10);
    l_lock_num.addButton(ui->btn_11, 11);
    l_lock_num.addButton(ui->btn_12, 12);
    l_lock_num.addButton(ui->btn_13, 13);
    l_lock_num.addButton(ui->btn_14, 14);
    l_lock_num.addButton(ui->btn_15, 15);
    l_lock_num.addButton(ui->btn_16, 16);
    l_lock_num.addButton(ui->btn_17, 17);
    l_lock_num.addButton(ui->btn_18, 18);
    l_lock_num.addButton(ui->btn_19, 19);
    l_lock_num.addButton(ui->btn_20, 20);
    l_lock_num.addButton(ui->btn_21, 21);
    l_lock_num.addButton(ui->btn_22, 22);
    l_lock_num.addButton(ui->btn_23, 23);

    connect(&l_lock_num, SIGNAL(buttonClicked(int)), this, SLOT(ctrl_lock(int)));
    connect(ui->boardcast, SIGNAL(clicked(bool)), this, SLOT(ctrl_boardcast()));
}

void CabinetService::initNetwork()
{
    qDebug("[QNetInterface1]");
//    QNetworkInterface::interfaceFromName("eth0");
//    qDebug()<<QNetworkInterface::interfaceFromName("eth1").flags();
//    return;

    if(dev_network != NULL)
        delete dev_network;
#ifdef SIMULATE_ON
    dev_network = new QNetInterface("eth0");
#else
    dev_network = new QNetInterface("eth1");
#endif
    qDebug("[QNetInterface2]");
    updateNetInfo();
}


void CabinetService::on_addr_returnPressed()
{
    this->setFocus();
}

//void CabinetService::on_addr_textEdited(const QString &arg1)
//{
//    qDebug()<<arg1;
//}

void CabinetService::on_ok_clicked()
{
    if(ui->addr->text() != dev_ip)
    {
        if(dev_network->numPointCheck(ui->addr->text()))
        {
            dev_ip = ui->addr->text();
            dev_network->setIp(dev_ip);
            ui->warning->clear();
        }
        else
        {
            ui->warning->setText("地址格式错误");
        }
    }
    if(ui->netmask->text() != dev_netmask)
    {
        if(dev_network->numPointCheck(dev_netmask))
        {
            dev_netmask = ui->netmask->text();
            dev_network->setNetmask(dev_netmask);
            ui->warning->clear();
        }
        else
        {
            ui->warning->setText("地址格式错误");
        }
    }
    if(ui->gateway->text() != dev_gateway)
    {
        if(dev_network->numPointCheck(dev_gateway))
        {
            dev_gateway = ui->gateway->text();
            dev_network->setGateway(dev_gateway);
            ui->warning->clear();
        }
        else
        {
            ui->warning->setText("地址格式错误");
        }
    }
    updateNetInfo();
}

void CabinetService::on_cancel_clicked()
{

}

void CabinetService::ctrl_lock(int id)
{
    int i = 0;
    QCheckBox* box;
    qDebug()<<id;
    foreach(box, l_board_num)
    {
        QElapsedTimer t;
        t.start();
        while(t.elapsed() < 50)
        {
            QCoreApplication::processEvents();
            ::usleep(10000);//sleep和usleep都已经obsolete，Linux下也可以使用nanosleep代替
        }
        if(box->isChecked())
        {
            emit requireOpenLock(i, id);
            qDebug()<<"ctrl_lock"<<i<<id;
        }

        i++;
    }
}

void CabinetService::ctrl_boardcast()
{
    int i = 0;
    int j = 0;

    for(j=0; j<5; j++)
    {
        for(i=0; i<24; i++)
        {
            QElapsedTimer t;
            t.start();
            while(t.elapsed() < 50)
            {
                QCoreApplication::processEvents();
                ::usleep(10000);//sleep和usleep都已经obsolete，Linux下也可以使用nanosleep代替
            }
            emit requireOpenLock(j, i);
        }
    }
}

void CabinetService::on_clear_clicked()
{
    emit requireClear();
}

void CabinetService::on_init_clicked()
{
    QProcess process;
    process.start("rm /home/config/cabinet.ini");
    process.waitForFinished();
    qApp->closeAllWindows();

#ifdef SIMULATE_ON
    QProcess::startDetached(qApp->applicationFilePath(), QStringList());
#else
    QStringList args;
    args.append("-qws");
    QProcess::startDetached(qApp->applicationFilePath(),args);
#endif
}

void CabinetService::on_check_clicked()
{
#ifdef SIMULATE_ON
    return;
#endif
    QProcess process;
    process.start("rm /etc/pointercal");
    process.waitForFinished();
    QProcess::startDetached("/home/qtdemo");
}
