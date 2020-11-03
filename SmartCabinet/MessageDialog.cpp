#include "MessageDialog.h"
#include "ui_MessageDialog.h"
#include <QTimer>

MessageDialog::MessageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
}

MessageDialog::~MessageDialog()
{
    delete ui;
}

void MessageDialog::updateMessage()
{
    ui->lab_msg->setText(QString("%1(%2)").arg(message).arg(curCount));
}

/**
 * @brief MessageDialog::showFinish 显示完成
 */
void MessageDialog::showFinish()
{
    downCount = 0;
    curCount = 0;
    this->hide();
    killTimer(timerId);
}

/**
 * @brief MessageDialog::showMessage
 * @param msg   需要显示的消息
 * @param showS 显示时间，单位秒,0表示不使用倒计时
 */
void MessageDialog::showMessage(QString msg, int showS)
{
    message = msg;
    curCount = showS;
    downCount = showS;
    updateMessage();
    this->showMaximized();
}

void MessageDialog::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MessageDialog::timerEvent(QTimerEvent *)
{
    curCount--;
    if(curCount <= 0 && downCount)//倒计时被启动且小于等于0
    {
        showFinish();
    }
    else
    {
        updateMessage();
    }
}

void MessageDialog::showEvent(QShowEvent *)
{
    timerId = startTimer(1000);
}

void MessageDialog::on_close_clicked()
{
    showFinish();
}
