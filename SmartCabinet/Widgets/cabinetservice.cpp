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

    sTest = NULL;

    cfg_layout = new QHBoxLayout();
    ui->ctrlCfg->setLayout(cfg_layout);

    dev_network = NULL;
    lockConfigIsOk = false;
    updateOk = false;

    win_ctrl_config = new CabinetCtrlConfig();
    connect(win_ctrl_config,SIGNAL(lockCtrl(int,int)),this, SIGNAL(requireOpenLock(int,int)));
    connect(win_ctrl_config, SIGNAL(updateBtn()), this,SLOT(updateBtn()));

    win_fingerPrint = new FingerPrint(this);
    connect(win_fingerPrint, SIGNAL(requireOpenLock(int,int)), this, SIGNAL(requireOpenLock(int,int)));
    connect(win_fingerPrint, SIGNAL(doorState(int)), this, SIGNAL(doorState(int)));//开关门控制led
    connect(win_fingerPrint, SIGNAL(userCardActive(QByteArray)), this, SIGNAL(userCardActive(QByteArray)));
    ui->frame_finger->layout()->addWidget(win_fingerPrint);

    SignalManager* sigMan = SignalManager::manager();
    connect(this, SIGNAL(rfidConfig()), sigMan, SIGNAL(configRfidDevice()));//

    initStack();
    initGroup();
    showVerInfo();

#ifdef TCP_API
    cabManager = CabinetManager::manager();
    QStringList types;
    types<<"331111"<<"31111111"<<"21111112"<<"2211112"<<"1111";
    ui->col_layout->addItems(types);
    nTab = new QTableWidget;
    nTab->setSelectionMode(QAbstractItemView::NoSelection);
    nTab->resize(7,270-10);
    ui->frame_new->show();
    ui->frame_master->hide();
#else
    ui->frame_new->hide();
    ui->frame_master->show();
#endif
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
    config->setCurVersion(ui->verInfo->text());
    win_ctrl_config->installGlobalConfig(config);
#ifdef TCP_API
    ui->insert_pos->setMaximum(config->list_cabinet.count());
#else
    ui->insert_pos_2->setMaximum(config->list_cabinet.count() - 1);
#endif
    QStringList proList;
    proList<<"spd-web"<<"cheset-admin"<<"hos-admin";
    ui->proName->addItems(proList);
    int curIndex = proList.indexOf(config->getApiProName());
    curIndex = (curIndex<0)?0:curIndex;
    ui->proName->setCurrentIndex(curIndex);
    return true;
}


void CabinetService::on_back_clicked()
{
    if(config->getCabinetType().at(BIT_CAB_AIO))
        emit winSwitch(INDEX_AIO);
    else
        emit winSwitch(INDEX_CAB_SHOW);

    if(sTest != NULL)
    {
        sTest->testFinish();
        delete sTest;
        sTest = NULL;
    }
}

