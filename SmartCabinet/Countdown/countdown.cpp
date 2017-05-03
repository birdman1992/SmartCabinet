#include "countdown.h"
#include "ui_countdown.h"

CountDown::CountDown(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CountDown)
{
    ui->setupUi(this);
     i = 30 ;
    ui->label->setText("请在30秒内扫描");
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(time_out()));
    timer->start(1000);
}

void CountDown::time_out()
{
    ui->label_2->setText(QString::number(i,10));
    i--;
    if(i == 0)
    {
        this->close();
    }
}

CountDown::~CountDown()
{
    delete ui;
}
