#include "cabinetlistview.h"
#include "ui_cabinetlistview.h"
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QIcon>
#include <QFont>
#include <qscrollbar.h>

CabinetListView::CabinetListView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetListView)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    QFont font(QFont("微软雅黑"));
    font.setPixelSize(25);
    ui->list_goods->setFont(font);
    font.setPixelSize(20);
    ui->list_select->setFont(font);
    initButtons();

    QFile qssScrollbar(":/stylesheet/styleSheet/ScrollBar.qss");
    qssScrollbar.open(QIODevice::ReadOnly);
    QString style = QString(qssScrollbar.readAll());
    ui->list_goods->verticalScrollBar()->setStyleSheet(style);
    ui->list_select->verticalScrollBar()->setStyleSheet(style);
    qssScrollbar.close();

    cabFrame = NULL;
}

CabinetListView::~CabinetListView()
{
    delete ui;
}

bool CabinetListView::installGlobalConfig(CabinetConfig *globalConfig)
{
    if(globalConfig == NULL)
        return false;
    config = globalConfig;
    return true;
}

void CabinetListView::show()
{
    this->showFullScreen();
}

void CabinetListView::setCabView(QFrame *cab)
{
    cabFrame = cab;
    //    showCabView();
}

void CabinetListView::fetchSuccess()
{
    ui->msg->setText("取出成功");
    clearList();
    getCabList();
    updateCabList();

}

void CabinetListView::fetchFailed(QString msg)
{
    ui->msg->setText(msg);
}

void CabinetListView::setNetState(bool state)
{
    networkState = state;
}

void CabinetListView::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.fillRect(this->rect(), QColor(22, 52, 73, 200));
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

}

void CabinetListView::showEvent(QShowEvent *)
{
    clearList();
    getCabList();
    updateCabList();
    ui->msg->setText("");
    config->state = STATE_LIST;
}

void CabinetListView::initButtons()
{
    groupSearch.addButton(ui->pushButton_1,0);
    groupSearch.addButton(ui->pushButton_2,1);
    groupSearch.addButton(ui->pushButton_3,2);
    groupSearch.addButton(ui->pushButton_4,3);
    groupSearch.addButton(ui->pushButton_5,4);
    groupSearch.addButton(ui->pushButton_6,5);
    groupSearch.addButton(ui->pushButton_7,6);
    groupSearch.addButton(ui->pushButton_8,7);
    groupSearch.addButton(ui->pushButton_9,8);
    groupSearch.addButton(ui->pushButton_10,9);
    groupSearch.addButton(ui->pushButton_11,10);
    groupSearch.addButton(ui->pushButton_12,11);
    groupSearch.addButton(ui->pushButton_13,12);
    groupSearch.addButton(ui->pushButton_14,13);
    groupSearch.addButton(ui->pushButton_15,14);
    groupSearch.addButton(ui->pushButton_16,15);
    groupSearch.addButton(ui->pushButton_17,16);
    groupSearch.addButton(ui->pushButton_18,17);
    groupSearch.addButton(ui->pushButton_19,18);
    groupSearch.addButton(ui->pushButton_20,19);
    groupSearch.addButton(ui->pushButton_21,20);
    groupSearch.addButton(ui->pushButton_22,21);
    groupSearch.addButton(ui->pushButton_23,22);
    groupSearch.addButton(ui->pushButton_24,23);
    groupSearch.addButton(ui->pushButton_25,24);
    groupSearch.addButton(ui->pushButton_26,25);
    groupSearch.setExclusive(false);
    connect(&groupSearch, SIGNAL(buttonClicked(int)), this, SLOT(search(int)));
}

void CabinetListView::showCabView()
{
    if(cabFrame == NULL)
        return;

    QPixmap pixmap = QPixmap::grabWidget(cabFrame);
    pixmap = pixmap.scaled(ui->cabWin->width(),ui->cabWin->height());
    qDebug()<<"scaled"<<ui->cabWin->width()<<ui->cabWin->height();
//    ui->cabWin->setScaledContents(true);
    ui->cabWin->setPixmap(pixmap);
}

void CabinetListView::clearList()
{
    qDeleteAll(list_goods.begin(), list_goods.end());
    list_goods.clear();
    ui->list_select->clear();
    ui->list_goods->clear();
    ui->list_goods->setRowCount(0);
    ui->list_select->setRowCount(0);
    ui->fetch->setEnabled(false);
    selectMap.clear();
}

void CabinetListView::getCabList()
{
    list_goods = SqlManager::getGoodsList();
}

