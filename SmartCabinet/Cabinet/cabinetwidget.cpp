#include "cabinetwidget.h"
#include "ui_cabinetwidget.h"
#include <QDebug>
#include "defines.h"

//提示信息
#define MSG_STORE "请扫描待存放物品条形码 并放入存放柜 存放完毕请点击此处并关闭柜门"
#define MSG_STORE_SELECT "请选择存放位置 柜门打开后请重新扫描条形码"
#define MSG_STORE_SELECT_REPEAT "选择的位置被占用 请重新选择"

CabinetWidget::CabinetWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetWidget)
{
    ui->setupUi(this);
    clickLock = false;
    waitForCodeScan = false;
    waitForInit = true;
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

    for(; index>0; index--)
    {
        if(index%2)
            ui->cabinet_layout->addWidget(cabinets.at(index));
    }
    ui->cabinet_layout->addWidget(cabinets.at(0));
    connect(cabinets.at(0), SIGNAL(logoClicked()), this, SLOT(logoClicked()));

    index = 2;
    for(; index<(cabinets.count()); index++)
    {
        if(!(index%2))
            ui->cabinet_layout->addWidget(cabinets.at(index));
    }

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
    }

    bool clickRepeat = false;
    if((caseIndex==selectCase) && (cabSeqNum == selectCab))
        clickRepeat = true;//标记为重复点击

    clickLock = true;
    selectCab = cabSeqNum;
    selectCase = caseIndex;
    qDebug()<<caseIndex<<cabSeqNum;

    if(config->state == STATE_STORE)
    {
        if(clickRepeat)
        {
            config->list_cabinet[0]->showMsg(MSG_STORE_SELECT_REPEAT, false);
        }
        config->list_cabinet[selectCab]->setCaseName(scanInfo, selectCase);
//        config->list_cabinet[selectCab]->consumableIn(selectCase);
        config->list_cabinet[0]->showMsg(MSG_STORE, false);
    }
}

void CabinetWidget::recvScanData(QByteArray qba)
{
    if(!waitForCodeScan)
    {
        qDebug()<<"[CabinetWidget]"<<"scan data not need";
    }
    bool newStore = false;
    if(scanInfo != QString(qba))
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
        //打开对应柜门
        qDebug()<<"[CabinetWidget]"<<"[open]"<<casePos.cabinetSeqNUM<<casePos.caseIndex;
        storeNum++;
        config->list_cabinet[0]->showMsg(MSG_STORE+
            QString("\n已放入\n%1 ×%2").arg(config->list_cabinet[selectCab]->list_case[selectCase]->name).arg(storeNum), false);
        config->list_cabinet[selectCab]->consumableIn(selectCase);
    }
}

void CabinetWidget::logoClicked()
{
    if(config->state == STATE_STORE)
    {
        config->state = STATE_NO;
        waitForCodeScan = false;
        config->list_cabinet.at(selectCab)->clearSelectState(selectCase);
        emit winSwitch(INDEX_STANDBY);
    }
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
