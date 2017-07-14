#include "cabinetlistview.h"
#include "ui_cabinetlistview.h"
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QIcon>
#include <QFont>

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

void CabinetListView::setCabView(QFrame *cab)
{
    cabFrame = cab;
//    showCabView();
}

void CabinetListView::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.fillRect(this->rect(), QColor(255, 255, 255, 1));
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

}

void CabinetListView::showEvent(QShowEvent *)
{
    clearList();
    showCabView();
    getCabList();
    updateCabList();
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
    ui->list_goods->clear();
}

void CabinetListView::getCabList()
{
    int i,j,k;

    for(i=0; i<config->list_cabinet.count(); i++)
    {
        Cabinet* cab = config->list_cabinet[i];
        for(j=0; j<cab->list_case.count(); j++)
        {
            CabinetInfo* info = cab->list_case[j];
            for(k=0; k<info->list_goods.count(); k++)
            {
                GoodsInfo* goods = info->list_goods[k];
                list_goods<<new GoodsInfo(*goods);
            }
        }
    }


}

void CabinetListView::updateCabList(QChar filter)
{
    list_filted.clear();

    if(!filter.isUpper())
    {
        int i = 0;
        for(i=0; i<list_goods.count(); i++)
        {
            list_filted<<list_goods[i];
        }
    }
    else
    {
        int i = 0;
        for(i=0; i<list_goods.count(); i++)
        {
            if(list_goods[i]->Py.at(0) == filter)
                list_filted<<list_goods[i];
        }
    }

    ui->list_goods->setRowCount(list_filted.count());
    ui->list_goods->setColumnCount(1);

    int i;
    for(i=0; i<list_filted.count(); i++)
    {
        ui->list_goods->setItem(i, 0, new QTableWidgetItem(QIcon(":/image/image/icon_ar_left.png"), list_filted[i]->name));
    }
}

void CabinetListView::on_back_clicked()
{
    this->close();
}
