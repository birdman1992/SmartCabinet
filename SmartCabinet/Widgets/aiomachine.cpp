#include "aiomachine.h"
#include "ui_aiomachine.h"

AIOMachine::AIOMachine(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AIOMachine)
{
    ui->setupUi(this);
}

AIOMachine::~AIOMachine()
{
    delete ui;
}

void AIOMachine::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
