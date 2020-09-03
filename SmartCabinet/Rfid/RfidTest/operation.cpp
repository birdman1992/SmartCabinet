#include "operation.h"
#include "ui_operation.h"

Operation::Operation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Operation)
{
    ui->setupUi(this);
}

Operation::~Operation()
{
    delete ui;
}