void CabinetService::showEvent(QShowEvent *)
{
    initNetwork();
    creatCtrlConfig();
#ifdef TCP_API
    if(!list_preview.isEmpty())
    {
        qDeleteAll(list_preview.begin(), list_preview.end());
        list_preview.clear();
    }
    list_preview = creatPreviewList(config->getCabinetLayout().split('#', QString::SkipEmptyParts));
    updateCabpreview(NULL, 0);
    updateCabpreviewScr();
#endif
    ui->server_addr->setText(config->getServerAddress());
    ui->version_msg->clear();
    ui->check->setEnabled(true);
    if(config->getStoreMode())//scan all
    {
        ui->scan_all->setChecked(true);
    }
    else
    {
        ui->scan_list->setChecked(false);
    }

    int funcWord = config->getFuncWord();
    ui->func_back->setChecked(funcWord & funcBack);
    ui->func_check->setChecked(funcWord & funcCheck);
    ui->func_refun->setChecked(funcWord & funcRefun);
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
    dev_mac = dev_network->macAddress();
    qDebug()<<dev_ip<<dev_gateway<<dev_netmask;
    ui->addr->setText(dev_ip);
    ui->netmask->setText(dev_netmask);
    ui->gateway->setText(dev_gateway);
    ui->mac->setText(dev_mac);
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

        for(j=0; j<list_temp[i]->rowCount(); j++)
        {
            QString layoutStrech = list_temp.at(i)->getLayout();
            QPushButton* btn = new QPushButton(this);
            btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            btn->setMaximumWidth(400);
            QString str = QString("序号：%1\nIO号：%2").arg(list_temp.at(i)->ctrlSeq(j)).arg(list_temp.at(i)->ctrlIndex(j));
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

        for(j=0; j<config->list_cabinet[i]->rowCount(); j++)
        {
                QPushButton* btn = new QPushButton(this);
                btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
                btn->setMaximumWidth(400);
                QString str = QString("序号：%1\nIO号：%2").arg(config->list_cabinet.at(i)->ctrlSeq(j)).arg(config->list_cabinet.at(i)->ctrlIndex(j));
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

        for(j=0; j<config->list_cabinet[i]->rowCount(); j++)
        {
                QPushButton* btn = new QPushButton(this);
                btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
                btn->setMaximumWidth(400);
                QString str = QString("序号：%1\nIO号：%2").arg(config->list_cabinet.at(i)->ctrlSeq(j)).arg(config->list_cabinet.at(i)->ctrlIndex(j));
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
    qDebug()<<"[VERSION]:"<<info;
}

bool CabinetService::inserCol(int pos, int num)
{
    if(!(num))
        return false;
    else
        emit requireInsertCol(pos, num);
    return true;
}

bool CabinetService::inserCol(int pos, QString layout)
{
    emit requireInsertCol(pos, layout);
    return true;
}

void CabinetService::saveInsert()
{
    nTab->setStyleSheet("");
    list_preview.insert(insert_pos, nTab);
    nTab = new QTableWidget;
    ui->insert_pos->setMaximum(list_preview.count());
}

void CabinetService::setFuncWord(FuncWord word, bool isEnabled)
{
    int funcWord = config->getFuncWord();
    if(isEnabled)
    {
        funcWord |= word;
    }
    else
    {
        funcWord &= (~word);
    }
    config->setFuncWord(funcWord);
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

void CabinetService::on_insert_num_2_valueChanged(int arg1)
{
    ui->insert_pos_2->setMaximum(config->list_cabinet.count() - qAbs(arg1));
}

void CabinetService::on_insert_pos_2_valueChanged(int arg1)
{
    ui->insert_num_2->setMaximum(config->list_cabinet.count() - arg1);
    ui->insert_num_2->setMinimum(arg1-config->list_cabinet.count());
}

//void CabinetService::on_addr_textEdited(const QString &arg1)
//{
//    qDebug()<<arg1;
//}

void CabinetService::on_ok_clicked()
{
    if(dev_network == NULL)
        return;

    QString ip = ui->addr->text();
    QString gateway = ui->gateway->text();
    QString netmask = ui->netmask->text();
    QString mac = ui->mac->text();

    dev_network->setMacAddress(mac);
    dev_network->setIp(ip);
    dev_network->setNetmask(netmask);
    dev_network->setGateway(gateway);
    dev_network->saveNetwork();

    QTimer::singleShot(5000, this, SLOT(updateNetInfo()));
    return;
}

void CabinetService::on_cancel_clicked()
{

}

void CabinetService::tsCalibration()
{
    qDebug()<<"[tsCalibration]";
    QProcess process;
    process.start("rm /etc/pointercal");
    process.waitForFinished();
    QProcess::startDetached("reboot");
}

void CabinetService::recvVersionInfo(bool needUpdate, QString version)
{
    ui->check->setEnabled(true);
    if(needUpdate)
    {
        updateOk = true;
        ui->version_msg->setText(QString("可更新版本 %1").arg(version));
        ui->check->setText("开始更新");
    }
    else
    {
        updateOk = false;
        ui->version_msg->setText(QString("当前是最新版本"));
    }
}

void CabinetService::recvCurCardId(QByteArray cardId)
{
    win_fingerPrint->recvCurCardId(cardId);
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
                ::usleep(400000);//sleep和usleep都已经obsolete，Linux下也可以使用nanosleep代替
            }
            emit requireOpenLock(j, i);
        }
    }
}

void CabinetService::recvInsertColResult(bool success)
{
#ifdef TCP_API
    ui->insert->setEnabled(true);
    if(success)
    {
        ui->insert->setText("插入\n成功");
        saveInsert();
    }
    else
    {
        ui->insert->setText("插入\n失败");
    }
#else
    ui->insert_2->setEnabled(true);
    if(success)
    {
        ui->insert_2->setText("插入\n成功");
    }
    else
    {
        ui->insert_2->setText("插入\n失败");
    }
#endif
}

void CabinetService::recvInsertUndoResult(bool)
{
    ui->undo->setEnabled(true);
}

void CabinetService::on_rfid_config_clicked()
{
    emit rfidConfig();
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
    ui->version_msg->clear();
    if(updateOk)
    {
        ui->version_msg->setText("开始更新...");
        ui->check->setEnabled(false);
        updateOk = false;
        emit updateStart();
    }
    else
    {
        emit checkVersion(true);
        ui->check->setEnabled(false);
    }
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
    {
        qDebug()<<"updateBtn:no btn";
        return;
    }

    int seq = curId>>8;
    int index = curId&0xff;
//    qDebug()<<"[updateBtn]"<<curId<<seq<<index;
    if(seq >= config->list_cabinet.count())
        return;
    if(index >= config->list_cabinet[seq]->rowCount())
        return;

//    qDebug()<<"[updateBtn] success"<<config->list_cabinet.at(seq)->ctrlSeq(index)<<config->list_cabinet.at(seq)->ctrlIndex(index);
    btn->setText(QString("序号：%1\nIO号：%2").arg(config->list_cabinet.at(seq)->ctrlSeq(index)).arg(config->list_cabinet.at(seq)->ctrlIndex(index)));
}

void CabinetService::on_rebind_clicked()
{
    config->state = STATE_REBIND;
    config->showMsg(MSG_REBIND_SCAN,0);
    emit winSwitch(INDEX_CAB_SHOW);
}

void CabinetService::on_set_spec_case_clicked()
{
    config->state = STATE_SPEC;
    config->showMsg(MSG_SET_SPEC,0);
    emit winSwitch(INDEX_CAB_SHOW);
}

void CabinetService::on_set_server_addr_clicked()
{
    QString strAddr = ui->server_addr->text();
//    if(strAddr.indexOf("http:") != 0)
//        strAddr = QString("http://") +strAddr;
    int idx = strAddr.indexOf(":");
    if(idx != -1)
    {
        dev_network->setServerAddr(strAddr.left(idx));
    }
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
        if(!inserCol(insert_pos, insert_layout))
        {
            ui->insert->setText("确定");
            ui->insert->setEnabled(true);
        }
    }
}

