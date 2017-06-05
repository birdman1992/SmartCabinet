#include "userwidget.h"
#include "ui_userwidget.h"
#include <QTableWidgetItem>
#include <QDebug>
#include "Structs/userinfo.h"
#include "defines.h"

UserWidget::UserWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserWidget)
{
    ui->setupUi(this);
    config = NULL;
    firstUse = false;
    waitForCardReader = false;
    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("用户ID"));
//    listUpdate();
}

bool UserWidget::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;

    return true;
}

UserWidget::~UserWidget()
{
    delete ui;
}

void UserWidget::updateUserList()
{
    listUpdate();
}


void UserWidget::listUpdate()
{
    int i = 0;
    qDebug("1");
    if(!configCheck())
        return;

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(config->list_user.count());

    for(i=0; i<config->list_user.count(); i++)//将用户信息显示到表格
    {
        ui->tableWidget->setItem(i,0, new QTableWidgetItem(config->list_user.at(i)->cardId));
    }
}

bool UserWidget::configCheck()
{
    if(config == NULL)
    {
        qDebug()<<"[UserWidget]"<<"config not init";
        return false;
    }
    else
        return true;
}

void UserWidget::showEvent(QShowEvent*)
{
    if(!configCheck())
        return;

    if(config->isFirstUse())
    {
        firstUse = true;
        waitForCardReader = true;
        msgBox = new QMessageBox(QMessageBox::NoIcon, "添加管理员用户", "请刷卡添加管理员用户",QMessageBox::Ok,NULL,
               Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
        msgBox->setModal(false);
        msgBox->show();
    }
}

void UserWidget::recvUserInfo(QByteArray qba)
{
    if(!waitForCardReader)
    {
        qDebug()<<"[UserWidget]"<<"recvUserInfo not need.";
        return;
    }

    waitForCardReader = false;
    UserInfo* info = new UserInfo();
    info->cardId = QString(qba);
    config->addUser(info);
    listUpdate();
    msgBox->close();
    msgBox->deleteLater();
}

void UserWidget::on_addUser_clicked()
{
    waitForCardReader = true;
    msgBox = new QMessageBox(QMessageBox::NoIcon, "添加普通用户", "请刷卡添加普通用户",QMessageBox::Ok,NULL,
                         Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
    msgBox->setModal(false);
    msgBox->show();
}

void UserWidget::on_addOk_clicked()
{
    if(firstUse)
        emit winSwitch(INDEX_CAB_SET);
    else
        emit winSwitch(INDEX_STANDBY);
}
