#include "cabinetpanel.h"
#include "ui_cabinetpanel.h"

CabinetPanel::CabinetPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetPanel)
{
    ui->setupUi(this);
    ui->tableWidget->setFont(QFont("Helvetica")); //设置字体
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

/**************************
 * 函 数 名：Cabinet_lattice_num_set(int num)
 * 函数功能：添加item
 * 参   数：int num--药柜格子总数
 * 返 回 值：无
 * ***************************/
void CabinetPanel::Cabinet_lattice_num_set(int num)
{
    ui->tableWidget->setRowCount(num);
}

/**************************
 * 函 数 名：Cabinet_lattice_num_set(int num)
 * 函数功能：添加item
 * 参   数：int num--药柜格子总数
 * 返 回 值：无
 * ***************************/
int CabinetPanel::return_row_num()
{
    int num = ui->tableWidget->rowCount();
    return num;
}

CabinetPanel::~CabinetPanel()
{
    delete ui;
}
