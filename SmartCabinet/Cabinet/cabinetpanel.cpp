#include "cabinetpanel.h"
#include "ui_cabinetpanel.h"

CabinetPanel::CabinetPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetPanel)
{
    ui->setupUi(this);
    ui->tableWidget->setFont(QFont("Helvetica")); //设置字体
//    ui->tableWidget->setBackgroundColor(QColor(0,60,10)); //设置单元格背景颜色
//    ui->tableWidget->setTextColor(QColor(200,111,30)); //设置文字颜色
//    ui->tableWidget->sett
}

/**************************
 * 函 数 名：item_add
 * 函数功能：添加item
 * 参   数：int row--行
 *          int low--列
 *          QString str--内容
 * 返 回 值：无
 * ***************************/
void CabinetPanel::item_add(int row,int low,QString str)
{
    ui->tableWidget->setItem(row,low,new QTableWidgetItem(str));
}

CabinetPanel::~CabinetPanel()
{
    delete ui;
}
