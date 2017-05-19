#include "cabinetwidget.h"
#include "ui_cabinetwidget.h"
#include <QDebug>
#include "defines.h"

//提示信息
#define MSG_STORE "请扫描待存放物品条形码 并放入存放柜 存放完毕请点击此处并关闭柜门"
#define MSG_STORE_SELECT "请选择存放位置 柜门打开后请重新扫描条形码"
#define MSG_STORE_SELECT_REPEAT "选择的位置被占用 请重新选择"
#define MSG_FETCH "请选择要取出的物品 柜门打开后请扫描条形码取出"
#define MSG_FETCH_SCAN "请扫描条形码取出物品 取用完毕请点击此处并关闭柜门"
#define MSG_FETCH_EMPTY "没有库存了 请关好柜门 点击此处退出"

CabinetWidget::CabinetWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetWidget)
{
    ui->setupUi(this);
    storeNum = 0;
    clickLock = true;
    waitForCodeScan = false;
    waitForGoodsListCode = false;
    waitForInit = true;
    msgBox = NULL;
    optUser = QString();
}

CabinetWidget::~CabinetWidget()
{
    delete ui;
}

//初始化药柜界面
void CabinetWidget::panel_init(QList<Cabinet *> cabinets)
{
    if(!waitForInit)
        return;
    qDebug("[panel_init]");
    waitForInit = false;
    int index = cabinets.count()-1;
    qDebug()<<"index"<<index;

    ui->cabinet_layout->addStretch();

    for(; index>0; index--)
    {
        if(cabinets.at(index)->isInLeft())
            ui->cabinet_layout->addWidget(cabinets.at(index));
    }
    ui->cabinet_layout->addWidget(cabinets.at(0));
    connect(cabinets.at(0), SIGNAL(logoClicked()), this, SLOT(logoClicked()));

    index = 1;
    for(; index<(cabinets.count()); index++)
    {
        if(!cabinets.at(index)->isInLeft())
            ui->cabinet_layout->addWidget(cabinets.at(index));
    }
    ui->cabinet_layout->addStretch();

    for(index=0; index<cabinets.count(); index++)
    {
        connect(cabinets.at(index), SIGNAL(caseSelect(int,int)), this, SLOT(caseClicked(int,int)));
    }
}

void CabinetWidget::caseClicked(int caseIndex, int cabSeqNum)
{
    if(clickLock)//锁定状态下点击无效
    {
        if((caseIndex==selectCase) && (cabSeqNum == selectCab))
            return;
        else
            config->list_cabinet.at(cabSeqNum)->clearSelectState(caseIndex);
        return;
    }

    bool clickRepeat = false;
    if((caseIndex==selectCase) && (cabSeqNum == selectCab))
        clickRepeat = true;//标记为重复点击
    if(!config->list_cabinet[cabSeqNum]->list_case[caseIndex]->name.isEmpty())
        clickRepeat = true;

    clickLock = true;
    selectCab = cabSeqNum;
    selectCase = caseIndex;
    qDebug()<<caseIndex<<cabSeqNum;

    if(config->state == STATE_STORE)
    {
        if(clickRepeat)
        {
            config->list_cabinet[0]->showMsg(MSG_STORE_SELECT_REPEAT, false);
            clickLock = false;
            return;
        }
        config->list_cabinet[selectCab]->setCaseName(scanInfo, selectCase);
//        config->list_cabinet[selectCab]->consumableIn(selectCase);
        config->list_cabinet[0]->showMsg(MSG_STORE, false);
    }
    else if(config->state == STATE_FETCH)
    {
        if(!clickRepeat)//如果该柜格没有存放药品
        {qDebug()<<"empty";
            selectCab = -1;
            selectCase = -1;
            clickLock = false;
            return;
        }
        //打开对应柜门
        qDebug()<<"[CabinetWidget]"<<"[open]"<<casePos.cabinetSeqNUM<<casePos.caseIndex;
        waitForCodeScan = true;
        scanInfo = QString();
        config->list_cabinet[0]->showMsg(MSG_FETCH_SCAN, false);
    }
}

void CabinetWidget::recvScanData(QByteArray qba)
{
    if(!waitForCodeScan)
    {
        qDebug()<<"[CabinetWidget]"<<"scan data not need";
        return;
    }
    bool newStore = false;
    if((scanInfo != QString(qba)) && (config->state != STATE_FETCH))
    {
        newStore = true;
        storeNum = 0;
    }
    scanInfo = QString(qba);
    casePos = config->checkCabinetByName(scanInfo);

    if(casePos.cabinetSeqNUM == -1)//没有搜索到药品对应的柜格
    {
        clickLock = false;
        config->list_cabinet[0]->showMsg(MSG_STORE_SELECT, false);
    }
    else
    {
        if(config->state == STATE_STORE)
        {
            //打开对应柜门
            qDebug()<<"[CabinetWidget]"<<"[open]"<<casePos.cabinetSeqNUM<<casePos.caseIndex;
            storeNum++;
            config->list_cabinet[0]->showMsg(MSG_STORE+
                                             QString("\n已放入\n%1 ×%2").arg(config->list_cabinet[casePos.cabinetSeqNUM]->list_case[casePos.caseIndex]->name).arg(storeNum), false);
            config->list_cabinet[casePos.cabinetSeqNUM]->consumableIn(casePos.caseIndex);
        }
        else if(config->state == STATE_FETCH)
        {
            if(config->list_cabinet[selectCab]->list_case[selectCase]->num == 0)
            {
                config->list_cabinet[0]->showMsg(MSG_FETCH_EMPTY, false);
                return;
            }
            if(config->list_cabinet[selectCab]->list_case[selectCase]->name != scanInfo)
            {
                return;
            }
            storeNum++;
            config->list_cabinet[0]->showMsg(MSG_FETCH_SCAN+
                                             QString("\n已取出\n%1 ×%2").arg(config->list_cabinet[selectCab]->list_case[selectCase]->name).arg(storeNum), false);
            config->list_cabinet[selectCab]->consumableOut(selectCase);
        }
    }
}

