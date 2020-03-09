#include "cabinetset.h"
#include "ui_cabinetset.h"
#include "defines.h"
#include <QDebug>
#include <QHeaderView>
#include "cabinetserver.h"

CabinetSet::CabinetSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetSet)
{
    ui->setupUi(this);
    cabinet_pos.clear();
    cabinet_pos[0] = 0;
    initStep = 0;
    ui->finish->hide();
    ui->testStack->setCurrentIndex(1);
    dev_network = NULL;
    sTest = NULL;
    needSelScreen = true;
    screenPos = QPoint(-1,-1);
    //初始化柜格类型
    initCabType();

//    ui->save->hide();
//#ifdef SIMULATE_ON
//    dev_network = new QNetInterface("eth0");
//#else
//    dev_network = new QNetInterface("eth1");
//#endif
//    dev_network->initNetwork();
//    dev_network->creatNetwork();

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

void CabinetSet::show()
{
    this->showFullScreen();
}

bool CabinetSet::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    checkDevice();

    QStringList proList;
    proList<<"spd-web"<<"cheset-admin"<<"hos-admin";
    ui->apiProName->addItems(proList);
    ui->apiProName->setCurrentIndex(0);

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

void CabinetSet::cloneResult(bool isSuccess, QString msg)
{
    if(isSuccess)
    {
        ui->save->setEnabled(false);
        ui->cloneMsg->setText("克隆成功");
        ui->cloneStart->setText("确定");
        ui->testStack->setCurrentIndex(0);
        ui->finish->show();
        config->setCabinetId(ui->cloneId->text());
        return;
    }
    ui->cloneMsg->setText(msg);
    resetRegState();
}

void CabinetSet::regResult(bool isSuccess)
{
    ui->regId->setEnabled(true);
    if(isSuccess)
    {
        QString msg = QString("ID注册成功：\n%1").arg(config->getCabinetId());
        ui->regMsg->setText(msg);
        ui->testStack->setCurrentIndex(0);
        ui->finish->show();
    }
    else
    {
        ui->regMsg->setText("ID注册失败");
        resetRegState();
    }
}

//void CabinetSet::on_add_left_clicked()
//{
//    int i;
//    int j;

//    for(i=1 ;i<list_cabinet.count(); i+=2)
//    {
//        if(list_cabinet.at(i)->isHidden())
//        {
//            j = 1;
//            while(cabinet_pos[j]) j++;
//            cabinet_pos[j] = i;
//            list_cabinet.at(i)->show();
//            break;
//        }
//    }
//}

void CabinetSet::on_add_right_clicked()
{
    QTableWidget* tab = new QTableWidget();
    tab->setSelectionMode(QAbstractItemView::NoSelection);
    tab->resize(10,ui->cabs->geometry().height()-12);

    ui->cabs->layout()->addWidget(tab);
    cabSplit(ui->cabType->currentText(), tab);
    list_layout<<ui->cabType->currentText();
    list_cabinet<<tab;

    if((screenPos.y() >= 0) && needSelScreen)//已经选择了屏幕位置
    {
        needSelScreen = false;
        screenPos.setX(list_layout.count()-1);
        qDebug()<<"[screen]"<<screenPos.y();
        QTableWidgetItem* item = new QTableWidgetItem();
        item->setBackgroundColor(QColor(62, 155, 255));
        tab->setItem(screenPos.y(),0,item);
        ui->tabExp->clearSelection();
        warningSelScreen(false);
    }
}

void CabinetSet::on_clear_clicked()
{
    if(list_cabinet.isEmpty() || list_layout.isEmpty())
        return;

    QTableWidget* tab = list_cabinet.takeLast();
    ui->cabs->layout()->removeWidget(tab);
    delete tab;
    tab = NULL;
    list_layout.removeLast();
    if(list_cabinet.count() <= screenPos.x())
    {
        needSelScreen = true;
        screenPos.setX(-1);
        screenPos.setY(-1);
        warningSelScreen(true);
    }

//    cabinet_pos.clear();
//    cabinet_pos[0] = 0;
//    ui->add_right->setEnabled(true);
}

void CabinetSet::on_save_clicked()
{
    config->setServerAddress(ui->serverAddr->text());
    emit updateServerAddr();
    initStep |= 1;
    if(sTest != NULL)
        delete sTest;
//    else
//    {
    QString testApi = "http://" + ui->serverAddr->text() + QString("/%1/sarkApi/Time/query/").arg(config->getApiProName());
    sTest = new ServerTest(testApi, QByteArray(), this, NULL);
    connect(sTest, SIGNAL(apiMsg(QString)), ui->api_msg, SLOT(setText(QString)));
    connect(sTest, SIGNAL(pingMsg(QString)), ui->ping_msg, SLOT(setText(QString)));
    sTest->testStart();
//    }

    return;
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
        dev_network->setServerAddr(str.left(index));
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
    ui->mac->setText(dev_network->macAddress());
}

void CabinetSet::on_netSet_clicked()
{
    if(dev_network == NULL)
        return;

    QString ip = ui->ip->text();
    QString gateway = ui->gateway->text();
    QString netmask = ui->netmask->text();
    QString mac = ui->mac->text();

    dev_network->setMacAddress(mac);
    dev_network->setIp(ip);
    dev_network->setNetmask(netmask);
    dev_network->setGateway(gateway);
//    dev_network->setServerAddr();
    dev_network->saveNetwork();

//    ui->ip->setText(dev_network->ip());
//    ui->netmask->setText(dev_network->netmask());
//    ui->gateway->setText(dev_network->gateway());
//    ui->mac->setText(dev_network->macAddress());
//    on_netUpdate_clicked();
    QTimer::singleShot(5000, this, SLOT(on_netUpdate_clicked()));

//    qDebug()<<dev_network->ip()<<dev_network->netmask()<<dev_network->gateway();
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

void CabinetSet::initCabType()
{
    cabTypeList<<"331111"<<"31111111"<<"21111112"<<"2211112"<<"1111";
    ui->cabType->addItems(cabTypeList);
}

void CabinetSet::cabSplit(QString scale, QTableWidget *table)
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

int CabinetSet::getBaseCount(QString scale)
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

void CabinetSet::warningSelScreen(bool waringOn)
{
    ui->screen_sel_warning->setVisible(waringOn);
}

void CabinetSet::on_devState_toggled(bool checked)
{
    if(checked)
        ui->devState->setText("设备异常");
    else
        ui->devState->setText("设备正常");
}

void CabinetSet::on_cloneStart_clicked()
{
    if(ui->cloneId->text().isEmpty())
    {
        ui->cloneMsg->setText("克隆ID为空");
        return;
    }
//    if(!(initStep & (1<<1)))
//    {
//        ui->cloneMsg->setText("请先配置柜格布局");
//        return;
//    }
    ui->regId->setEnabled(false);
    ui->cloneMsg->show();
    ui->regMsg->hide();

    emit cabinetClone(ui->cloneId->text());
}

void CabinetSet::on_regId_clicked()
{
    qDebug()<<"[reg]"<<initStep;
    if(!(initStep & (1<<1)))
    {
        ui->regMsg->setText("请先配置柜格布局");
        return;
    }
    ui->cloneMsg->hide();
    ui->regMsg->show();
    ui->regId->setEnabled(false);
    emit requireCabRigster();
    QTimer::singleShot(10000, this, SLOT(resetRegState()));
}

void CabinetSet::on_savePos_clicked()
{
    if(!(initStep & 1))
    {
        ui->posMsg->setText("请先配置服务器地址");
        if(ui->aio_mode->isChecked())
            ui->aio_mode->setChecked(false);
        return;
    }
    if(list_layout.isEmpty() || screenPos.x()<0 || screenPos.y()<0)
    {
        ui->posMsg->setText("无效配置");
        return;
    }

    qDebug()<<cabinet_pos.toHex();
    config->setScreenPos(screenPos.x(),screenPos.y());
//    config->creatCabinetConfig(cabinet_pos);
//    config->clearConfig();
    config->creatCabinetConfig(list_layout, screenPos);
    config->readCabinetConfig();
    initStep |= (1<<1);
    ui->posMsg->setText("保存成功");
    qDebug()<<"creat over";
}

void CabinetSet::on_finish_clicked()
{
    emit cabinetCreated();
    if(config->getCabinetMode() == "cabinet")
        emit winSwitch(INDEX_CAB_SHOW);
    else
        emit winSwitch(INDEX_AIO);
    config->cabVoice.voicePlay(VOICE_WELCOME);
    sTest->testFinish();
    sTest->deleteLater();
    sTest = NULL;
}

void CabinetSet::on_cabType_currentIndexChanged(int)
{
    qDebug()<<ui->cabType->currentText();
    cabSplit(ui->cabType->currentText(), ui->tabExp);
}

void CabinetSet::on_tabExp_clicked(const QModelIndex &index)
{
    qDebug()<<index.row()<<index.column();
    if(!needSelScreen)
    {
        ui->tabExp->clearSelection();
        return;
    }

    if(index.row() == screenPos.y())
    {
        ui->tabExp->clearSelection();
        screenPos.setY(-1);
        screenPos.setX(-1);
        return;
    }

    screenPos.setY(index.row());
}


void CabinetSet::on_cabType_2_activated(int index)
{
    QString str = "RXS";
    QString strId = ui->cloneId->text();

    if(!strId.isEmpty())
    {
        QRegExp rx("[RXS]\\d*");
        int idx = rx.indexIn(strId);
        qDebug()<<index;
        if(idx == -1)//开头没有RXS
        {
            if(index != 0)
            {
                strId.insert(0, str.at(index-1));
            }
        }
        else
        {
            if(index != 0)
            {
                strId[0] = str.at(index-1);
            }
            else
            {
                strId.remove(0,1);
            }
        }
    }
    config->setCabinetType(ui->cabType_2->currentIndex()+1);
}

void CabinetSet::on_cloneId_textChanged(const QString &arg1)
{
    QString str = "RXS";
    QString strId = arg1;
    int index = ui->cabType_2->currentIndex();
    QRegExp rx("[RXS][\\d*]");
    int idx = rx.indexIn(strId);
    qDebug()<<index;
    if(idx == -1)//开头没有RXS
    {
        if(index != 0)
        {
            strId.insert(0, str.at(index-1));
        }
    }
    else
    {
        if(index != 0)
        {
            strId[0] = str.at(index-1);
        }
        else
        {
            strId.remove(0,1);
        }
    }
}

void CabinetSet::resetRegState()
{
    ui->regId->setEnabled(true);
}

void CabinetSet::layoutInit()
{
    ui->add_right->setEnabled(true);
    ui->clear->setEnabled(true);
    ui->savePos->setEnabled(true);
    if(!list_cabinet.isEmpty())
    {
        foreach(QTableWidget* w, list_cabinet)
        {
            ui->cabs->layout()->removeWidget(w);
            w->deleteLater();
        }
        list_cabinet.clear();
        list_layout.clear();
    }
    warningSelScreen(false);
    screenPos = QPoint(-1,-1);
    needSelScreen = true;
}

void CabinetSet::on_aio_mode_toggled(bool checked)
{
    if(checked)
    {
        ui->cabType_2->hide();
        ui->label_22->hide();
        config->setCabinetMode("aio");
        layoutInit();
        screenPos.setX(0);
        screenPos.setY(1);

        QTableWidget* tab = new QTableWidget();
        tab->setSelectionMode(QAbstractItemView::NoSelection);
        tab->resize(10,ui->cabs->geometry().height()-12);

        ui->cabs->layout()->addWidget(tab);
        cabSplit("1111", tab);
        list_layout<<QString("1111");
        list_cabinet<<tab;

        needSelScreen = false;
        QTableWidgetItem* item = new QTableWidgetItem();
        item->setBackgroundColor(QColor(62, 155, 255));
        tab->setItem(screenPos.y(),0,item);
        warningSelScreen(false);
        on_savePos_clicked();
        ui->add_right->setEnabled(false);
        ui->clear->setEnabled(false);
        ui->savePos->setEnabled(false);
    }
    else
    {
        layoutInit();
        ui->cabType_2->show();
        ui->label_22->show();
        config->setCabinetMode("cabinet");
        initStep &= ~(1<<1);
    }
}

void CabinetSet::on_apiProName_activated(const QString &arg1)
{
    config->setApiProName(arg1);
    emit updateServerAddr();
}
