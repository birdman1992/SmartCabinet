#include "standbywidget.h"
#include "ui_standbywidget.h"
#include <QTimer>
#include <QDebug>
#include "defines.h"

StandbyWidget::StandbyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StandbyWidget)
{
    ui->setupUi(this);
    waitForCardReader = false;
    msgBox = NULL;
}

StandbyWidget::~StandbyWidget()
{
    delete ui;
}

void StandbyWidget::on_cun_clicked()//存放按钮
{
    waitForCardReader = true;
    config->state = STATE_STORE;
    msgBox = new QMessageBox(QMessageBox::NoIcon, "身份验证", "请刷卡验证身份",QMessageBox::Ok,NULL,
           Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
    msgBox->setModal(false);
    msgBox->show();
    QTimer::singleShot(3000,this, SLOT(wait_timeout()));
}

void StandbyWidget::warningMsgBox(QString title, QString msg)
{
    QMessageBox* box = new QMessageBox(QMessageBox::NoIcon, title, msg,QMessageBox::Ok,NULL,
           Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
    box->setModal(true);
    box->exec();
    box->deleteLater();
}

void StandbyWidget::recvUserInfo(QByteArray qba)
{
    if(!waitForCardReader)
    {
        qDebug()<<"[StandbyWidget]"<<"recvUserInfo not need.";
        if(msgBox != NULL)
        {
            msgBox->close();
            msgBox->deleteLater();
            msgBox = NULL;
        }
        return;
    }

    int ret = config->checkUser(QString(qba));
    qDebug()<<"check"<<ret;

    if(ret < 0)
    {
        warningMsgBox("警告", "身份校验失败");
    }
    else
    {
        if(config->state == STATE_STORE)
            emit winSwitch(INDEX_CAB_SHOW);
        else if(config->state = STATE_FETCH)
            emit winSwitch(INDEX_CAB_SHOW);
    }

    waitForCardReader = false;

    if(msgBox != NULL)
    {
        msgBox->close();
        msgBox->deleteLater();
        msgBox = NULL;
    }
}

void StandbyWidget::wait_timeout()
{
    if(msgBox == NULL)
        return;

    if(!waitForCardReader)
        return;
    waitForCardReader = false;
    msgBox->close();
    msgBox->deleteLater();

    msgBox = new QMessageBox(QMessageBox::NoIcon, "等待超时", "身份校验超时",QMessageBox::Ok,NULL,
           Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
    msgBox->setModal(true);
    msgBox->exec();
    msgBox->deleteLater();
    msgBox = NULL;
}

bool StandbyWidget::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    return true;
}

void StandbyWidget::on_qu_clicked()
{
    waitForCardReader = true;
    config->state = STATE_FETCH;
    msgBox = new QMessageBox(QMessageBox::NoIcon, "身份验证", "请刷卡验证身份",QMessageBox::Ok,NULL,
           Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
    msgBox->setModal(false);
    msgBox->show();
    QTimer::singleShot(3000,this, SLOT(wait_timeout()));
}
