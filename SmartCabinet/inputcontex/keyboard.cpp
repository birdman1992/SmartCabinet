#include "keyboard.h"
#include "ui_keyboard.h"
#include <QDebug>

KeyBoard::KeyBoard(QWidget *parent) :
    QWidget(parent),lastFocusedWidget(0),
    ui(new Ui::KeyBoard)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);

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
    group_key.addButton(ui->key_10, 10);
    group_key.addButton(ui->key_back, 11);
    group_key.addButton(ui->key_enter, 12);
    connect(&group_key, SIGNAL(buttonClicked(int)), this,SLOT(onKeysClicked(int)));
//    connect(&group_key, SIGNAL(buttonPressed(int)), this, SLOT(onKeysPressed(int)));
//    connect(&group_key, SIGNAL(buttonReleased(int)), this, SLOT(onKeysReleased(int)));
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)),
            this, SLOT(saveFocusWidget(QWidget*,QWidget*)));
}

KeyBoard::~KeyBoard()
{
    delete ui;
}

bool KeyBoard::event(QEvent *e)
{
    switch (e->type()) {
//! [1]
    case QEvent::WindowActivate:
        if (lastFocusedWidget)
            lastFocusedWidget->activateWindow();
        break;
//! [1]
    default:
        break;
    }

    return QWidget::event(e);
}

void KeyBoard::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void KeyBoard::onKeysClicked(int val)
{
    if(val<10)
    {
        emit key(QString::number(val).at(0).unicode());
    }
    else if(val == 10)
    {
        emit key(QChar('.').unicode());
    }
    else if(val == 11)
    {
        emit key(Qt::Key_Backspace);
    }
    else if(val == 12)
    {
        emit key(Qt::Key_Enter);
        this->hide();
    }
}

void KeyBoard::onKeysPressed(int val)
{
    if(val<10)
    {
        emit keyPress(QString::number(val).at(0).unicode());
    }
    else if(val == 10)
    {
        emit keyPress(QChar('.').unicode());
    }
    else if(val == 11)
    {
        emit keyPress(Qt::Key_Backspace);
    }
    else if(val == 12)
    {
        emit keyPress(Qt::Key_Enter);
        this->hide();
    }
}

void KeyBoard::onKeysReleased(int val)
{
    if(val<10)
    {
        emit keyRelease(QString::number(val).at(0).unicode());
    }
    else if(val == 10)
    {
        emit keyRelease(QChar('.').unicode());
    }
    else if(val == 11)
    {
        emit keyRelease(Qt::Key_Backspace);
    }
    else if(val == 12)
    {
        emit keyRelease(Qt::Key_Enter);
    }
}

void KeyBoard::saveFocusWidget(QWidget * /*oldFocus*/, QWidget *newFocus)
{
    if (newFocus != 0 && !this->isAncestorOf(newFocus)) {
        lastFocusedWidget = newFocus;
    }
}

void KeyBoard::on_key_back_pressed()
{
    timer_backspace = new QTimer();
    timer_backspace->setSingleShot(true);
    timer_backspace->start(500);

    connect(timer_backspace, SIGNAL(timeout()), this, SLOT(backspace_timeout()));
}

void KeyBoard::on_key_back_released()
{
    disconnect(timer_backspace, SIGNAL(timeout()), this, SLOT(backspace_timeout()));
    timer_backspace->stop();
    delete timer_backspace;
}

void KeyBoard::backspace_timeout()
{
    timer_backspace->start(80);
    emit key(Qt::Key_Backspace);
}
