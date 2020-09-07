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

QString Operation::curOperation()
{

}

void Operation::loadOperations()
{

}

void Operation::on_back_clicked()
{
    emit winClose();
}

void Operation::on_update_clicked()
{

}
