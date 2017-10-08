#ifndef CABINETPANEL_H
#define CABINETPANEL_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QList>
#include"Cabinet/cabinet.h"
namespace Ui {
class CabinetPanel;
}

class CabinetPanel : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetPanel(QWidget *parent = 0);
    ~CabinetPanel();
    //--添加item,即添加格子
    void item_add(int row,int low,QString str);
    //--设置药柜格子总数，添加格子要和总数配套使用，当格子大于总数添加无效
    void Cabinet_lattice_num_set(int num);
    int return_row_num();
    void panel_init(QList<Cabinet*> cabinets);

signals:
    void lattice_inf(int row);

public slots:
    void double_click(QTableWidgetItem* item);

private:
    Ui::CabinetPanel *ui;

};

#endif // CABINETPANEL_H
