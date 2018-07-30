#include "keyboad.h"
#include "ui_keyboad.h"
#include <QPushButton>
#include <QDebug>

Keyboad::Keyboad(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Keyboad)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    initKeys();
}

Keyboad::~Keyboad()
{
    delete ui;
}

void Keyboad::initKeys()
{
    btnGroup = new QButtonGroup(this);

    foreach(QObject* obj, ui->frame->children())
    {
        if(obj->objectName() != "gridLayout")
        {
            btnGroup->addButton((QPushButton*)obj);
        }
    }
    connect(btnGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(keyClicked(QAbstractButton*)));
}

void Keyboad::keyClicked(QAbstractButton *b)
{
    emit clicked(b->text());
}
