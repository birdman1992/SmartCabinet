#include "cabinetservice.h"
#include "ui_cabinetservice.h"
#include "defines.h"
#include <QDebug>
#include <arpa/inet.h>
#include <QPainter>
#include <QListWidgetItem>
#include <QElapsedTimer>
#include <unistd.h>
#include <stdio.h>
#include <QDebug>
#include <QLayout>
#include <QPushButton>
#include "Device/controldevice.h"

CabinetService::CabinetService(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetService)
{
    ui->setupUi(this);
    ui->addr->installEventFilter(this);

    cfg_layout = new QHBoxLayout();
    ui->ctrlCfg->setLayout(cfg_layout);

    dev_network = NULL;
    lockConfigIsOk = false;
    win_ctrl_config = new CabinetCtrlConfig();
    connect(win_ctrl_config,SIGNAL(lockCtrl(int,int)),this, SIGNAL(requireOpenLock(int,int)));
    connect(win_ctrl_config, SIGNAL(updateBtn()), this,SLOT(updateBtn()));

    initStack();
    initGroup();
    showVerInfo();

    ui->listWidget->setCurrentRow(0);
    ui->stackedWidget->setCurrentIndex(0);
//    QTimer::singleShot(1000, this, SLOT(initNetwork()));
//    initNetwork();

    setAttribute(Qt::WA_TranslucentBackground, true);
}

CabinetService::~CabinetService()
{
    delete win_ctrl_config;
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

bool CabinetService::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    win_ctrl_config->installGlobalConfig(config);
    ui->insert_pos->setMaximum(config->list_cabinet.count()-1);
    return true;
}


void CabinetService::on_back_clicked()
{
    emit winSwitch(INDEX_CAB_SHOW);
}

void CabinetService::showEvent(QShowEvent *)
{
    initNetwork();
    creatCtrlConfig();
    ui->server_addr->setText(config->getServerAddress());
    qDebug()<<ui->server_addr->text();
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
    qDebug()<<"updateNetInfo1";
    dev_ip = dev_network->ip();
    dev_gateway = dev_network->gateway();
    dev_netmask = dev_network->netmask();
    qDebug()<<dev_ip<<dev_gateway<<dev_netmask;
    ui->addr->setText(dev_ip);
    ui->netmask->setText(dev_netmask);
    ui->gateway->setText(dev_gateway);
    qDebug()<<"updateNetInfo2";
}

void CabinetService::initStack()
{
//    QListWidgetItem* item = new QListWidgetItem("网络配置");
//    item->setSizeHint();
    QListWidgetItem *add_item;
    add_item = new QListWidgetItem("网络配置");
    add_item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->listWidget->addItem(add_item);

    add_item = new QListWidgetItem("锁控测试");
    add_item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->listWidget->addItem(add_item);

    add_item = new QListWidgetItem("设置");
    add_item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->listWidget->addItem(add_item);

    add_item = new QListWidgetItem("锁控配置");
    add_item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->listWidget->addItem(add_item);

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

void CabinetService::creatCtrlConfig()
{
    if(lockConfigIsOk)
        return;

    lockConfigIsOk = true;

    int i = 0;
    int j = 0;

//    i = config->list_cabinet.count() - 1;
    qDebug()<<"[creatCtrlConfig]:"<<config->list_cabinet.count();
    QList<Cabinet*> list_temp = config->list_cabinet;
    qSort(list_temp.begin(), list_temp.end(), posSort);

    for(i=0; i<list_temp.count(); i++)
    {
        QVBoxLayout* layout = new QVBoxLayout();

        for(j=0; j<list_temp[i]->list_case.count(); j++)
        {
            QString layoutStrech = list_temp.at(i)->getLayout();
            QPushButton* btn = new QPushButton(this);
            btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            btn->setMaximumWidth(400);
            QString str = QString("序号：%1\nIO号：%2").arg(list_temp.at(i)->list_case.at(j)->ctrlSeq).arg(list_temp.at(i)->list_case.at(j)->ctrlIndex);
            btn->setText(str);
            layout->addWidget(btn);
            l_lock_conf.addButton(btn,((list_temp.at(i)->getSeqNum()<<8)+j));
            layout->setStretch(j,QString(layoutStrech.at(j)).toInt());
        }
        cfg_layout->addLayout(layout);
    }
    connect(&l_lock_conf, SIGNAL(buttonClicked(int)), this, SLOT(ctrl_conf(int)));
    return;

    for(; i>=0; i--)
    {
        QVBoxLayout* layout = new QVBoxLayout();

        if(!config->list_cabinet[i]->isInLeft())
            continue;

        for(j=0; j<config->list_cabinet[i]->list_case.count(); j++)
        {
                QPushButton* btn = new QPushButton(this);
                btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
                btn->setMaximumWidth(400);
                QString str = QString("序号：%1\nIO号：%2").arg(config->list_cabinet.at(i)->list_case.at(j)->ctrlSeq).arg(config->list_cabinet.at(i)->list_case.at(j)->ctrlIndex);
                btn->setText(str);
                layout->addWidget(btn);
                l_lock_conf.addButton(btn,(i<<8)+j);
        }
        cfg_layout->addLayout(layout);
    }

    i = 0;
    for(; i<config->list_cabinet.count(); i++)
    {
        QVBoxLayout* layout = new QVBoxLayout();

        if((config->list_cabinet[i]->isInLeft()))
            continue;

        for(j=0; j<config->list_cabinet[i]->list_case.count(); j++)
        {
                QPushButton* btn = new QPushButton(this);
                btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
                btn->setMaximumWidth(400);
                QString str = QString("序号：%1\nIO号：%2").arg(config->list_cabinet.at(i)->list_case.at(j)->ctrlSeq).arg(config->list_cabinet.at(i)->list_case.at(j)->ctrlIndex);
                btn->setText(str);
                layout->addWidget(btn);
                l_lock_conf.addButton(btn,(i<<8)+j);
        }
        cfg_layout->addLayout(layout);
    }

    connect(&l_lock_conf, SIGNAL(buttonClicked(int)), this, SLOT(ctrl_conf(int)));
}

void CabinetService::showVerInfo()
{
    QByteArray info = ui->verInfo->text().toUtf8();
    printf("************************************\n\n\n");
    printf("%s\n",info.data());
    printf("\n\n************************************\n");
}

bool CabinetService::inserCol(int pos, int num)
{
    if(!(pos+num))
        return false;
    else
        emit requireInsertCol(pos, num);
    return true;
}

void CabinetService::initNetwork()
{
//    qDebug("[QNetInterface1]");
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
//    qDebug("[QNetInterface2]");
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
//    if(ui->gateway->text() != dev_gateway)
//    {
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
//    }
    dev_network->saveNetwork();
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
            while(t.elapsed() < 150)
            {
                QCoreApplication::processEvents();
                ::usleep(10000);//sleep和usleep都已经obsolete，Linux下也可以使用nanosleep代替
            }
            emit requireOpenLock(j, i);
        }
    }
}