void CabinetService::on_undo_clicked()
{
    cabManager->readConfig();
    int lastPos = cabManager->maxPos(cabManager->cabMap);
    if(lastPos == cabManager->scrPos.x())
        return;
    ui->undo->setEnabled(false);
    QTableWidget* oTab = list_preview.at(lastPos);
    list_preview.removeAt(lastPos);
    delete oTab;
    emit requireInsertUndo();
}

void CabinetService::on_insert_pos_valueChanged(int arg1)
{
    cabSplit(ui->col_layout->currentText(), nTab);
    insert_pos = arg1;
    insert_layout = ui->col_layout->currentText();
    updateCabpreview(nTab, arg1);
}

void CabinetService::on_col_layout_activated(const QString &arg1)
{
    cabSplit(arg1, nTab);
    insert_pos = ui->insert_pos->value();
    insert_layout = arg1;
    updateCabpreview(nTab, ui->insert_pos->value());
}

void CabinetService::cabSplit(QString scale, QTableWidget *table)
{
    if(scale.isEmpty()||(table == NULL))
    {
        return;
    }
    int rowCount = scale.length();
    int baseCount = getBaseCount(scale);
    int baseHeight = table->geometry().height()/baseCount;
    int i = 0;
    table->setRowCount(rowCount);
    table->setColumnCount(1);

    table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setVisible(false);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    qDebug()<<table->geometry().height()<<baseCount<<baseHeight;
    for(i=0; i<rowCount; i++)
    {
        table->setRowHeight(i,baseHeight*(scale.mid(i,1).toInt()));
    }

}

int CabinetService::getBaseCount(QString scale)
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

