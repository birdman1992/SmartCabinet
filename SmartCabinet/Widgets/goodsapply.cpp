#include "goodsapply.h"
#include "ui_goodsapply.h"
#include <QDebug>

GoodsApply::GoodsApply(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GoodsApply)
{
    ui->setupUi(this);
    keyboad = new Keyboad();
    connect(keyboad, SIGNAL(clicked(QString)), this, SLOT(keyboadStr(QString)));
}

GoodsApply::~GoodsApply()
{
    delete keyboad;
    delete ui;
}

void GoodsApply::recvSearchRst(CheckList *l)
{
    if(!listSearch.isEmpty())
    {
        qDeleteAll(listSearch.begin(), listSearch.end());
        listSearch.clear();
    }
    listSearch = l->listInfo;
    setTableSearch(listSearch);
}

void GoodsApply::recvReplyRst(bool success, QString msg)
{
    if(success)
    {
        ui->msg->setStyleSheet("color: rgb(0, 204, 0);");
    }
    else
    {
        ui->msg->setStyleSheet("color: rgb(204, 0, 0);");
    }
    ui->msg->setText(msg);
}

void GoodsApply::show()
{
    this->showFullScreen();
}

void GoodsApply::keyboadStr(QString str)
{
    if(str == "关闭")
    {
        keyboad->hide();
    }
    else if(str == "清空")
    {
        ui->searchText->clear();
    }
    else if(str == "删除")
    {
        QString searchStr = ui->searchText->text();
        ui->searchText->setText(searchStr.left(searchStr.length()-1));
    }
    else
    {
        QString searchStr = ui->searchText->text();
        ui->searchText->setText(searchStr+str);
    }
}

void GoodsApply::on_searchText_textChanged(const QString &arg1)
{
    emit searchRequire(arg1);
}

void GoodsApply::setTableSearch(QList<GoodsCheckInfo *> l)
{
    QStringList names;
    ui->searchRst->clearContents();
    ui->searchRst->setRowCount(l.count());
    ui->searchRst->setColumnCount(1);

    if(l.isEmpty())
        return;

    for(int i=0; i<l.count(); i++)
    {
        ui->searchRst->setItem(i, 0, new QTableWidgetItem(QString("%1 ×%2").arg(l.at(i)->name).arg(l.at(i)->type)));
    }
}

void GoodsApply::pushTableInsert(GoodsCheckInfo *info)
{
    if(isRepeat(listPush, info))
        return;

    GoodsCheckInfo* nInfo = new GoodsCheckInfo(*info);
    listPush<<nInfo;
    updatePushTableRow(ui->replyTable->rowCount(), nInfo);
    ui->replyTable->resizeColumnsToContents();
}

void GoodsApply::pushTableDelete(int row)
{
    if(row>=ui->replyTable->rowCount())
        return;

    delete listPush[row];
    listPush.removeAt(row);
    ui->replyTable->removeRow(row);
}

void GoodsApply::updatePushTableRow(int row, GoodsCheckInfo *info)
{
    if(row >= ui->replyTable->rowCount())
    {
        ui->replyTable->setRowCount(row+1);
        info->num_in = 0;
        ui->replyTable->setItem(row, 0, new QTableWidgetItem(QString("%1 ×%2").arg(info->name).arg(info->type)));
        ui->replyTable->setItem(row, 1, new QTableWidgetItem(info->id));
        ui->replyTable->setItem(row, 2, new QTableWidgetItem(info->goodsSize));
        ui->replyTable->setItem(row, 3, new QTableWidgetItem(info->producerName));
        ui->replyTable->setItem(row, 4, new QTableWidgetItem(QString::number(info->num_cur)));
        ui->replyTable->setItem(row, 5, new QTableWidgetItem(QString::number(info->num_in)));
        ui->replyTable->setItem(row, 6, new QTableWidgetItem("-"));
        ui->replyTable->setItem(row, 7, new QTableWidgetItem("+"));
        ui->replyTable->item(row, 6)->setTextAlignment(Qt::AlignCenter);
        ui->replyTable->item(row, 7)->setTextAlignment(Qt::AlignCenter);
    }
    else
    {
        ui->replyTable->item(row,0)->setText(info->name);
        ui->replyTable->item(row,1)->setText(info->id);
        ui->replyTable->item(row,2)->setText(info->goodsSize);
        ui->replyTable->item(row,3)->setText(info->producerName);
        ui->replyTable->item(row,4)->setText(QString::number(info->num_cur));
        ui->replyTable->item(row,5)->setText(QString::number(info->num_in));
    }
}

bool GoodsApply::isRepeat(QList<GoodsCheckInfo *> l, GoodsCheckInfo *info)
{
    foreach(GoodsCheckInfo* i, l)
    {
        if(i->packageBarCode == info->packageBarCode)
            return true;
    }
    return false;
}

void GoodsApply::on_searchRst_cellClicked(int row, int)
{
    pushTableInsert(listSearch.at(row));
}

void GoodsApply::on_replyTable_cellClicked(int row, int column)
{
    if(column == 7)
    {
        ui->replyTable->item(row, 5)->setText(QString::number(listPush.at(row)->addPack()));
    }
    else if(column == 6)
    {
        int pNum = listPush.at(row)->redPack();
        if(pNum<0)
            pushTableDelete(row);
        else
            ui->replyTable->item(row, 5)->setText(QString::number(pNum));
    }
//        pushTableDelete(row);
//    else if(column == 5)
//    {
//        QTableWidgetItem *item = ui->replyTable->item(row, 5); //edit_row为想要编辑的行号
//        ui->replyTable->setCurrentCell(row, 5);
//        ui->replyTable->openPersistentEditor(item); //打开编辑项
//        ui->replyTable->editItem(item);
//        ui->replyTable->closePersistentEditor(item);
//    }
}

void GoodsApply::on_push_clicked()
{
    ui->replyTable->clearContents();
    ui->replyTable->setRowCount(0);
    if(listPush.isEmpty())
        return;

    foreach(GoodsCheckInfo* info, listPush)
    {
        if(info->num_pack == 0)
        {
            listPush.removeOne(info);
            delete info;
        }
    }
    if(listPush.isEmpty())
    {
        ui->msg->setStyleSheet("color: rgb(0, 204, 0);");
        ui->msg->setText("提交为空");
        return;
    }

    ui->msg->setStyleSheet("color: rgb(0, 204, 0);");
    ui->msg->setText("正在提交");

    emit replyRequire(listPush);
    emit searchRequire(ui->searchText->text());
    listPush.clear();
}

void GoodsApply::on_pushButton_clicked(bool checked)
{
    ui->searchText->clear();
    QPoint keyboadPos = ui->searchRst->mapToGlobal(ui->searchRst->rect().topRight());
    keyboad->move(keyboadPos);
    keyboad->show();
}

void GoodsApply::on_close_clicked()
{
    keyboad->hide();
    this->hide();
}