void CabinetService::recvInsertColResult(bool success)
{
    ui->insert->setEnabled(true);
    if(success)
    {
        ui->insert->setText("插入\n成功");
    }
    else
    {
        ui->insert->setText("插入\n失败");
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
#else
    QProcess process;
    process.start("rm /etc/pointercal");
    process.waitForFinished();
    QProcess::startDetached("reboot");
//    QStringList args;
//    args.append("-qws");
//    QProcess::startDetached(qApp->applicationFilePath(),args);
#endif
}

void CabinetService::ctrl_conf(int id)
{
    curId = id;
    win_ctrl_config->configStart(id>>8, id&0xff);
//        qDebug()<<(id>>8)<<(id&0xff);
}

void CabinetService::updateBtn()
{
    QPushButton* btn = (QPushButton*)l_lock_conf.button(curId);
    if(btn == NULL)
        return;

    int seq = curId>>8;
    int index = curId&0xff;

    if(seq >= config->list_cabinet.count())
        return;
    if(index >= config->list_cabinet[seq]->list_case.count())
        return;

    btn->setText(QString("序号：%1\nIO号：%2").arg(config->list_cabinet.at(seq)->list_case.at(index)->ctrlSeq).arg(config->list_cabinet.at(seq)->list_case.at(index)->ctrlIndex));
}

void CabinetService::on_rebind_clicked()
{
    config->state = STATE_REBIND;
    config->showMsg(MSG_REBIND_SCAN,0);
    emit winSwitch(INDEX_CAB_SHOW);
}

void CabinetService::on_set_server_addr_clicked()
{
    QString strAddr = ui->server_addr->text();
//    if(strAddr.indexOf("http:") != 0)
//        strAddr = QString("http://") +strAddr;

    config->setServerAddress(strAddr);
    qDebug()<<"setServerAddress"<<ui->server_addr->text();
    emit requireUpdateServerAddress();
}

void CabinetService::on_server_addr_editingFinished()
{
    QString str = ui->server_addr->text();

    if(str.split('.').count() > 4)
    {
        int index = str.lastIndexOf('.');
        str.replace(index,1,':');
        ui->server_addr->setText(str);
    }
}

void CabinetService::on_insert_clicked()
{
    if(ui->insert->text().indexOf("插入") != -1)
    {
        ui->insert->setText("确定");
        return;
    }
    else
    {
        ui->insert->setText("正在插入");
        ui->insert->setEnabled(false);
        if(!inserCol(ui->insert_pos->value(), ui->insert_num->value()))
        {
            ui->insert->setText("确定");
            ui->insert->setEnabled(true);
        }
    }
}

void CabinetService::on_insert_pos_valueChanged(int arg1)
{
    ui->insert_num->setMaximum(config->list_cabinet.count() - arg1);
    ui->insert_num->setMinimum(arg1-config->list_cabinet.count());
}

void CabinetService::on_insert_num_valueChanged(int arg1)
{
    ui->insert_pos->setMaximum(config->list_cabinet.count() - qAbs(arg1));
}