void CabinetService::updateCabpreview(QTableWidget* newTab, int pos)
{
    int i=0;
    if(!ui->layout_preview->layout()->children().isEmpty())
    {
        foreach(QObject* obj, ui->layout_preview->layout()->children())
        {
            ui->layout_preview->layout()->removeWidget((QWidget*)obj);
        }
    }

    foreach(QTableWidget* tab, list_preview)
    {
        if((i == pos) && (newTab != NULL))
        {
            ui->layout_preview->layout()->addWidget(newTab);
            newTab->setStyleSheet("background:blue;");
        }
        ui->layout_preview->layout()->addWidget(tab);
        i++;
    }
    if((i == pos) && (newTab != NULL))
    {
        ui->layout_preview->layout()->addWidget(newTab);
        newTab->setStyleSheet("background:blue;");
    }

}

void CabinetService::updateCabpreviewScr()
{
    QPoint sPos = cabManager->getScrPos();qDebug("1");
    if(sPos.x()>=list_preview.count())
        return;
    QTableWidgetItem* item = new QTableWidgetItem();
    item->setBackgroundColor(QColor(62, 155, 255));
    list_preview.at(sPos.x())->setItem(sPos.y(), 0, item);
}

QList<QTableWidget *> CabinetService::creatPreviewList(QStringList layouts)
{
    QTableWidget* tab;
    QList<QTableWidget*> ret;
    qDebug()<<"[creatPreviewList]"<<layouts;

    foreach(QString layout, layouts)
    {
        tab = new QTableWidget();
        tab->setSelectionMode(QAbstractItemView::NoSelection);
        tab->resize(7,270-10);
        cabSplit(layout, tab);
        ret<<tab;
    }
    return ret;
//    if((screenPos.y() >= 0) && needSelScreen)//已经选择了屏幕位置
//    {
//        needSelScreen = false;
//        screenPos.setX(list_layout.count()-1);
//        qDebug()<<"[screen]"<<screenPos.y();
//        QTableWidgetItem* item = new QTableWidgetItem();
//        item->setBackgroundColor(QColor(62, 155, 255));
//        tab->setItem(screenPos.y(),0,item);
//        ui->tabExp->clearSelection();
//        warningSelScreen(false);
//    }
}


void CabinetService::on_insert_2_clicked()
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
        if(!inserCol(ui->insert_pos_2->value(), ui->insert_num_2->value()))
        {
            ui->insert->setText("确定");
            ui->insert->setEnabled(true);
        }
    }
}

void CabinetService::on_testServer_clicked()
{
    if(sTest != NULL)
        delete sTest;

    QString testApi = ui->server_addr->text() + "/sarkApi/Time/query/";
    sTest = new ServerTest(testApi, QByteArray(), this, NULL);
    connect(sTest, SIGNAL(apiMsg(QString)), ui->api_msg, SLOT(setText(QString)));
    connect(sTest, SIGNAL(pingMsg(QString)), ui->ping_msg, SLOT(setText(QString)));
    connect(sTest, SIGNAL(responseTime(QString)), ui->response, SLOT(setText(QString)));
    sTest->testStart();
}

void CabinetService::on_scan_list_toggled(bool checked)
{
    if(checked)
    {
        config->setStoreMode(false);
    }
}

void CabinetService::on_scan_all_toggled(bool checked)
{
    if(checked)
    {
        config->setStoreMode(true);
    }
}

//void CabinetService::on_proName_currentTextChanged(const QString &arg1)
//{
//    qDebug()<<"on_proName_currentTextChanged"<<arg1;
//    config->setApiProName(arg1);
//    emit requireUpdateServerAddress();
//}

void CabinetService::on_proName_activated(const QString &arg1)
{
//    qDebug()<<"on_proName_activated"<<arg1;
    config->setApiProName(arg1);
    emit requireUpdateServerAddress();
}

void CabinetService::on_func_refun_clicked(bool checked)
{
    setFuncWord(funcRefun, checked);
}

void CabinetService::on_func_back_clicked(bool checked)
{
    setFuncWord(funcBack, checked);
}

void CabinetService::on_func_check_clicked(bool checked)
{
    setFuncWord(funcCheck, checked);
}

void CabinetService::on_func_apply_toggled(bool checked)
{
    setFuncWord(funcApply, checked);
}
