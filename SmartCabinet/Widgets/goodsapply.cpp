#include "goodsapply.h"
#include "ui_goodsapply.h"

GoodsApply::GoodsApply(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GoodsApply)
{
    ui->setupUi(this);
}

GoodsApply::~GoodsApply()
{
    delete ui;
}

void GoodsApply::recvSearchRst()
{

}

void GoodsApply::on_searchText_textChanged(const QString &arg1)
{
    emit searchRequire(arg1);
}
