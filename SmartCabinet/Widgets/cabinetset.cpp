#include "cabinetset.h"
#include "ui_cabinetset.h"
#include "defines.h"
#include <QDebug>

CabinetSet::CabinetSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetSet)
{
    ui->setupUi(this);
    cabinet_pos.clear();
    cabinet_pos[0] = 0;
    dev_network = NULL;
    ui->save->hide();
    list_cabinet<< ui->label_0 << ui->label_1<< ui->label_2 << ui->label_3 << ui->label_4;
#ifdef SIMULATE_ON
    dev_network = new QNetInterface("eth0");
#else
    dev_network = new QNetInterface("eth1");
#endif
    dev_network->initNetwork();

    ui->label_1->hide();
    ui->label_2->hide();
    ui->label_3->hide();
    ui->label_4->hide();
    ui->devState->setEnabled(false);

    group_lock.addButton(ui->lock_test_2 , 0);
    group_lock.addButton(ui->lock_test_3 , 1);
    group_lock.addButton(ui->lock_test_4 , 2);
    group_lock.addButton(ui->lock_test_5 , 3);
    group_lock.addButton(ui->lock_test_6 , 4);
    group_lock.addButton(ui->lock_test_7 , 5);
    group_lock.addButton(ui->lock_test_8 , 6);
    group_lock.addButton(ui->lock_test_9 , 7);
    group_lock.addButton(ui->lock_test_10 , 8);
    group_lock.addButton(ui->lock_test_11 , 9);
    group_lock.addButton(ui->lock_test_12 , 10);
    group_lock.addButton(ui->lock_test_13 , 11);
    group_lock.addButton(ui->lock_test_14 , 12);
    group_lock.addButton(ui->lock_test_15 , 13);
    group_lock.addButton(ui->lock_test_16 , 14);
    group_lock.addButton(ui->lock_test_17 , 15);


    connect(&group_lock, SIGNAL(buttonClicked(int)), this, SLOT(on_lock_group_clicked(int)));
    on_netUpdate_clicked();
}

CabinetSet::~CabinetSet()
{
    delete ui;
    if(dev_network != NULL)
        delete dev_network;
}

bool CabinetSet::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    checkDevice();

    return true;
}

void CabinetSet::getCardId(QByteArray id)
{
    ui->cardId->setText(id);
}

void CabinetSet::getCodeScanData(QByteArray code)
{
    ui->scanCode->setText(code);
}

void CabinetSet::on_add_left_clicked()
{
    int i;
    int j;

    for(i=1 ;i<list_cabinet.count(); i+=2)
    {
        if(list_cabinet.at(i)->isHidden())
        {
            j = 1;
            while(cabinet_pos[j]) j++;
            cabinet_pos[j] = i;
            list_cabinet.at(i)->show();
            break;
        }
    }
    if((i+2)>=list_cabinet.count())
        ui->add_left->setEnabled(false);
}

void CabinetSet::on_add_right_clicked()
{
    int i;
    int j;

    for(i=2 ;i<list_cabinet.count(); i+=2)
    {
        if(list_cabinet.at(i)->isHidden())
        {
            j = 1;
            while(cabinet_pos[j]) j++;
            cabinet_pos[j] = i;
            list_cabinet.at(i)->show();
            break;
        }
    }
    if((i+2)>=list_cabinet.count())
        ui->add_right->setEnabled(false);
}

void CabinetSet::on_clear_clicked()
{
    cabinet_pos.clear();
    cabinet_pos[0] = 0;
    ui->label_1->hide();
    ui->label_2->hide();
    ui->label_3->hide();
    ui->label_4->hide();
    ui->add_left->setEnabled(true);
    ui->add_right->setEnabled(true);
}

void CabinetSet::on_save_clicked()
{
    qDebug()<<cabinet_pos.toHex();
    config->creatCabinetConfig(cabinet_pos);
    qDebug()<<"creat over";
//    emit setCabinet(cabinet_pos);
    emit updateServerAddr(ui->serverAddr->text());
    emit cabinetCreated();
    emit winSwitch(INDEX_CAB_SHOW);
    config->cabVoice.voicePlay(VOICE_WELCOME);
}

void CabinetSet::on_serverAddr_editingFinished()
{
    if(!ui->serverAddr->text().isEmpty())
        ui->save->show();

    QString str = ui->serverAddr->text();

    if(str.split('.').count() > 4)
    {
        int index = str.lastIndexOf('.');
        str.replace(index,1,':');
        ui->serverAddr->setText(str);
    }
    config->clearCabinet();
}

void CabinetSet::on_lock_test_clicked()
{
    emit lockTest();
}

void CabinetSet::on_lock_group_clicked(int id)
{
    emit requireOpenCase(ui->comboBox->currentIndex(), id);
}

void CabinetSet::on_pushButton_clicked()
{
    ui->cardId->clear();
    ui->scanCode->clear();
}

void CabinetSet::on_netUpdate_clicked()
{
    if(dev_network != NULL)
        delete dev_network;
#ifdef SIMULATE_ON
    dev_network = new QNetInterface("eth0");
#else
    dev_network = new QNetInterface("eth1");
#endif

    ui->ip->setText(dev_network->ip());
    ui->netmask->setText(dev_network->netmask());
    ui->gateway->setText(dev_network->gateway());
}

void CabinetSet::on_netSet_clicked()
{
    if(dev_network == NULL)
        return;

    QString ip = ui->ip->text();
    QString gateway = ui->gateway->text();
    QString netmask = ui->netmask->text();

    dev_network->setIp(ip);
    dev_network->setGateway(gateway);
    dev_network->setNetmask(netmask);
    dev_network->saveNetwork();

    ui->ip->setText(dev_network->ip());
    ui->netmask->setText(dev_network->netmask());
    ui->gateway->setText(dev_network->gateway());
}

void CabinetSet::checkDevice()
{
    ui->devReader->setVisible(config->getCardReaderState());
    ui->devScan->setVisible(config->getCodeScanState());

    if(config->getCardReaderState() && config->getCodeScanState())
        ui->devState->setChecked(false);//设备正常
    else
        ui->devState->setChecked(true);//设备异常
}

void CabinetSet::on_devState_toggled(bool checked)
{
    if(checked)
        ui->devState->setText("设备异常");
    else
        ui->devState->setText("设备正常");
}