void CabinetListView::updateCabList(QString filter)
{
    list_filted.clear();
    goodsMap.clear();

    QSqlQuery query = SqlManager::goodsInfoList(filter);
    while(query.next())
    {
        goodsMap.insert(query.value(0).toString(), query.value(1).toString());
    }

    ui->list_goods->setRowCount(goodsMap.count());
    ui->list_goods->setColumnCount(1);

    int i=0;
    QMap<QString, QString>::Iterator it = goodsMap.begin();
    for(; it!=goodsMap.end(); it++)
    {
        qDebug()<<it.key();
        ui->list_goods->setItem(i, 0, new QTableWidgetItem(QIcon(":/image/image/icon_ar_left.png"), it.key()));
        i++;
    }
    emit searchGoods(filter);
//    config->searchByPinyin(filter);
    showCabView();
}

bool CabinetListView::packIsSelected(QString packId)
{
    int i=0;

    for(i=0; i<ui->list_select->rowCount(); i++)
    {
        CabinetListItem* item = (CabinetListItem*)ui->list_select->cellWidget(i, 0);
        if(packId == item->id())
            return true;
    }
    return false;
}

void CabinetListView::on_back_clicked()
{
    config->state = STATE_FETCH;
    emit searchGoods(QString());
    this->close();
}

void CabinetListView::on_list_goods_clicked(const QModelIndex &index)
{
//    Goods* info = list_filted[index.row()];
//    if(packIsSelected(info->packageId))
//        return;

    QString goodsStr = ui->list_goods->item(index.row(), index.column())->text();
    QString packageId = goodsMap[goodsStr];
    if(selectMap.contains(packageId))
        return;

    ui->msg->setText("");
    CabinetListItem* item = new CabinetListItem(goodsStr, packageId);
    QPoint addr = SqlManager::searchByPackageId(packageId);
    emit requireOpenCase(addr.x(), addr.y());
    selectMap.insert(packageId, item);

    int listSize = ui->list_select->rowCount();
    listSize++;
    ui->list_select->setRowCount(listSize);
    ui->list_select->setColumnCount(1);
    ui->list_select->setCellWidget(listSize-1,0,item);
}

void CabinetListView::on_fetch_clicked()
{
    QStringList fetchList;
    int i = 0;

    for(i=0; i<ui->list_select->rowCount(); i++)
    {
        CabinetListItem* item = (CabinetListItem*)ui->list_select->cellWidget(i, 0);
        fetchList<<item->getBarList();
    }
    ui->msg->setText("正在取出");
    ui->fetch->setEnabled(false);
    emit requireAccessList(fetchList, 1);
    qDebug()<<"networkState"<<networkState;
    if(!networkState)
    {
        fetchSuccess();
    }
}

void CabinetListView::search(int id)
{
//    int i = 0;
//    qDebug()<<groupSearch.button(id)->text()<<groupSearch.button(id)->isChecked();
    QString ch = ui->searchStr->text() + groupSearch.button(id)->text().at(0);
    ui->searchStr->setText(ch);

//    if(!groupSearch.button(id)->isChecked())
//    {
//        groupSearch.button(id)->setChecked(false);
//        config->clearSearch();
//        updateCabList();
//        return;
//    }

//    for(i=0; i<25; i++)
//    {
//        if(i != id)
//            groupSearch.button(i)->setChecked(false);
//    }
    emit searchGoods(ch);
//    config->searchByPinyin(ch);
    updateCabList(ch);
}

void CabinetListView::on_searchClear_clicked()
{
    ui->searchStr->clear();
    emit searchGoods(QString());
//    config->clearSearch();
    updateCabList();
}

void CabinetListView::recvScanData(QByteArray qba)
{
    QString fullCode = QString(qba);
    QString idCode = scanDataTrans(fullCode);

    if(qba.indexOf("-") == -1)
        return;
    if(selectMap.isEmpty())
        return;
    CabinetListItem* item = selectMap.value(idCode, NULL);
    if(item == NULL)
    {
        ui->msg->setText("添加失败");
        return;
    }
    if(item->addPackage(fullCode))
    {
        ui->msg->setText("添加成功");
        ui->fetch->setEnabled(true);
    }
    else
    {
        ui->msg->setText("添加失败");
    }
}

QString CabinetListView::scanDataTrans(QString code)
{
    int index = code.indexOf("-");
    if(index == -1)
        return code;

    code = code.right(code.size()-index-1);

    index = code.lastIndexOf("-");
    if(index == -1)
        return code;

    return code.left(index);
}
