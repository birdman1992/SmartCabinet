#include "checkwarning.h"
#include "ui_checkwarning.h"
#include <QPainter>

CheckWarning::CheckWarning(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CheckWarning)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
}

CheckWarning::~CheckWarning()
{
    delete ui;
}

void CheckWarning::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.fillRect(this->rect(), QColor(22, 52, 73, 200));
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CheckWarning::warnningMsg(QString msg, bool pushReady)
{
    ui->msg->setText(msg);
    ui->push->setText("确认提交");
    ui->push->setEnabled(pushReady);
    this->showFullScreen();
}

void CheckWarning::checkSuccess(bool success)
{
    if(success)
    {
        ui->msg->setText("盘点成功");
        ui->push->setText("关闭");
    }
    else
    {
        ui->msg->setText("盘点失败");
    }

}

void CheckWarning::on_back_clicked()
{
    this->close();
}

void CheckWarning::on_push_clicked()
{
    if(ui->push->text() == "确认提交")
        emit pushCheck();
    else if(ui->push->text() == "关闭")
    {
        this->close();
    }
}
