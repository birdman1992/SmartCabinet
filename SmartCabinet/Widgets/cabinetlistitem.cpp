#include "cabinetlistitem.h"
#include "ui_cabinetlistitem.h"
#include <qpainter.h>
#include <QDebug>
#include "sql/sqlmanager.h"

CabinetListItem::CabinetListItem(QString goodsName, QString goodsId, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetListItem)
{
    ui->setupUi(this);
    name = goodsName;
    pack_id = goodsId;
    num = 0;
    ui->delete_2->hide();
    ui->name->setText(name);
    ui->num->setText(QString::number(num));
}

CabinetListItem::~CabinetListItem()
{
    delete ui;
}

void CabinetListItem::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

bool CabinetListItem::addPackage(QString bar)
{
    QString subBar = SqlManager::getPackageId(bar);//获取条码对应的包ID

    if(code_bar.indexOf(bar) != -1)
        return false;

    if(subBar == pack_id)
    {
        code_bar<<bar;
        num = code_bar.count();
        ui->num->setText(QString::number(num));
        return true;
    }
    else
        return false;
}

QStringList CabinetListItem::getBarList()
{
    return code_bar;
}

QString CabinetListItem::id()
{
    return pack_id;
}

/**
 * @brief CabinetListItem::scanDataTrans  数据库版本,已废弃
 * @param code
 * @return
 */
QString CabinetListItem::scanDataTrans(QString code)
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