void CabinetWidget::logoClicked()
{//qDebug("logoClicked1");
    if(config->state == STATE_STORE)
    {
        config->state = STATE_NO;
        waitForCodeScan = false;
        storeNum = 0;
//        config->list_cabinet.at(selectCab)->clearSelectState(selectCase);
        emit winSwitch(INDEX_STANDBY);
    }
    else if(config->state == STATE_FETCH)
    {
        config->state = STATE_NO;
        waitForCodeScan = false;
        storeNum = 0;
//        config->list_cabinet.at(selectCab)->clearSelectState(selectCase);
        emit winSwitch(INDEX_STANDBY);
    }
//    qDebug("logoClicked2");
}

void CabinetWidget::cabinetInit()
{
    panel_init(config->list_cabinet);
}



void CabinetWidget::showEvent(QShowEvent *)
{
//    qDebug()<<"[CabinetWidget]"<<"showEvent";
    if(config->state == STATE_STORE)
    {
        waitForCodeScan = true;
        config->list_cabinet[0]->showMsg(MSG_STORE, false);
    }
    else if(config->state == STATE_FETCH)
    {
        clickLock = false;
        selectCab = -1;
        selectCase = -1;
        config->list_cabinet[0]->showMsg(MSG_FETCH, false);
    }
}

bool CabinetWidget::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    return true;
}

void CabinetWidget::caseLock()
{
    clickLock = true;
}

void CabinetWidget::caseUnlock()
{
    clickLock = false;
}

void CabinetWidget::on_store_clicked()
{
    waitForCardReader = true;
    config->state = STATE_STORE;
//    if(msgBox != NULL)
//    {
//        msgBox->close();
//        msgBox->deleteLater();
//        msgBox = NULL;
//    }
//    msgBox = new QMessageBox(QMessageBox::NoIcon, "身份验证", "请刷卡验证身份",QMessageBox::Ok,NULL,
//           Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
//    msgBox->setModal(false);
//    msgBox->show();
    msgShow("身份验证", "请刷卡验证身份",false);
    QTimer::singleShot(10000,this, SLOT(wait_timeout()));
}

void CabinetWidget::on_fetch_clicked()
{
    waitForCardReader = true;
    config->state = STATE_FETCH;
    msgShow("身份验证", "请刷卡验证身份",false);
//    msgBox = new QMessageBox(QMessageBox::NoIcon, "身份验证", "请刷卡验证身份",QMessageBox::Ok,NULL,
//           Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
//    msgBox->setModal(false);
//    msgBox->show();
    QTimer::singleShot(10000,this, SLOT(wait_timeout()));
}

void CabinetWidget::wait_timeout()
{
    if(msgBox == NULL)
        return;

    if(!waitForCardReader)
        return;
    waitForCardReader = false;
    msgShow("等待超时", "身份校验超时",false);
//    msgBox->close();
//    msgBox->deleteLater();
//    msgBox = NULL;

//    msgBox = new QMessageBox(QMessageBox::NoIcon, "等待超时", "身份校验超时",QMessageBox::Ok,NULL,
//           Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
//    msgBox->setModal(true);
//    msgBox->exec();
//    msgBox->deleteLater();
//    msgBox = NULL;
}

void CabinetWidget::warningMsgBox(QString title, QString msg)
{
    QMessageBox* box = new QMessageBox(QMessageBox::NoIcon, title, msg,QMessageBox::Ok,NULL,
           Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
    box->setModal(true);
    box->exec();
    box->deleteLater();
}

void CabinetWidget::msgClear()
{
    if(msgBox != NULL)
    {
        msgBox->close();
        msgBox->deleteLater();
        msgBox = NULL;
    }
}

void CabinetWidget::msgShow(QString title, QString msg, bool setmodal)
{
    msgClear();

    if(setmodal)
    {
        msgBox = new QMessageBox(QMessageBox::NoIcon, title, msg,QMessageBox::Ok,NULL,
                                 Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
        msgBox->setModal(true);
        msgBox->exec();
        msgClear();
    }
    else
    {
        msgBox = new QMessageBox(QMessageBox::NoIcon, title, msg,QMessageBox::Ok,NULL,
                                 Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
        msgBox->setModal(false);
        msgBox->show();
    }
}

void CabinetWidget::recvUserInfo(QByteArray qba)
{
    if(!waitForCardReader)
    {
        qDebug()<<"[StandbyWidget]"<<"recvUserInfo not need.";
        msgClear();
        return;
    }
//    waitForCardReader = false;
    optUser = QString(qba);
    msgShow("身份验证", "身份验证中...",false);
    emit requireUserCheck(optUser);
//    if(msgBox != NULL)
//    {
//        msgBox->close();
//        msgBox->deleteLater();
//        msgBox = NULL;
//    }
//    msgBox = new QMessageBox(QMessageBox::NoIcon, "身份验证", "身份验证中...",QMessageBox::Ok,NULL,
//           Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
//    msgBox->setModal(false);
//    msgBox->show();
}

void CabinetWidget::recvUserCheckRst(bool pass)
{
//    msgClear();

    if(pass)
    {
        msgShow("身份验证", "身份验证通过，请扫描送货单条码",true);
//        warningMsgBox("身份验证", "身份验证通过，请扫描送货单条码");
        waitForCodeScan = true;
        waitForGoodsListCode = true;
        waitForCardReader = false;
    }
    else
    {
        warningMsgBox("身份验证", "身份验证失败");
    }
    qDebug("rst");
}
