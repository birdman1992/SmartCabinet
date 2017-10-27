#include "coderkeyboard.h"
#include "ui_coderkeyboard.h"

coderKeyboard::coderKeyboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::coderKeyboard)
{
    ui->setupUi(this);
    str.clear();
    connect(&group_key, SIGNAL(buttonClicked(int)), this,SLOT(onKeysClicked(int)));
}

coderKeyboard::~coderKeyboard()
{
    delete ui;
}

void coderKeyboard::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void coderKeyboard::onKeysClicked(int val)
{
    if(val<10)
    {
        str += QString::number(val);
    }
//    else if(val == 10)
//        emit backspace();
//    else if(val == 11)
//        emit clearAll();
}
