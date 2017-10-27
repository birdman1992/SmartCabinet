#include "numkeyboard.h"
#include "ui_numkeyboard.h"

NumKeyboard::NumKeyboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NumKeyboard)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    group_key.addButton(ui->key_0, 0);
    group_key.addButton(ui->key_1, 1);
    group_key.addButton(ui->key_2, 2);
    group_key.addButton(ui->key_3, 3);
    group_key.addButton(ui->key_4, 4);
    group_key.addButton(ui->key_5, 5);
    group_key.addButton(ui->key_6, 6);
    group_key.addButton(ui->key_7, 7);
    group_key.addButton(ui->key_8, 8);
    group_key.addButton(ui->key_9, 9);
    group_key.addButton(ui->key_back, 10);
    group_key.addButton(ui->key_clear, 11);
    connect(&group_key, SIGNAL(buttonClicked(int)), this,SLOT(onKeysClicked(int)));
}

NumKeyboard::~NumKeyboard()
{
    delete ui;
}

void NumKeyboard::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void NumKeyboard::onKeysClicked(int val)
{
    if(val<10)
        emit key(val);
    else if(val == 10)
        emit backspace();
    else if(val == 11)
        emit clearAll();
}
