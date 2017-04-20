#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    cabinets = new CabinetPanel(this);

    ui->caseLayout->addWidget(cabinets);
    cabinets->show();
}

MainWidget::~MainWidget()
{
    delete ui;
}
